#include "list.h"


LIST* new_list()
{
    LIST* newl = newList;
    return newl;
}

LIST_NODE* lnofetch(LIST* l, int i)
{
    if(is_list_empty(l)) return NULL;

    LIST_NODE* seeker = l->head;

    for(i %= l->length; i != 0; i -= (abs(i)/i))
      seeker = seeker->next[(i < 0)];
    return seeker;
}

int lnoset(LIST_NODE* n, void* el, LIST_NODE* next, LIST_NODE* prev)
{
    return (n)?
                n->el      = el,
                n->next[0] = next,
                n->next[1] = prev,
                1
              :
                0;
}

int ladd(LIST* l, void* item, int index)
{
    if(is_null(l)) return 0;
    LIST_NODE* n = newListNode, *seeker = lnofetch(l, index+(index < 0));
    if(is_null(n)) return 0;

    if(is_null(seeker)){
        seeker     = n;
        n->next[1] = n;
        l->head    = n;
    }else if(index == 0)
        l->head    = n;

    lnoset(n, item, seeker, seeker->next[1]);

    seeker->next[1]->next[0] = n;
    seeker->next[1]          = n;
    return ++l->length;
}

int lrem(LIST* l, int index)
{
    if(is_list_empty(l)) return 0;

    if(l->length == 1){
        free(l->head);
        l->head = NULL;
        return (l->length = 0);
    }

    LIST_NODE* seeker = lnofetch(l, index);

    if(seeker == l->head)
      l->head = l->head->next[0];

    seeker->next[0]->next[1] = seeker->next[1];
    seeker->next[1]->next[0] = seeker->next[0];
    free(seeker);

    return --l->length;
}

void* lget(LIST* l, int index)
{
    LIST_NODE* seeker = lnofetch(l, index);
    return (seeker)? seeker->el: NULL;
}

int lfind(LIST* l, void* item, int* index)
{
    if(is_null(l->head)) return 0;
    int dump;
    if(is_null(index)) index = &dump;

    LIST_NODE  *indexWise = l->head, *ctrIdWise = l->head;

    if( indexWise->el == item ){*index =  0;  return 1;}

    int i;
    for(i = 1; i <= ((l->length)/2); i++){
      indexWise = indexWise->next[0];
      ctrIdWise = ctrIdWise->next[1];
      if(indexWise->el == item){*index =  i;  return 1;}
      if(ctrIdWise->el == item){*index = -i;  return 1;}
    }

    return 0;
}

int lclear(LIST* l)
{
    if(l->length == 0) return 0;
    while(l->length > 0) lrem(l, 0);
    return 1;
}

void* ltake(LIST* l, int index)
{
    void* tempElPtr = lget(l, index);
    lrem(l, index);
    return tempElPtr;
}

int ledit(LIST* l, int index, void* item)
{
    LIST_NODE* temp = lnofetch(l, index);

    if(!temp) return 0;

    temp->el = item;
    return 1;
}
