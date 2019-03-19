#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "tga.h"
#include "model.h"

void line(tgaImage *image, int x0, int y0, int x1, int y1, tgaColor color);
Model *scaleModel(Model *model, double scale);
void grid(tgaImage *image, Model *model);
void swap(int *a, int *b);
void Triangle(tgaImage *image, int x0, int y0, int x1, int y1, int x2, int y2, tgaColor color);

int main(int argc, char *argv[]){
    int sysExit = 0;
    Model *model = loadFromObj(argv[1]);
    scaleModel(model, strtod(argv[3], NULL));
    tgaImage *image = tgaNewImage(800, 800, RGB);
    grid(image, model);
    if (-1 == tgaSaveToFile(image, argv[2])) {
        perror("tgaSaveToFile");
        sysExit = -1;
    }
    if (argc < 4){
        perror("Need more parameters! ./... *.obj *.tga");
        sysExit = -1;
    }
    
    tgaFreeImage(image);
    freeModel(model);
    return sysExit;
}

void line(tgaImage *image, int x0, int y0, int x1, int y1, tgaColor color){
    char steep = 0;
    if (abs(x0 - x1) < abs(y0 - y1)){
        swap(&x0, &y0);
        swap(&x1, &y1);
        steep = 1;
    }
    if (x0 > x1){
        swap(&x0, &x1);
        swap(&y0, &y1);
    }

    int errorAccumulation = 0;
    int deltaError = 2 * abs(y1 - y0);
    int y = y0;
    for (int x = x0; x <= x1; x++){
        if (steep == 1){
            tgaSetPixel(image, (unsigned int)y, (unsigned int)x, color);
        } else{
            tgaSetPixel(image, (unsigned int)x, (unsigned int)y, color);
        }
        errorAccumulation += deltaError;

        if (errorAccumulation > (x1 - x0)){
            y += (y1 > y0?1:-1);
            errorAccumulation -= 2 * (x1-x0);
        }
    }
} 

void Triangle(tgaImage *image, int x0, int y0, int x1, int y1, int x2, int y2, tgaColor color) {
    int xa, xb;

    if(y0 > y2) {
        swap(&y0, &y2);
        swap(&x0, &x2);
    }
    if(y0 > y1) {
        swap(&y0, &y1);
        swap(&x0, &x1);
    }
    if(y1 > y2) {
        swap(&y1, &y2);
        swap(&x1, &x2);
    }

    if(y0 == y2) {
        printf("It's line\n");
        return;
    }

    for (int y = y0; y <= y1; y++){
        if ((y1 - y0) != 0){
        xa = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
        }else{
            xa = x0;
        }
        if ((y2 - y0) != 0){
            xb = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        }else{
            xb = x0;
        }
        if (xa > xb){
            swap(&xa, &xb);
        }
        for (int x = xa; x <= xb; x++){
            tgaSetPixel(image, x, y, color);
        }
    }

    for(int y = y1; y <= y2; y++){
        if ((y1 - y0) != 0){
            xa = x0 + (x2 - x0) * (y - y0) / (y2 - y0);
        }else{
            xa = x0;
        }
        if ((y2 - y0) != 0){
            xb = x1 + (x2 - x1) * (y - y1) / (y2 - y1 + 1);
        }else{
            xb = x0;
        }
        if (xa > xb){
            swap(&xa, &xb);
        }
        for (int x = xa; x <= xb; x++){
            tgaSetPixel(image, x, y, color);
        }
    }
}


Model *scaleModel(Model *model, double scale){
    for (unsigned i = 0; i < model->nvert; i++){
        for(unsigned j = 0; j < 3; j++){
            (model->vertices[i])[j] = (model->vertices[i])[j] * scale;
        }
    }
    return model;
}

void grid(tgaImage *image, Model *model){
    int i, j; tgaColor white = tgaRGB(255, 255, 255);

    for (i = 0; i < model->nface; ++i){
        int screenCoordinats[3][2];
        for (j = 0; j < 3; ++j){
            Vec3 *v = &(model->vertices[model->faces[i][3*j]]);
            screenCoordinats[j][0] = ((*v)[0] + 1) * image->width / 2;
            screenCoordinats[j][1] = (1 - (*v)[1]) * image->height / 2;
        }
        /*for (j = 0; j < 3; ++j){
            line(image, screenCoordinats[j][0], screenCoordinats[j][1],
                 screenCoordinats[(j + 1) % 3][0], screenCoordinats[(j + 1) % 3][1], white);
        }*/
        int j = 0;
        tgaColor randColor = tgaRGB(rand() % 255, rand() % 255, rand() % 255);
        Triangle(image, screenCoordinats[j][0],screenCoordinats[j][1],
            screenCoordinats[(j+1)%3][0], screenCoordinats[(j+1)%3][1],
            screenCoordinats[(j+2)%3][0], screenCoordinats[(j+2)%3][1], randColor);
    }

}

void swap(int *a, int *b) {
    int t = *a;
    *a = *b;
    *b = t;
}
