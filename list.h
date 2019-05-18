#ifndef LIST_H_INCLUDED
#define LIST_H_INCLUDED

  #include <stdlib.h>

  #define newListNode (struct LIST_NODE*)calloc(1, sizeof(LIST_NODE))
  #define newList (struct LIST*)calloc(1, sizeof(LIST))

  #define is_null(pointer) (!pointer)

  #define is_list_empty(list) is_null(list) || is_null(list->head)

  #ifndef wrap_around
    #define wrap_around(var, range) (var + range) % range
  #endif // wrap_around

    struct LIST_NODE{
      void* el;
      struct LIST_NODE* next[2];//!<next[0] = right<br>   next[1] = left, previous
    };

    typedef struct LIST_NODE LIST_NODE;

    struct LIST{
      LIST_NODE* head;
      int length;
    };

    typedef struct LIST LIST;

    LIST* new_list();
      LIST_NODE* lnofetch(LIST* l, int i);
      int ladd(LIST* l, void* item, int index);
      int lrem(LIST* l, int index);

      int lnoset(LIST_NODE* n, void* el, LIST_NODE* next, LIST_NODE* prev);
      int ledit(LIST* l, int index, void* item);
      int lfind(LIST* l, void* item, int* index);

      void* lget(LIST* l, int index);
      void* ltake(LIST* l, int index);


      int lclear(LIST* l);

    #define lpush(List, item) ladd(List, item, 0)
    #define lpop(List) ltake(List, 0)
    #define lenqueue(List, item) ladd(List, item, -1)
    #define ldequeue(List) ltake(List, 0)
    #define ldetach(List) ltake(List, -1)

  #define for_each_list(item, list, iterator) \
    for(iterator = 0, item = list->head; iterator < list->length; iterator++, item = item->next[0])


#endif // LIST_H_INCLUDED
