/*

    Pixel Streaming (pxs) - decoder/presenter

    Written by Preston Corless

*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <GL/gl.h>
#include <GL/glut.h>

#define RESOLUTION 256

typedef struct {
    int x;
    int y;
    int width;
    int height;
} Rectangle;

char filePath[1000] = "";

int log4int(int x) { return log(x) / log(4); }

void calcPos(int idx, Rectangle* rect) {
    int base4digs = idx? log4int(idx) + 1 : 0; // amount of sigfigs for the index if put in base-4 notation
    rect -> width = rect -> height = pow(2, -base4digs) * RESOLUTION;
    for (int i = 0; i < base4digs; i++) {
        char channels = (idx & (3 << (i * 2))) >> (i * 2); // JS implementation's "newVal"
        float inc = pow(2, -(i + 1)) * RESOLUTION;
        switch (channels) {
            case 0:
                break;
            case 3:
                rect -> x += inc;
                break;
            case 2:
                rect -> y += inc;
                break;
            case 1:
                rect -> x += inc;
                rect -> y += inc;
                break;
        }
    }
}

void displayPixel(int idx, unsigned char color[3]) {
    Rectangle rect;

    rect.x = rect.y = 0;

    calcPos(idx, &rect);

//    printf("%d %d %d\n", color[0], color[1], color[2]);

    glBegin(GL_QUADS);
    glColor3f(color[0]/255, color[1]/255, color[2]/255);
    glVertex2i(rect.x, rect.y);
    glVertex2i(rect.x + rect.width, rect.y);
    glVertex2i(rect.x + rect.width, rect.y + rect.height);
    glVertex2i(rect.x, rect.y + rect.height);


    // glPointSize(rect.width);
    // glVertex2i(rect.x, rect.y);
//    glRecti(rect.x, rect.y, rect.x + rect.width, rect.y + rect.height);
    glEnd();
    glutSwapBuffers();
}

void init() {
    glClearColor( 0.5, 0.5, 0.5, 1 );  // (In fact, this is the default.)
}

void handleFile() {
    glClearColor( 0.5, 0.5, 0.5, 1 );  // (In fact, this is the default.)
    glClear(GL_COLOR_BUFFER_BIT);

    FILE* f = fopen(filePath, "r");
    
    unsigned char curr; // current byte being read
    int head = 0; // read-head for the file
    int pixelIdx = 0;
    unsigned char color[3] = {0}; // RGB values

    do {
        curr = fgetc(f);
        int idx = head % 3; // destination color channel
        color[idx] = curr;
        head++;
        if ((head % 3) == 0) { // whenever the index goes back to 0 (finishes color)
            displayPixel(pixelIdx, color);
            pixelIdx++;
        }
    } while (head <= 49152);
}

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Usage: %s [image].pxs\n", argv[0]);
        return 1;
    }

    init();

    strcpy(filePath, argv[1]);

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(RESOLUTION, RESOLUTION);
    glutCreateWindow("pxstream viewer - glut");

    glutDisplayFunc(handleFile);

    glutMainLoop();

    return 0;
}
