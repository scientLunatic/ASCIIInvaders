#include<stdio.h>
#include<stdlib.h>
#include<conio.h>

#define PROMPT_WIDTH 80
#define PROMPT_HEIGHT 24

typedef struct{
    char** elements;
    int height;
    int width;
    int vOffset;
    int hOffset;
    int size;
}image;

int testfunctions();
int load(FILE* source, image* target);
int clear(image* target);
int display(image screen, int width, int height);
int overlay(image background, image foreground, image* target);
void genBorder();

void printchars();

int main(void)
{
    //testfunctions();
    printchars();
}

///ASCII table
void printchars()
{
    int i, j;
    for(i = 0; i < 256; i++)
    {
        if(i%8== 0)
        {
            for(j = 0; j < 78; j++)
                printf("\xC4");
            printf("\n");
        }

        printf("\xB3%03d '%c'\xB3 ", i, i);
    }
    for(j = 0; j < 78; j++)
        printf("\xC4");
    printf("\n");
}

int testfunctions()
{
    image background = {0};
    image foreground = {0};
    image logo = {0};

    load(fopen("./files/background.txt", "r"), &background);
          //printf("[background: %d, %d, %d]\n", background.height, background.width, background.size);
          display(background, background.height, background.width);
    load(fopen("./files/border.txt", "r"), &foreground);
          //printf("[border: %d, %d, %d]\n", foreground.height, foreground.width, foreground.size);
          display(foreground, foreground.height, foreground.width);
    load(fopen("./files/cool.txt", "r"), &logo);
          //printf("[logo: %d, %d, %d]\n", logo.height, logo.width, logo.size);
          display(logo, logo.height, logo.width);

    overlay(background, foreground, &background);

        //printf("[screen with border]: %d, %d, %d\n", background.height, background.width, background.size);
        if(display(background, background.height, background.width))
            while(!kbhit()){};

    clear(&background);
    clear(&foreground);

    return 1;
}

int clear(image* target)
{
    int i;
    for(i = 0; i < target->height; i++)
    {
        free(target->elements[i]);
        target->elements[i] = NULL;
    }

    target->height = 0;
    target->size = target->width;
    free(target->elements);
    target->elements = NULL;
    target->width = 0;
    target->size = 0;
}

///Places foreground over background.
int overlay(image background, image foreground, image* target)
{
    int i;

    target->elements = (char**)calloc(background.height, sizeof(char*));
    for(i = 0; i < background.height; i++)
        target->elements[i] = (char*)calloc(background.width+1, sizeof(char));

    for(i = 0; i < background.size; i++)
    {
        if(foreground.elements[(int)(i/background.width)][i%background.width] != '\0')
        {
            target->elements[(int)(i/background.width)][(i)%background.width] = (   foreground.elements[(int)(i/background.width)][i%background.width] == ' ')?
                                                                                    background.elements[(int)(i/background.width)][i%background.width]:
                                                                                    foreground.elements[(int)(i/background.width)][i%background.width];
        }//--------------------------------------------------------------------------------------------------------------------------------------------|
    }
    /*printf("\"debug\"\n\t%s: %d\n\t%s: %d\n\t%s: %d\n\t%s: %d\n\t%s: %d\n\n",
           "i", i, "height", background.height,
           "width: ", background.width,
           "indexline", (int)(i/background.width),
           "indexcol", (i-1)%(background.width));*/

    target->height = background.height;
    target->width = background.width;
    target->hOffset = background.hOffset;
    target->vOffset = background.vOffset;
    target->size = background.size;
}

int load(FILE* source, image* target)
{
    int i, height = 0, width = 0, hMax = 0, wMax = 0;
    char c = 0;

    target->elements = (char**)calloc(1, sizeof(char*));//line to be filled
    target->elements[0] = (char*)calloc(1, sizeof(char));//char to be filled

    while((c = getc(source))!= EOF)
    {
        if(c == '#')
        {
            while(c != '\n')
            {
                c = getc(source);
                if(c == EOF)
                    break;
            }
        }
        if(c == '\n')
        {
            //printf("%c", c);
            target->elements[height][width] = '\0';
            width = 0;
            height++;
            target->elements = (char**)realloc(target->elements, (height+1)*sizeof(char*));
            target->elements[height] = (char*)calloc(width+1, sizeof(char));
        }
        else
        {
            //printf("%c", c);
            target->elements[height][width] = c;
            width++;
            if(wMax < width)//{
                wMax = width;
                //printf("%d\n", wMax);
            //}
            target->elements[height] = (char*)realloc(target->elements[height], width+1*sizeof(char));
        }
    }
    target->elements[height][width] = '\0';

    target->width = wMax;
    target->height = height+1;
    target->vOffset = 0;
    target->hOffset = 0;
    target->size = wMax*(height+1);
    //printf("\n\"debug\":\n\t height:%d\n\t width:%d\n\n", height+1, wMax);

    return 1;
}

int display(image screen, int height, int width)
{
    if((height < 1)||(width < 1)||((screen.width - screen.hOffset) < 1)||((screen.height - screen.vOffset) < 1))
        return 0;

    //Attempt at O(n). I have no idea if this really worked.
    int i;

    /*If any of the to-be-printed lengths are greater the size of the prompt,
        use the prompt size instead, else, use the length max. This prevents
        indexes out of bounds.*/
    int wMax = ((screen.width - screen.hOffset) <= PROMPT_WIDTH)? (screen.width - screen.hOffset): PROMPT_WIDTH,
    hMax = ((screen.height - screen.vOffset) <= PROMPT_HEIGHT)? (screen.height - screen.vOffset): PROMPT_HEIGHT;

    //Check for redundancy

    if(width == PROMPT_WIDTH)
        for(i = 0; i < height*PROMPT_WIDTH; i++)
        {
            if(screen.elements[(int)(i/width)][i%width] != '\0')
                printf("%c", screen.elements[(int)(i/width)][i%width]);//smaller than prompt(both)
            else
            {
                printf("\n");
                i += (width-1)-(i % width);
            }
        }
    else
        for(i = 0; i < height*width; i++)
        {
            if(((i % width) != (width-1)) && (screen.elements[(int)(i/width)][i%width] != '\0'))
                printf("%c", screen.elements[(int)(i/width)][i%width]);//smaller than prompt(both)
            else
            {
                printf("%c\n", screen.elements[(int)(i/width)][i%width]);
                i += (width-1)-(i % width);
            }
        }
    return 1;
}

void genBorder()
{
    int i, j;

    FILE* overlay = fopen("./files/border.txt", "w");

    for(i = 0; i < 24; i++){
        for(j = 0; j < 80; j++){
            fputc((( (i==0) || (i==24-1))?
                    ((j==0) || (j==80-1))?
                        '*'
                        :'-'
                    :((j==0) || (j==80-1))?
                        '|'
                        :' '), overlay);
        }
        fputc('\n', overlay);
    }

    fclose(overlay);
}
