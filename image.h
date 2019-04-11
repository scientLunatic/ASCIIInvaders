#ifndef IMAGE_H_INCLUDED
#define IMAGE_H_INCLUDED

    #include<stdlib.h>
    #include<stdio.h>

    #define PROMPT_WIDTH 80
    #define PROMPT_HEIGHT 24

    typedef struct{
        char** elements;
        int height;
        int width;
        int hOffset;
        int vOffset;
        int size;
    }image;

    typedef struct{
        int x;
        int y;
    }point;


        int load(FILE* source, image* target);
            int display(image screen, int height, int width);

            int mergeImg(image background, image foreground, image* target, int vOffset, int hOffset);

            void clear(image* target);


    //void oldDisplay(image screen);
    //image oldLoad();

#endif // IMAGE_H_INCLUDED
