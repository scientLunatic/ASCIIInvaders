#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <stdarg.h>
#include <math.h>
#include <time.h>
#include <locale.h>
#include "list.h"

#define PROMPT_WIDTH 80
#define PROMPT_HEIGHT 24
#define PROMPT_SIZE 1920
#define printflush(...) printf(__VA_ARGS__); fflush(stdout)
#define WAITFORINPUT 1
#define NOINPUTBLOCK 0
#define SUCCESS 1
#define FAILURE 0
#define CAUTION -1
//difficulty
#define HARD {settings.color, settings.delay, 0, 1, 60}
#define NORM {settings.color, settings.delay, 4, 2, 40}
#define EASY {settings.color, settings.delay, 8, 4, 10}
//speed
#define INST {settings.color, 0, settings.enemy_wait, settings.shot_velocity, settings.enemy_number}
#define FAST {settings.color, 10, settings.enemy_wait, settings.shot_velocity, settings.enemy_number}
#define MEDI {settings.color, 33, settings.enemy_wait, settings.shot_velocity, settings.enemy_number}
#define SLOW {settings.color, 100, settings.enemy_wait, settings.shot_velocity, settings.enemy_number}

typedef char* String;
typedef String Phrase[];
typedef String* wordSoup;
typedef void* ViP;
typedef void* voidParr[];
typedef void** voidPP;
typedef void (*funPtr)(void*);

typedef struct {
    char color[3];//{<0...f>, <0...f>, null}
    int delay;//delay at game
    int enemy_wait;//enemy wait cycles
    int shot_velocity;
    int enemy_number;
    //control configuration
}SETTINGS;

SETTINGS settings = {"0a", 10, 8, 1, 40};

typedef struct {
    int width;
    int height;
}DIMENSIONS;

typedef struct {
    int x;
    int y;
}COORD;

typedef struct IMAGE{
    String data;
    union{
        struct{
            int width;
            int height;
        };
        DIMENSIONS d;
    };
}IMAGE;

typedef struct SPRITE{
    IMAGE i;
    union{
        struct{
            int x;
            int y;
        };
        COORD pos;
    };
}SPRITE;

IMAGE screen;

enum cfg_code
{
    LOAD,
    UPDATE
};

enum key_alias
{
    ENTER   = 13,
    ESC     = 27,
    SPACE   = 32,
    HOME    = 57415,
    UP             ,
    PGUP           ,
    LEFT    = 57419,
    RIGHT   = 57421,
    DOWN    = 57424,
    END_K   = 57423,
    PGDOWN  = 57425,
    INS            ,
    DEL
};

enum program_states
{
    NONE,
    ERRORFLG,
    LOADMENU,
    LOADGAME,
    GAMEOVER,
    MATCHWON,
    LOADOPTI,
    SHOWCRED,
    FINISHFL,
    DEBUGFLG,
    WILDCARD,
    CONTINUE
};

//!--------------------
//misc.
int min(int a, int b);
int max(int a, int b);
void dump(void);
int gen();//!< accessory function, variable contents, test and temporary results.
void delay(unsigned int mseconds);
SPRITE* spawn(String req);//!< create new sprite
int despawn(SPRITE* sp);
SPRITE* findByCoord(LIST* sprList, int x, int y, int* index);

//!--------------------
//char matrix images;
IMAGE weavPat;      //weave pattern
IMAGE noPat;        //"blank" bg
IMAGE border;       //
IMAGE inv;          //invaders icon
IMAGE cool;         //weird ring thing
IMAGE titleScreen;  //take a guess
IMAGE cred;         //biscuit recipe.
IMAGE optScreen;    //after render option screen
//screen is above, if you can't find it.

//!----------------------
//image manipulation
int loadImg(FILE* f, IMAGE* tgt, int w, int h);
int copyImg(IMAGE* cpy, IMAGE* src);
int overlay(IMAGE* out, IMAGE* top);//!< 'out', with the same dimensions is rewritten with 'top' on top of it. Ignores spaces.
int olayOffset(IMAGE* out, IMAGE* top, int hoff, int voff);
int olayCenter(IMAGE* out, IMAGE* top, int voff);

//!--------------------
//drawing functions
void weave();
void invader();
void display(funPtr fun, void* arg);
void dispSiz(IMAGE* img);

//!--------------------
//setup functions

void loadMenu(void);
void loadCred(void);
void loadGame(void);
void loadEndLose(void);
void loadEndWin(void);
void loadOpti(void);


//!--------------------
//contexts
int control(int lockF);//!< merely in-line reading is OK but this allows me to return arbitrary ints to any reading strategy
void updateOptions(int op);
int options();
int menu();//!< menu with options to start the game, see the credits and configure(ideally color, controls, speed, difficulty(?))
int game();
int end();
int credits();
int init();
int finish();
int main_loop();//!< loop to control states on the program, the whole flow.

//!--------------------
//meta-programming
String whatFunc(void* fptr);//!< ask main_loop which context is running.

//!--------------------
//program meta data
char buffer[PROMPT_SIZE] = {0};
voidParr __gfa = {game, menu, finish};      //!<references
Phrase   __gfn = {"game", "menu", "finish"};//!<names
int      __gfc = 3;                         //!<count

String whatFunc(void* fptr)         //!<self-awareness is awesome
{
    voidPP funcs;
    wordSoup fnames;
    funcs = __gfa; fnames = __gfn;
    int i, siz = __gfc;
    for(i = 0; i < siz; i++){
        if(funcs[i] == fptr)
            return fnames[i];
    }
    return "!NO MATCH!";
}

int min(int a, int b){
    return(a < b)? a: b;
}

int max(int a, int b){
    return(a > b)? a: b;
}

void dump(void)
{
    while(kbhit()) getch();
}

int gen()
{
    //FILE* f = fopen("files/cool.txt", "rb");
    //String inv = calloc(55, sizeof(char));
    //while(fgets(inv, 54, f))
    //  printf("%s\n", inv);
    //fclose(f);
    //putc(255, stdout);
    display((funPtr)dispSiz, (ViP)&weavPat);
    return FINISHFL;
}

void delay(unsigned int mseconds)
{
    clock_t goal = mseconds + clock();
    while (goal > clock());
}

SPRITE* spawn(String req)
{//AKA playing scribblenauts with the engine.
    char opt = ((!strcmp(req, "player"))<<2) + ((!strcmp(req, "enemy"))<<1) + (!strcmp(req, "shot"));
    if(opt){
        SPRITE* sp = malloc(sizeof(SPRITE));
        sp->pos = (COORD){0, 0};
        switch(opt)
        {
            case 0b1:   sp->i.data = calloc(2, sizeof(char));
                        strcpy(sp->i.data, "^");
                        sp->i.d = (DIMENSIONS){1, 1};
                        break;
            case 0b10:  sp->i.data = calloc(3, sizeof(char));
                        sp->i.d = (DIMENSIONS){3, 1};
                        strcpy(sp->i.data, "\xc9\xcb\xbb");//Revert to old sprite once at home, load from file(maybe that'll do the trick).
                        break;                             //The other part would be to properly use the right code page(which seems to be a problem)
            case 0b100: sp->i.data = calloc(4, sizeof(char));
                        strcpy(sp->i.data, "\xda\xc1\xbf");
                        sp->i.d = (DIMENSIONS){3, 1};
                        break;
        }
        return sp;
    }
    return NULL;
}

int despawn(SPRITE* sp)
{
    if(sp){
        free(sp->i.data);
        free(sp);
        return SUCCESS;
    }return CAUTION;
}

SPRITE* findByCoord(LIST* sprList, int x, int y, int* index)
{
    int i;
    LIST_NODE* it;
    SPRITE* hold;
    for_each_list(it, sprList, i){
        hold = it->el;
        if((y >= hold->y) && (y < (hold->y + hold->i.height)))
            if((x >= hold->x) && (x < (hold->x + hold->i.width))){
                *index = i;
                return hold;
            }
    }
    return NULL;
}

int loadImg(FILE* f, IMAGE* tgt, int w, int h)
{
    *tgt = (IMAGE){calloc(w*h + 1, sizeof(char)), .d = (DIMENSIONS){w, h}};
    if(!f) return -1;

    fgets(tgt->data, w*h + 1, f);
    fclose(f);
    return 0;
}

int copyImg(IMAGE* cpy, IMAGE* src)
{
    int siz = src->width*src->height;
    free(cpy->data); cpy->data = NULL;
    *cpy = (IMAGE){memcpy(malloc((siz + 1) * sizeof(char)), src->data, siz), .d = src->d};
    return 0;
}

int overlay(IMAGE* out, IMAGE* top)
{
    int i, j,
        w = min(out->width, top->width),    //|trim
        h = min(out->height, top->height),  //|
        c, outpos, toppos;

    for(i = 0; i < h; i++)
        for(j = 0, outpos = (i * out->width) + j, toppos = (i * top->width) + j; j < w; j++, outpos++, toppos++)
            out->data[outpos] = (((c = top->data[toppos]) == ' ') ? out->data[outpos]: c);

    return 0;
}

int olayOffset(IMAGE* out, IMAGE* top, int hoff, int voff)
{
    int i, j,
    //!there we go again
    #define THINGAMAJIG(dim, doff) min(out->dim,  (top->dim  + min(doff, 0) + out->dim  - min(max(out->dim,  doff + top->dim ), (top->dim  + out->dim ))))
    w = THINGAMAJIG(width, hoff),
    h = THINGAMAJIG(height, voff),
    #undef THINGAMAJIG
    c, outpos, toppos;
    /*intersection is:                 limited by size of out and top
    //                                    min(out->dim, <top->dim>)
    //                                   reduced by negative values
    //                                     top->dim + min(doff, 0)
    //                                              and
    //                      reduced by values that push against the far corners
    //        top->dim + out->dim - min(max(out->dim, doff + top->dim), (top->dim + out->dim))
    //   (if((doff + top->dim) > out->dim), how much it is greater is how much top->dim is smaller)
    */
    for(i = 0; i < h; i++)
        for(j = 0, outpos = ((i + max(0, voff)) * out->width) + j + max(0, hoff),
                   toppos = ((i - min(0, voff)) * top->width) + j - min(0, hoff); j < w; j++, outpos++, toppos++)
            out->data[outpos] = (((c = top->data[toppos]) == ' ') ? out->data[outpos]: c);

    return 0;
}

int olayCenter(IMAGE* out, IMAGE* top, int voff)
{
    int hoff = out->width/2 - top->width/2;
    olayOffset(out, top, hoff, voff);
    return SUCCESS;
}

void weave()
{
    if(strlen(weavPat.data) == 0) exit(0);
    printf("%s", weavPat.data);
}

void invader()
{
    if(strlen(inv.data) == 0) exit(0);
    int i;
    for(i = 0; i < inv.height; i++, putc('\n', stdout))
        fprintf(stdout, "%.*s", inv.width, inv.data + i*inv.width);
}

void display(funPtr fun, void* arg)
{
    system("cls");
    fun(arg);
    fflush(stdout);
}

void dispSiz(IMAGE* img)
{
    if(strlen(img->data) == 0) exit(EXIT_FAILURE);
    int i, h = min(img->height, PROMPT_HEIGHT),
           w = min(img->width, PROMPT_WIDTH),
        crop = (w < PROMPT_WIDTH);
    for(i = 0; i < h; i++, (void)(crop && putc('\n', stdout)))
        fprintf(stdout, "%.*s", w, &img->data[i * img->width]);
}

void loadGame()
{
    copyImg(&screen, &noPat);//!changing bg means changing hit detection.
}

void loadEndLose()
{
    IMAGE gmovr = {"\
\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\
\xffGAME\xffOVER\xff\
\xffYOU\xff\xffLOST\xff\
\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", .d = {11, 4}};
    olayCenter(&screen, &gmovr, screen.height/2);
}

void loadEndWin()
{
    copyImg(&screen, &noPat);
    IMAGE gmwin = {"YOU\xffWON!", .d = {8, 1}};
    olayCenter(&screen, &gmwin, screen.height/2);
}

void loadMenu()
{
    copyImg(&screen, &titleScreen);
}

void loadOpti()
{
    copyImg(&screen, &optScreen);
}

void showCred()
{
    copyImg(&screen, &titleScreen);//could be another bg
}

int control(int lockF)
{
    int k;
    if(lockF || kbhit())
        return ((k = getch()) != 224)? k : (k<<8) + getch();
    else return 0;
}

void updateOptions(int op)
{
    switch(op){
        case LOAD:
            {
                char cmd[9];
                sprintf(cmd, "color %s", settings.color);
                system(cmd);
            }
            break;
        case UPDATE:
            break;
    }
}
int options()
{
    Phrase topics = {"Color", "Difficulty", "Speed"};
    Phrase diffs = {"Easy", "Normal", "Hard", "Custom"};
    Phrase speeds = {"Custom", "None", "Fast", "Medium", "Slow"};
    LIST* colors = new_list();
    int i, tsizs[] = {5, 10, 5},      tops[3][2], selected = 0,
           dsizs[] = {4, 6, 4, 6},    difs[4][2],
           ssizs[] = {6, 4, 4, 6, 4}, spee[5][2],
           csiz = 0;//handle colors separately since they're dynamic

    for(i = 0; i < 3; i++)
    {
        IMAGE temp = (IMAGE){topics[i], .d = {tsizs[i], 1}};
        olayOffset(&optScreen, &temp, tops[i][0] = ((i*4) + 1) * optScreen.width/10 - temp.width/2, tops[i][1] = 3);
    }

    {
        IMAGE back = (IMAGE){"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", .d = {8, 4}};
        olayOffset(&optScreen, &back, 5*optScreen.width/10 - back.width/2, 6);
        for(i = 0; i < 4; i++)
        {
            IMAGE temp = (IMAGE){diffs[i], .d = {dsizs[i], 1}};
            olayOffset(&optScreen, &temp, difs[i][0] = 5*optScreen.width/10 - temp.width/2, difs[i][1] = 6+i);
        }
    }

    {
        IMAGE back = (IMAGE){"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", .d = {8, 5}};
        olayOffset(&optScreen, &back, 9*optScreen.width/10 - back.width/2, 8);
        for(i = 0; i < 5; i++)
        {
            IMAGE temp = (IMAGE){speeds[i], .d = {ssizs[i], 1}};
            olayOffset(&optScreen, &temp, spee[i][0] = 9*optScreen.width/10 - temp.width/2, spee[i][1] = 8+i);
        }
    }

    {
        IMAGE back = (IMAGE){"\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff\xff", .d = {8, 10}};
        olayOffset(&optScreen, &back, optScreen.width/10 - back.width/2, 8);
    }

    while(1){
        copyImg(&screen, &optScreen);
        screen.data[tops[selected][1] * screen.width + tops[selected][0] - 1] = '>';
        screen.data[tops[selected][1] * screen.width + tops[selected][0] + tsizs[selected]] = '<';
        display((funPtr)dispSiz, (ViP)&screen);
        int k;
        switch(k = control(WAITFORINPUT))
        {
            case LEFT  : selected = (selected + 2) % 3; break;
            case RIGHT : selected = (selected + 1) % 3; break;
            case ESC   : return LOADMENU;
            case ENTER :
                switch(selected)
                {//stack menu by hierarchy
                    //case 0: return colopt();
                    //case 1: selected = difs[];
                    //case 2: return speopt();
                }
        }
    }
}

int credits()
{
    int i;
    for(i = screen.height; i > -(cred.height); i--){
        copyImg(&screen, &titleScreen);
        olayCenter(&screen, &cred, i);
        display((funPtr)dispSiz, (ViP)&screen);
        switch(control(NOINPUTBLOCK))
        {
            case ESC:
            case ENTER:
                return LOADMENU;
        }
        delay(88);
    }return LOADMENU;
}

int menu()
{
    int opts[3][2];
    static int selected = 0;
    {//It's here because I need it to be close to the input handling to change it quickly
        int i;
        Phrase titles = {"Start", "Options", "Credits"};
        int sizes[] = {5, 7, 7};
        for(i = 0; i < 3; i++)
        {
            IMAGE temp = (IMAGE){titles[i], .d = {sizes[i], 1}};
            olayOffset(&titleScreen, &temp, opts[i][0] = (i + 1)*titleScreen.width/4 - temp.width/2, opts[i][1] = titleScreen.height - 3);
        }
    }
    while(1){
        copyImg(&screen, &titleScreen);
        screen.data[opts[selected][1] * screen.width + opts[selected][0] - 1] = '\xb2';
        display((funPtr)dispSiz, (ViP)&screen);
        int k;
        switch(k = control(WAITFORINPUT))
        {
            case LEFT  : selected = (selected + 2) % 3; break;
            case RIGHT : selected = (selected + 1) % 3; break;
            case ESC   : return FINISHFL;
            case ENTER :
                switch(selected)
                {
                    case 0: return LOADGAME;
                    case 1: return LOADOPTI;
                    case 2: return SHOWCRED;
                }
        }
    }
}

int game()
{//add scores
    int i, j, running = 1, retval = LOADMENU,
        shotVel = settings.shot_velocity,//Shot velocity
        nmyWait = settings.enemy_wait,//Turns enemies wait
        nmyTurn = 0;
    LIST *foes = new_list(), *shots = new_list();
    for(i = 0; i < settings.enemy_number; i++){
        SPRITE* enemy = spawn("enemy");
        enemy->pos = (COORD){(i%10 + 1) * screen.width/11 - enemy->i.width/2, 3*(i/10) + 4};
        lpush(foes, enemy);
    }
    SPRITE* player = spawn("player");
    player->pos = (COORD){screen.width/2, screen.height - 1};
    while(running)
    {
        copyImg(&screen, &noPat);
        LIST_NODE* item;
        SPRITE* hold;

        for_each_list(item, foes, i){
            hold = (SPRITE*)item->el;
            if(nmyTurn == nmyWait - 1)
                hold->y = (hold->y % 2)? ((hold->x == 0)? ++hold->y: hold->x--, hold->y):
                                         ((hold->x == screen.width - hold->i.width)? ++hold->y: hold->x++, hold->y);
            if(hold->y == screen.height - player->i.height) {running = 0; break;}
            olayOffset(&screen, &(hold->i), hold->x, hold->y);
        }if(running == 0){retval = GAMEOVER; break;}
         nmyTurn = (nmyTurn + 1) % nmyWait;

        for_each_list(item, shots, i){
            hold = (SPRITE*)item->el;
            olayOffset(&screen, &(hold->i), hold->x, hold->y);
            //check hit & move shot
            for(j = (hold->y - 1); (j >= 0) && (j >= hold->y - shotVel); j--)
                if(screen.data[j * screen.width + hold->x] != '\xb0')
                {
                    int t, e;
                    SPRITE* Nmy;
                    Nmy = findByCoord(foes, hold->x, j, &e);
                    if(Nmy) lrem(foes, e);
                    despawn(Nmy);

                    lfind(shots, hold, &t);
                    lrem(shots, t);
                    despawn(hold);
                }
                    //printflush("hit");
            hold->y = hold->y - shotVel;
            if(hold->y < 0) {
                int t;
                lfind(shots, hold, &t);
                lrem(shots, t);
                despawn(hold);
            }
        }if(foes->length == 0){retval = MATCHWON; break;}

        olayOffset(&screen, &player->i, player->x, player->y);
        display((funPtr)dispSiz, (ViP)&screen);
        switch(control(NOINPUTBLOCK))
        {
            case LEFT  : player->x = ((player->x) > 0)?
                                       player->x - 1: player->x;
                        break;
            case RIGHT : player->x = ((player->x) < screen.width - player->i.width)?
                                       player->x + 1: player->x;
                        break;
            case ENTER : hold = spawn("shot");
                         hold->pos = (COORD){player->pos.x + player->i.width/2, player->pos.y - 1};
                         lpush(shots, hold);
                        break;
            case HOME:  return DEBUGFLG;
            case ESC:   running = 0;
        }delay(settings.delay);
    }
    while(foes->length) despawn((SPRITE*)lpop(foes));
    while(shots->length) despawn((SPRITE*)lpop(shots));
    free(foes); free(shots);
    despawn(player);
    return retval;
}

int end()
{
    display((funPtr)dispSiz, (ViP)&screen);
    delay(20);
    dump();
    control(WAITFORINPUT);
    return LOADMENU;
}

int init()
{//load files into buffers and set the console buffer to "big enough"
    loadImg(fopen("files\\weaveBG.txt", "rb"), &weavPat, PROMPT_WIDTH, PROMPT_HEIGHT);
    loadImg(fopen("files\\invader.txt", "rb"), &inv, 22, 8);
    loadImg(fopen("files\\cool.txt", "rb"), &cool, 53, 34);
    loadImg(fopen("files\\background.txt", "rb"), &noPat, PROMPT_WIDTH, PROMPT_HEIGHT);
    loadImg(fopen("files\\border.txt", "rb"), &border, PROMPT_WIDTH, PROMPT_HEIGHT);
    loadImg(fopen("files\\titleScreen.txt", "rb"), &titleScreen, PROMPT_WIDTH, PROMPT_HEIGHT);
    loadImg(fopen("files\\credits.txt", "rb"), &cred, 42,37);
    copyImg(&optScreen, &weavPat);
    {IMAGE opt = {"Options", .d = {7, 1}};
    olayCenter(&optScreen, &opt, 0);}
    updateOptions(LOAD);

    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
    return LOADMENU;
}
//keep init&finish together to counteract global var. trouble
int finish()
{//free global data and end normally
    free(weavPat.data);
    free(inv.data);
    free(cool.data);
    free(noPat.data);
    free(border.data);
    free(titleScreen.data);
    free(optScreen.data);
    free(cred.data);
    free(screen.data);
    return EXIT_SUCCESS;
}

int main_loop()//rename to control loop?
{
    int (*context)(void) = init;
    int flag = NONE;

    while((flag = context()))
        switch(flag)
        {
            case FINISHFL: return finish();
            case LOADGAME: loadGame(); context = game; break;
            case GAMEOVER: loadEndLose(); context = end; break;
            case MATCHWON: loadEndWin(); context = end; break;
            case LOADMENU: loadMenu(); context = menu; break;
            case DEBUGFLG: printflush("Returning from %s\n", whatFunc(context)); break;
            case SHOWCRED: showCred(); context = credits; break;
            case LOADOPTI: loadOpti(); context = options; break;
            case WILDCARD: context = gen; break;
            case CONTINUE: break;
            default: printflush("CODING ERROR, UNEXPECTED STATE\n");
            case ERRORFLG: return EXIT_FAILURE;
        }
    printflush("CODING ERROR(?), FLAG IS NONE\nASSUMING YOU WANTED TO QUIT.\n");
    return EXIT_SUCCESS;
}

int main()
{
    return main_loop();
}

/*static void weave_old()
{
    //checker background optimization, partly unrolled.
    static const String weavPat = {"\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\
\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2\xb0\xb1\xb2"};
    printf("%s%s", weavPat, weavPat);
    printf("%s%s", weavPat, weavPat);
    printf("%s%s", weavPat, weavPat);
    printf("%s%s", weavPat, weavPat);
    printf("%s%s", weavPat, weavPat);

    fflush(stdout);
}*/

/*void loadMenu_old()
{
    copyImg(&screen, &noPat);
    overlay(&screen, &border);
    olayCenter(&screen, &inv, 7);

    int i;
    {
        Phrase titles = {"Space\xffinvaders!", "(a\377bootleg\377copy\377by\377a\377struggling\377student)",
                         "(\xa6u\xa6)", "Eduardo\xffRodrigues\377Baldini\377Filho"};
        int sizes[] = {15, 40, 5, 31};
        for(i = 0; i < 4; i++){
            {//extra brackets for scope delimitation of temp
                IMAGE temp = (IMAGE){titles[i], .d = {sizes[i], 1}};
                olayCenter(&screen, &temp, i);
            }
        }
    }
    {
        Phrase titles = {"Start", "Options", "Credits"};
        int sizes[] = {5, 7, 7};
        for(i = 0; i < 3; i++){
            {
                IMAGE temp = (IMAGE){titles[i], .d = {sizes[i], 1}};
                olayOffset(&screen, &temp, (i+1)*screen.width/4 - temp.width/2, screen.height - 3);
            }
        }
    }
}*/
