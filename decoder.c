/*

    Pixel Streaming (pxs) - decoder/presenter

    Written by Preston Corless

    usage: ./pxs [file].pxs

*/

#include <stdio.h>
#include <stdlib.h>
#include <GL/glut.h>
#include <GL/glu.h>


/* CONSTANTS */

#define HEADER_SIZE 8 // bytes


/* GLOBAL VARS */

FILE* f;
unsigned long imgWidth = 0, imgHeight = 0, virtualWidth = 0;

typedef struct Square {
  int x;
  int y;
  int w;
} Square;


/* GENERAL FUNCTIONS */

int getBase4Digs(int x) {
  // Obatins the abount of digits that an integer would occupy in base-4
  // notation
  int digs = 0;

  while (x > 0) {
    x >>= 2;
    digs++;
  }

  return digs;
}

int log2Ceil(int x) {
  int result = 1;
  while (x>0) {
    x >>= 1;
    result++;
  }
  return result;
}

float pow2(int exp) { // simple power of 2 calculator (works with negatives too)
  if (exp >= 0) {
    return 1 << exp;
  } else {
    return (1.0f / (1 << -exp));
  }
}

int virtualResolution(int x, int y) {
  // This function obtains the next-largest power of 2 above the max between x
  // and y. Useful for displaying variable-dimension images
  int max = x > y? x : y;
  return pow2(log2Ceil(max));
}


/* PXS-SPECIFIC FUNCTIONS */

void calcPos(int idx, Square* squ) {
  // Calculates the location of pixel idx on a virtual screen with dimensions
  // virtualWidth, and stores its location (and size) in squ

  int base4digs = getBase4Digs(idx);
  squ -> w = pow2(-base4digs) * virtualWidth;

  for (int i = 0; i < base4digs; i++) {
    char channels = (idx >> (i * 2)) & 3;
    // The above is JS implementation's "newVal".
    // It is simply the "address" within subdivision i that pixel idx falls in.
    // It is a number between 0 and 3.

    int inc = pow2(-(i + 1)) * virtualWidth;
    // The amount that a pixel would need to be shifted over from the origin
    // for being in subdivision i

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
  // Checks if a given idx is in bounds of the screen.
  // (It uses a fake square to do the calculations)
  // NOTE: There is a huge opportunity for optimization here, since if the pixel
  // under consideration is in bounds, this exact calculation happens again in
  // order to determine its location, later on.

  Square fakeSquare;
  fakeSquare.x = fakeSquare.y = 0;
  calcPos(idx, &fakeSquare);
  return (fakeSquare.x < imgWidth && fakeSquare.y < imgHeight) ? 1 : 0;
}

void displayPixel(int idx, float color[3]) {
  // Actually renders the pixel on the screen, yay

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

   while (!feof(f)) {
    // pxs's read-display loop

    curr = fgetc(f);
    int idx = head % 3; // destination color channel
    color[idx] = curr;
    head++;

    if ((head % 3) == 0) { // whenever the index goes back to 0 (finishes color)
      displayPixel(pixelIdx, color);
      do {
        pixelIdx++;
      } while (!inBounds(pixelIdx));
    }
  }

  // Flush drawing command buffer to make drawing happen as soon as possible.
  glFlush();
}

int main(int argc, char** argv) {

  // File I/O checks

  if (argc < 2) {
    printf("Usage: %s [image].pxs\n", argv[0]);
    return 1;
  }

  f = fopen(argv[1], "r");
  if (f == NULL) {
    printf("Failed to open \"%s\": File not found.\n", argv[1]);
    return 1;
  }

  // Determine certain global values

  for (int i = 0; i < 4; i++) {
    imgWidth = (imgWidth << 8) | fgetc(f);
  }
  for (int i = 0; i < 4; i++) {
    imgHeight = (imgHeight << 8) | fgetc(f);
  }
  virtualWidth = virtualResolution(imgWidth, imgHeight);


  // Buncha OpenGl setup stuff

  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);

  glutInitWindowSize(imgWidth, imgHeight);
  glutCreateWindow("pxs Viewer");

  glClearColor(1, 0, 0, 0);
  gluOrtho2D(0, imgWidth, imgHeight, 0);

  glutDisplayFunc(display);

  glutMainLoop();
}
