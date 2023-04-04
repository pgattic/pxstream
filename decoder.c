/*

    Pixel Streaming (pxs) - decoder/presenter

    Written by Preston Corless

*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <raylib.h>

#define RESOLUTION 256

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
    Color pixelColor;
    Rectangle rect;

    pixelColor.r = color[0];
    pixelColor.g = color[1];
    pixelColor.b = color[2];
    pixelColor.a = 255;

    rect.x = rect.y = 0;

    calcPos(idx, &rect);

    DrawRectangleRec(rect, pixelColor);
}

void handleFile(FILE* f) {
    unsigned char curr; // current byte being read
    int head = 0; // read-head for the file
    int pixelIdx = 0;
    unsigned char color[3] = {0}; // RGB values

    BeginDrawing();

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

    EndDrawing();
}

int main(int argc, char** argv) {

    if (argc < 2) {
        printf("Usage: %s [image].pxs\n", argv[0]);
        return 1;
    }

    FILE* f = fopen(argv[1], "r");

    InitWindow(RESOLUTION, RESOLUTION, "pxstream viewer - raylib");

    handleFile(f);

    SetTargetFPS(1);
    while (!WindowShouldClose()){ // Yeah, a hackjob, I know. It works though
        BeginDrawing();
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
