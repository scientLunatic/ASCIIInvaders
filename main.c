#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>
#include <string.h>
#include <stdbool.h>
#include <conio.h>
#include <stdarg.h>
#include "misc.h"
#include "image.h"

//signatures data declarations and nesting 'trees'

#define LBLOCK '\xb0'
#define MBLOCK '\xb1'
#define DBLOCK '\xb2'
#define BBLOCK '\xdb'
#define UPERHALF '\xdc'
#define LOWERHALF '\xdf'
#define BLANK '\xff'


//key and defaults mnemonic setting
enum
{
    ESC       = 27,
    ENTER     = 13,
    KEY_DIRECTION = -32,
    KEY_UP    = 72,
    KEY_LEFT  = 75,
    KEY_RIGHT = 77,
    KEY_DOWN  = 80,
    SPACEBAR  = 32,

    DEFAULT_WINDOW_WIDTH = 79,
    DEFAULT_WINDOW_HEIGHT = 24
};

//char matrix of N size
//left-upper corner, diagonal offset from elements's [0][0].
//right-upper corner, column number.
//left-lower corner, line number
/*right-lower corner, matrix column and line number product.->\
//could become the offset's offset from the diagonal.          |
//(0 = diag, n>0 = line over col, n<0 = col over line)       */

typedef struct list list;
struct list{
    void* element;
    list* next;
};


///Loads initial screen where none is present, ergo: initializes display.
///An proper equivalent should be a load() of the background.txt merged with border.txt
void initDisp(image* init)
{
    image borders;

    load(fopen("./files/background.txt", "r"), init);
    load(fopen("./files/border.txt", "r"), &borders);
    mergeImg(*init, borders, init, 0, 0);
    clear(&borders);
}


void menu();
    image initMen(size_t cols, size_t lines);

    void options();
    void credits(char** menu, FILE* cred);
    void game();
        int logic(char* mode, char* arg);


int main()
{

    //use sparingly, this is dangerous.

    char buffer[PROMPT_WIDTH*PROMPT_HEIGHT] = {0};
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));

    //setvbuf() changes the buffer size so that printf() can write to the command window faster,
    //but(in this case) it only prints with a fflush() or meeting the full 1896 chars on buffer size.
    //It's a major improvement on speed.

    logic("setting", "color");

    menu();

    return 0;
}

///game loop
void game()
{
    image bg;
    image full;

    load(fopen("./files/background.txt", "r"), &bg);
    load(fopen("./files/background.txt", "r"), &full);

    char* player = "\xDA\xC1\xBF";                                          //player 'sprite'
    point pPos = {full.width/2-1, 23};                                      //place player on center of last line
    char shot = '^';
    point* sPos = NULL;
    char enemy = '\xCA';                                                    //enemy 'sprite'
    point* ePos = NULL;
    int step = 1;

    bool stay = true;                                                       //run game loop flag
    int i, j, sNum = 0, eNum = 8;

    ePos = calloc(eNum, sizeof(point));
    sPos = calloc(1, sizeof(point));
    for(i = 0; i < eNum; i++)
    {
        ePos[i].x = ((i % full.width)*full.width) / 8;                      //initialize enemy locations
        ePos[i].y = (((int)i/full.width)*full.height) / 10;
    }

    centerCpy(full.elements[pPos.y], player);

    while(stay && (eNum > 0))
    {
        checkFocus();

        mergeImg(full, bg, &full, 0, 0);

        strncpy(full.elements[pPos.y] + pPos.x, player, sizeof(player)-1);

        for(i = 0; i < eNum; i++)
            full.elements[ePos[i].y][ePos[i].x] = enemy;                        //place enemies on screen

        if(sNum != 0)
            for(i = 0; i < sNum; i++)
                full.elements[sPos[i].y][sPos[i].x] = shot;                     //places shots on screen

        display(full, 24, 80);
        delay(10);

        printf("sNum:%d|eNum:%d|sPos.x:%d|sPos.y:%d", sNum, eNum, sPos[sNum-1].x, sPos[sNum-1].y);
        fflush(stdout);

        switch(logic("eval_key", NULL))
        {
            case 0:
                break;
            case 1:
            case 2:
                stay = !stay;
                break;
            case 3:
            case 7:
                pPos.x = (pPos.x+1 < 77)? pPos.x+1: pPos.x;
                break;
            case 4:
            case 8:
                pPos.x = (pPos.x-1 > 0)? pPos.x-1: pPos.x;
                break;
            case 6:
                realloc(sPos, sizeof(point)*(sNum+1));                      //realloc set location and increment sNum
                sPos[sNum].x = pPos.x + 1;
                sPos[sNum].y = pPos.y;
                sNum++;
                break;
            default:
                system("color 0C");                                         //unrecognized input
                pause();
                logic("setting", "color");
        }

        //rewrite enemy travel
        if(eNum != 0)
        {
            for(i = 0; i < eNum; i++)
            {
                if((ePos[i].y & 1) != 0)                                    //
                {
                    if((ePos[i].x + step) < full.width - 1){
                        ePos[i].x += step;
                    }else if((ePos[i].y + 1) < (full.height-1)){
                        ePos[i].y++;
                    }
                    else
                    {
                        for(j = i; j < eNum; j++)
                        {
                            ePos[j].x = ePos[j+1].x;
                            ePos[j].y = ePos[j+1].y;
                        }
                        eNum--;
                        ePos = realloc(ePos, sizeof(point)*(eNum));
                    }
                }else
                {
                    if((ePos[i].x - step) > 0)
                        ePos[i].x -= step;
                    else if((ePos[i].y + 1) < (full.height-1))
                        ePos[i].y++;
                    else
                    {
                        for(j = i; j < eNum; j++)
                        {
                            ePos[j].x = ePos[j+1].x;
                            ePos[j].y = ePos[j+1].y;
                        }
                        eNum--;
                        ePos = realloc(ePos, sizeof(point)*(eNum));
                    }
                }
            }
        }
        else break;
        //rewrite shot collision and travel
        if(sNum != 0)                                                       //there is a bullet
        {
            for(i = 0; i < sNum; i++)                                       //for each bullet
            {
                if((sPos[i].y - step) > 0)
                {
                    for(j = 0; j < step; j++)                               //every tile between shot and destination
                        if(full.elements[sPos[i].y-j][sPos[i].x] == enemy)  //if tile at bullet col and j row is an enemy
                            ePos[i] = (point){2, full.height-2};            //enemy hit, destroy shot and enemy.
                    sPos[i].y -= step;
                }
                else
                {
                    if(sNum > 1)
                    {
                        for(j = 0; j < sNum-1; j++)
                        {
                            sPos[j].x = sPos[j+1].x;
                            sPos[j].y = sPos[j+1].y;                        //top of screen hit, destroy shot.(or the top of the screen, that would be fun).
                        }

                        sNum--;
                        sPos = (point*)realloc(sPos, (sNum)*sizeof(point));
                        printf("What I did:\n");
                        for(j = 0; j < sNum-1; j++)
                        {
                            printf("\tShot %d:\n\t x:%d\n\t y:%d", j+1, sPos[j].x, sPos[j].y);
                        }
                        fflush(stdout);
                    }
                    else
                    {
                        sNum--;
                        free(sPos);
                        sPos = calloc(1, sizeof(point));
                    }
                }
            }
        }
    }


    clear(&bg);
    clear(&full);
}

///Returns useful integers for logic and calls certain functions; takes processing mode and arguments.
int logic(char* mode, char* arg)
{
    if(!strcmp(mode, "eval_key"))
    {
        if(kbhit())
        {
            char key = getch();
            switch(key)
            {
                case ESC:
                    return 1;
                case 'x':
                case 'X':
                    return 2;
                case 'd':
                case 'D':
                    return 3;
                case 'a':
                case 'A':
                    return 4;
                case ENTER:
                    return 5;
                case 'e':
                case 'E':
                case SPACEBAR:
                    return 6;
                case KEY_DIRECTION:
                    switch(key = getch())
                    {
                        case KEY_RIGHT:
                            return 7;
                        case KEY_LEFT:
                            return 8;
                        case KEY_UP:
                            return 6;
                        case KEY_DOWN:
                            return 2;
                    }
                default:
                    return 9;
            }

            dump();
        }
    }
    if(!strcmp(mode, "setting"))
    {
        FILE* cfg = fopen("./files/settings.ini", "r+");
        if(!strcmp(arg, "color"))
        {
            char* option = malloc(30);
            while(fscanf(cfg, "%s", option) != EOF)
                if(strcmp(option, "color"))
                {
                    fscanf(cfg, "%s", option);
                        if(!strcmp(option, "matrix"))
                        {
                            option = "color 0A";
                            system(option);
                        }
                        else if(!strcmp(option, "paper"))
                        {
                            option = "color 78";
                            system(option);
                        }
                        else if(!strcmp(option, "turbo"))
                        {
                            option = "color 1E";
                            system(option);
                        }
                        else if(!strcmp(option, "contrast"))
                        {
                            option = "color C9";
                            system(option);
                        }
                        else if(!strcmp(option, "ice"))
                        {
                            option = "color 17";
                            system(option);
                        }
                        else if(!strcmp(option, "scorpion"))
                        {
                            option = "color 0E";
                            system(option);
                        }
                        else if(!strcmp(option, "camo"))
                        {
                            option = "color 26";
                            system(option);
                        }
                        else if(!strcmp(option, "blood"))
                        {
                            option = "color 4C";
                            system(option);
                        }
                        else if(!strcmp(option, "haunted"))
                        {
                            option = "color 40";
                            system(option);
                        }
                        else if(!strcmp(option, "magic"))
                        {
                            option = "color 5C";
                            system(option);
                        }
                        else if(!strcmp(option, "pimp"))
                        {
                            option = "color 5D";
                            system(option);
                        }
                        else if(!strcmp(option, "gold"))
                        {
                            option = "color 6E";
                            system(option);
                        }
                        else if(!strcmp(option, "ink"))
                        {
                            option = "color 80";
                            system(option);
                        }
                        else if(!strcmp(option, "toxic"))
                        {
                            option = "color EA";
                            system(option);
                        }
                        else if(!strcmp(option, "plutonium"))
                        {
                            option = "color A8";
                            system(option);
                        }
                        else if(!strcmp(option, "focus"))
                        {
                            option = "color FC";
                            system(option);
                        }
                        else
                        {
                            char* out = malloc(9);
                            sprintf(out, "color %s\n", option);
                            system(out);
                            free(out);
                        }
                    free(option);
                    fclose(cfg);
                    return 1;
                }
            fclose(cfg);
            free(option);
            return 0;
        }
        fclose(cfg);
    }
    /*if(!strcmp(mode, "debug"))
    {
        if();
    }*/
    return 0;
}

///Calls and handles menu screen. Possibly, the logic and visual functions can be made to handle respective operations on scope.
void menu(){
    image title = initMen(80, 24);
    char** opts = retrieveOptions(title.elements[21]+1, '\xB0', '|');
    int option = 0;
    *opts[option] = 178;

    bool stay = true;

    while(stay)
    {
        checkFocus();
        display(title, 24, 80);
        switch(logic("eval_key", NULL))
        {
            case 0:
                break;
            case 1:
            case 2:
                stay = !stay;
                break;
            case 3:
            case 7:

                if(option == 2){
                    changeC(opts[option], opts[0]);
                    option = 0;
                }else{
                    changeC(opts[option], opts[option+1]);
                    option++;
                }break;

            case 4:
            case 8:

                //if(option-1 == -1){ WHAT THE HELL
                if(!option){
                    changeC(opts[option], opts[2]);
                    option = 2;
                }else{
                    changeC(opts[option], opts[option-1]);
                    option--;
                }break;

            case 5:
            case 6:
                switch(option)
                {
                    case 0:
                        game();
                        break;
                    case 1:
                        options();
                        break;
                    case 2:
                        credits(title.elements, fopen("./files/credits.txt", "r"));
                        break;
                }
                break;
            default:
                system("color 0C");
                pause();
                logic("setting", "color");
        }
        delay(33);
    }
    free(opts);
    clear(&title);
}


///Returns a "simple" menu with given number lines and their respective prompts. Initializes menu.
image initMen(size_t cols, size_t lines)
{
    image setup;

    initDisp(&setup);


    int i;
    char* header[] = {"Space Invaders!","(a bootleg copy by a struggling student)","(\xA6u\xA6)","Eduardo Rodrigues Baldini Filho"};
    char* options[] = {"Start game", "Options", "Credits"};

    char* logo[] = {"\xDB\xDB\xB0\xB0\xB0\xB0\xB0\xB0\xB0\xB0\xB0\xB0\xDB\xDB",
                    "\xDB\xDB\xB0\xB0\xB0\xB0\xB0\xB0\xDB\xDB",
                    "\xDB\xDB\xDF\xDF\xDB\xDB\xDB\xDB\xDB\xDB\xDF\xDF\xDB\xDB",
                    "\xDB\xDB\xDB\xDB\xDC\xDC\xDB\xDB\xDB\xDB\xDB\xDB\xDC\xDC\xDB\xDB\xDB\xDB",
                    "\xDB\xDB\xDF\xDF\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDB\xDF\xDF\xDB\xDB",
                    "\xDB\xDB\xB0\xB0\xDB\xDB\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDF\xDB\xDB\xB0\xB0\xDB\xDB",
                    "\xDF\xDF\xB0\xB0\xDF\xDF\xDC\xDC\xDC\xDC\xB0\xB0\xDC\xDC\xDC\xDC\xDF\xDF\xB0\xB0\xDF\xDF",
                    "\xDF\xDF\xDF\xDF\xB0\xB0\xDF\xDF\xDF\xDF"};

    for(i = 0; i < 3; i++)
        formatCpy(setup.elements[i], header+i, 1);
    formatCpy(setup.elements[23], header+3, 1);

    for(i = 0; i < 8; i++)
        centerCpy(setup.elements[i+7], logo[i]);

    formatCpy(setup.elements[21], options, 3);

    return setup;
}


///TO DO: main menu options
void options()
{
    logic("","");
}


///rolls credits on cred
void credits(char** menu, FILE* cred)
{
    int credsize = 0, i, j, k = 1;

    char** creds = (char**)malloc(sizeof(char*));
    creds[credsize] = malloc(70);
    while(fgets(creds[credsize], 70, cred) != NULL)
    {
        if(creds[credsize][strlen(creds[credsize])-1] == '\n')
            creds[credsize][strlen(creds[credsize])-1] = '\0';
        credsize++;
        creds = (char**)realloc(creds, (credsize+1)*sizeof(char*));
        creds[credsize] = (char*)malloc(70);
    }
    *(creds+credsize) = NULL;

    image credits = initMen(80, 24);
    for(i = 0; i < 22+credsize-1; i++)                                  //credits roll loop
    {
        checkFocus();
        for(j = 1; j < 23; j++)
            strcpy(credits.elements[23-j], menu[23-j]);       //resets the screen

        if(i < 22){

            for(j = 0; j < k; j++)
                centerCpy(credits.elements[22-i+j], creds[j]);          //loads first lines until screen is full with 'em
            if(k < credsize)
                k++;

        }else{

            for(j = 0; j < k-1; j++)
                centerCpy(credits.elements[j+1], creds[(j+i-21)]);      //loads remaining lines and then fades when text hits top.
            if((j+i-21) > (credsize-1))
                k--;

        }
        display(credits, 24, 80);                                       //call render function
        delay(170);                                                     //menu roll speed
        int quit;                                                       //variable that holds the logic evaluation of key press
        if((quit = logic("eval_key", NULL)) == 1 || quit == 2)          //check if user pressed ESC or X
            break;                                                      //quit loop
    }

    clear(&credits);
    for(i = 0; i < credsize; i++)
        free(creds[i]);
    free(creds);
    fclose(cred);
}
