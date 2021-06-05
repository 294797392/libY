#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "linked_list.h"
#include "readdata.h"

static int foreach_printf(linkedlist *list, linkeditem *item, void *userdata)
{
	printf("%s\n", item->data);
	return 0;
}

static int foreach_printf2(linkedlist *list, linkeditem *item, void *userdata)
{
	sorted_data *sd = (sorted_data *)item->data;
	if (sd->type == DT_DOUBLE)
	{
		for (size_t i = 0; i < sd->values_double_len; i++)
		{
			printf("%f\n", sd->values_double[i]);
		}
	}
	else if (sd->type == DT_FLOAT)
	{
		for (size_t i = 0; i < sd->values_float_len; i++)
		{
			printf("%f\n", sd->values_float[i]);
		}
	}
	else if (sd->type == DT_INT)
	{
		for (size_t i = 0; i < sd->values_int_len; i++)
		{
			printf("%d\n", sd->values_int[i]);
		}
	}

	printf("over\n");
	printf("\n\n\n");

	return 0;
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

	linkedlist_foreach(list, foreach_printf, NULL);

	linkedlist_removelast(list);
	linkedlist_removelast(list);

	linkedlist_foreach(list, foreach_printf, NULL);

	linkedlist_removelast(list);
	linkedlist_removelast(list);

	linkedlist_foreach(list, foreach_printf, NULL);

	printf("test over\n");
}

int main(int argc, char **argv)
{
	//test_linkedlist();
	read_data("D:\\code\\oheiheiheiheihei\\tools\\msvc\\libtools\\x64\\Debug\\data.txt");

	linkedlist *list = get_linkedlist();

	//linkedlist_foreach(list, foreach_printf2, NULL);

	save_binary_file();

	//linkedlist_foreach(list, foreach_printf2, NULL);

	read_binary_file();

	linkedlist_foreach(list, foreach_printf2, NULL);

	return 0;
}