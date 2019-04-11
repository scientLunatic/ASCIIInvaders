#define _WIN32_WINNT 0x0500
#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include <time.h>
#include "misc.h"


///classic delay function
void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}


///ASCII table
void printchars()
{
    int i, j;
    for(i = 0; i < 256; i++)
    {
        printf("%d, %c   ", i, i);
        i++;

        printf("%d, %c   ", i, i);
        i++;

        printf("%d, %c   ", i, i);
        i++;

        printf("%d, %c   \n", i, i);
    }
}


///pauses if there is no focus.
void checkFocus()
{
    while(GetConsoleWindow() != GetForegroundWindow()){}
}


///doesn't work with line and col limitations so watch out
char* centerCpy(char* target, char* str)
{
    if(strlen(target)<strlen(str))
        return NULL;
    strncpy((target+(strlen(target)/2)-(strlen(str)/2)), str, strlen(str));
    return str;
}

///formatCpy's old copy, does the same but has a different name.
char* justifyCpy(char* target, char* str[], int strnum)
{
    int total = 0, i;
    for(i = 0; i < strnum; i++)
        total += strlen(str[i]);
    if(strlen(target)<total)
        return target;

    for(i = 0; i < strnum; i++)
        strncpy(target+((i+1)*strlen(target)/(strnum+1) - strlen(str[i])/2), str[i], strlen(str[i]));

    return target;
}

char* formatCpy(char* target, char* str[], int strnum)
{
    int total = 0, i;
    for(i = 0; i < strnum; i++)
        total += strlen(str[i]);
    if(strlen(target)<total)
        return target;

    for(i = 0; i < strnum; i++)
        strncpy(target+((i+1)*strlen(target)/(strnum+1) - strlen(str[i])/2), str[i], strlen(str[i]));

    return target;
}


int changeC(char* first, char* second)
{
    if(!first || !second)
        return 0;

    *first ^= *second;
    *second ^= *first;
    *first ^= *second;

    return 1;
}

int changeS(char* first, char* second)
{
    if(!first || !second)
        return 0;

    *first ^= *second;
    *second ^= *first;
    *first ^= *second;

    return 1;
}


char** retrieveOptions(char* line, char bg, char border)
{
    char** options = NULL;

    int i = 0, j = 0, size = 0;
        while(line[i+1] != border)
        {
            if(line[i+1] != bg)
                {
                    size++;
                    options = (char**)realloc(options, size*sizeof(char*));
                    options[j] = line+i;
                    j++;
                    while(line[i+1] != bg){i++;}
                }
            i++;
        }
    return options;
}


void dump()
{
    while(kbhit()){getch();}
}


void pause()
{
    while(!kbhit()){}
    dump();
}
