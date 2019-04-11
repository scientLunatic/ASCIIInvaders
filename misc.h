#ifndef MISC_H_INCLUDED
#define MISC_H_INCLUDED

    void delay(unsigned int mseconds);
    void printchars();
    void checkFocus();
    char* centerCpy(char* target, char* str);
    char* justifyCpy(char* target, char** str, int strnum);
    char* formatCpy(char* target, char* str[], int strnum);
    char** retrieveOptions(char* line, char bg, char border);
    int changeC(char* first, char* second);
    void pause();
    void dump();

#endif // MISC_H_INCLUDED
