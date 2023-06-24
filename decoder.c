/*

    Pixel Streaming (pxs) - decoder/presenter

    Written by Preston Corless

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>
#include <GL/glu.h>

#define HEADER_SIZE 8 // bytes

FILE* f;
unsigned long imgWidth = 0, imgHeight = 0, virtualWidth = 0;

typedef struct Square {
  int x;
  int y;
  int w;
} Square;

int getBase4Digs(int x) {
  if (x == 0) {return 1;}

  int digs = 0;
  while (x > 0) {
    x >>= 2;
    digs++;
  }
  return digs;
}

float pow2(int exp) {
  if (exp >= 0) {
    return 1 << exp;
  } else {
    return (1.0f / (1 << -exp));
  }
}

int virtualRes(int x, int y) {
  int max = x > y? x : y;
  return pow2(ceil(log2(max)));
}

void calcPos(int idx, Square* squ) {
  int base4digs = getBase4Digs(idx);
  squ -> w = pow2(-base4digs) * virtualWidth;
  for (int i = 0; i < base4digs; i++) {
    char channels = (idx & (3 << (i * 2))) >> (i * 2); // JS implementation's "newVal"
    float inc = pow2(-(i + 1)) * virtualWidth;
    switch (channels) {
      case 0:
        break;
      case 3:
        squ -> x += inc;
        break;
      case 2:
        squ -> y += inc;
        break;
      case 1:
        squ -> x += inc;
        squ -> y += inc;
        break;
    }
  }
}

char inBounds(int idx) {
  Square fakeSquare;
  fakeSquare.x = fakeSquare.y = 0;
  calcPos(idx, &fakeSquare);
  return (fakeSquare.x < imgWidth && fakeSquare.y < imgHeight) ? 1 : 0;
}

void displayPixel(int idx, float color[3]) {
  Square square;
  square.x = square.y = square.w = 0;

  calcPos(idx, &square);

  glPointSize(square.w);


  glBegin(GL_POINTS);

	glColor3f(color[0] / 255, color[1] / 255, color[2] / 255);
  glVertex2f(square.x + 1, square.y + 1);

	glEnd();
}

void display() {

  // Set every pixel in the frame buffer to the current clear color.
  glClear(GL_COLOR_BUFFER_BIT);
  fseek(f, HEADER_SIZE, SEEK_SET);


  unsigned char curr; // current byte being read
  int head = 0; // read-head for the file
  int pixelIdx = 0;
  float color[3] = {0}; // RGB values


  do {
    curr = fgetc(f);
    int idx = head % 3; // destination color channel
    color[idx] = curr;
    head++;
    if ((head % 3) == 0) { // whenever the index goes back to 0 (finishes color)
      displayPixel(pixelIdx, color);
      do {pixelIdx++;} while (!inBounds(pixelIdx));
    }
  } while (!feof(f));

  // Flush drawing command buffer to make drawing happen as soon as possible.
  glFlush();
}

int main(int argc, char** argv) {

  if (argc < 2) {
    printf("Usage: %s [image].pxs\n", argv[0]);
    return 1;
  }

  f = fopen(argv[1], "r");
  if (f == NULL) {
    printf("Failed to open \"%s\": File not found.\n", argv[1]);
    return 1;
  }

  for (int i = 0; i < 4; i++) {
    imgWidth = (imgWidth << 8) | fgetc(f);
  }
  for (int i = 0; i < 4; i++) {
    imgHeight = (imgHeight << 8) | fgetc(f);
  }
  virtualWidth = virtualRes(imgWidth, imgHeight);

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  glutInitWindowSize(imgWidth, imgHeight);
  glutCreateWindow("pxs Viewer");

  glClearColor(1, 0, 0, 0);
  gluOrtho2D(0, imgWidth, imgHeight, 0);

  glutDisplayFunc(display);

  glutMainLoop();
}
