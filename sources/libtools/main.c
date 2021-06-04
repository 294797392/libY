#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linked_list.h"

static int foreach_linkedlist(void *data)
{
    printf("%s\n", data);
}

static void test_linkedlist()
{
    linkedlist *list = new_linkedlist();

    char *a = "a";
    char *b = "b";
    char *c = "c";
    char *d = "d";
    linkedlist_addlast(list, a);
    linkedlist_addlast(list, b);
    linkedlist_addlast(list, c);
    linkedlist_addlast(list, d);

    linkedlist_foreach(list, foreach_linkedlist);

    linkedlist_removelast(list);
    linkedlist_removelast(list);

    linkedlist_foreach(list, foreach_linkedlist);

    linkedlist_removelast(list);
    linkedlist_removelast(list);

    linkedlist_foreach(list, foreach_linkedlist);

    printf("test over\n");
}

int main(int argc, char **argv)
{
    test_linkedlist();

    return 0;
}