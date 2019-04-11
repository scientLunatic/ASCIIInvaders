#include "image.h"

/**
*   Loads FILE into image,
*   allocates dynamically depending on file size; sets null-terminators and theoretically creates uneven matrices.
*   returns boolean values for success.
*/
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

    return 1;
}

///Renders any properly formatted screen
int display(image screen, int height, int width)
{
    system("cls");
    if((height < 1)||(width < 1)||((screen.width - screen.hOffset) < 1)||((screen.height - screen.vOffset) < 1))
        return 0;

    //Attempt at O(n). I have no idea if this really worked.
    int i;

    /*If any of the to-be-printed lengths are greater the size of the prompt,
    use the prompt size instead, else, use the length max. This prevents
    indexes out of bounds.*/
    int wMax = ((screen.width - screen.hOffset) <= PROMPT_WIDTH)? (screen.width - screen.hOffset): PROMPT_WIDTH,
    hMax = ((screen.height - screen.vOffset) <= PROMPT_HEIGHT)? (screen.height - screen.vOffset): PROMPT_HEIGHT;

{
    if(width == PROMPT_WIDTH)
        for(i = 0; i < height*PROMPT_WIDTH; i++)
        {
            if(screen.elements[(int)(i/width)][i%width] != '\0')
                printf("%c", screen.elements[(int)(i/width)][i%width]);//can be shorter than prompt
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
                printf("%c", screen.elements[(int)(i/width)][i%width]);//thinner than prompt
            else
            {
                printf("%c\n", screen.elements[(int)(i/width)][i%width]);
                i += (width-1)-(i % width);
            }
        }
    }
    fflush(stdout);
    return 1;
}

void clear(image* target)
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

int mergeImg(image background, image foreground, image* target, int vOffset, int hOffset)
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

    target->height = background.height;
    target->width = background.width;
    target->hOffset = background.hOffset;
    target->vOffset = background.vOffset;
    target->size = background.size;
}



/* old functions
void oldDisplay(image screen)
{
    int i, j;
    system("cls");
    for(i = screen.lu; i < screen.height; i++){
        for(j = screen.lu; j < screen.width; j++)
            printf("%c", screen.elements[i][j]);
        printf("\n");
    }
    fflush(stdout);
}

image oldLoad()
{
    FILE* level = fopen("./files/main.txt", "r");
    char** target = (char**)malloc(24*sizeof(char*));
    int i;

    for(i = 0; i < 24; i++)
    {
        target[i] = (char*)malloc(81);
        if(fgets(target[i], 81, level) == NULL)
        {
            free(target[i]);
            break;
        } else {
            if(target[i][strlen(target[i])-1] == '\n')
                target[i][strlen(target[i])-1] = '\0';
        }
    }
    fclose(level);

    return (image){target, 0, strlen(target[0]), i, i*(strlen(target[0]))};
}
*/
