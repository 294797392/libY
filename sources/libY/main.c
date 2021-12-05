#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// #include "linked_list.h"
// #include "tool_tree.h"
// #include "tool_tcpcli.h"
// #include "tool_tcpsvc.h"

// static int foreach_printf(linkedlist *list, linkeditem *item, void *userdata)
// {
// 	//printf("%s\n", item->data);
// 	return 0;
// }

// static int foreach_printf2(linkedlist *list, linkeditem *item, void *userdata)
// {
// 	return 0;
// }

// static void test_linkedlist()
// {
// 	linkedlist *list = new_linkedlist();

// 	char *a = "a";
// 	char *b = "b";
// 	char *c = "c";
// 	char *d = "d";
// 	linkedlist_addlast(list, a);
// 	linkedlist_addlast(list, b);
// 	linkedlist_addlast(list, c);
// 	linkedlist_addlast(list, d);

// 	linkedlist_foreach(list, foreach_printf, NULL);

// 	linkedlist_removelast(list);
// 	linkedlist_removelast(list);

// 	linkedlist_foreach(list, foreach_printf, NULL);

// 	linkedlist_removelast(list);
// 	linkedlist_removelast(list);

// 	linkedlist_foreach(list, foreach_printf, NULL);

// 	printf("test over\n");
// }

// static char *newstr(const char *str)
// {
// 	char *r = (char *)calloc(1, strlen(str) + 1);
// 	strncpy(r, str, strlen(str));
// 	return r;
// }

// static tool_treenode *new_treenode(tool_tree *tree, tool_treenode *parent, void *data)
// {
// 	tool_treenode *node = (tool_treenode *)calloc(1, sizeof(tool_treenode));
// 	node->data = data;
// 	node->parent = parent;
// 	parent->children[parent->num_child] = node;
// 	parent->num_child++;
// 	return node;
// }

// static void init_tree(tool_tree *tree, tool_treenode *root)
// {
// 	tool_treenode *node1 = new_treenode(tree, root, newstr("1.1"));
// 	tool_treenode *node2 = new_treenode(tree, root, newstr("1.2"));
// 	tool_treenode *node3 = new_treenode(tree, root, newstr("1.3"));

// 	tool_treenode *node11 = new_treenode(tree, node1, newstr("1.1.1"));
// 	tool_treenode *node12 = new_treenode(tree, node1, newstr("1.1.2"));
// 	tool_treenode *node13 = new_treenode(tree, node1, newstr("1.1.3"));

// 	tool_treenode *node21 = new_treenode(tree, node2, newstr("1.2.1"));
// 	tool_treenode *node22 = new_treenode(tree, node2, newstr("1.2.2"));
// 	tool_treenode *node23 = new_treenode(tree, node2, newstr("1.2.3"));

// 	tool_treenode *node31 = new_treenode(tree, node3, newstr("1.3.1"));
// 	tool_treenode *node32 = new_treenode(tree, node3, newstr("1.3.2"));
// 	tool_treenode *node33 = new_treenode(tree, node3, newstr("1.3.3"));
// }

// static int tree_foreach2_action(tool_tree *tree, tool_treenode *node, void *userdata)
// {
// 	printf("%s\n", (char*)node->data);

// 	return 1;
// }

// static int tcp_cli_callback(tcpcli *cli, TCPCLI_EVENT event, void *data, size_t dsize, void *userdata)
// {
// 	switch (event)
// 	{
// 		case TCPCLI_EVT_CONNECTING:
// 		{
// 			printf("connecting...\n");
// 			break;
// 		}

// 		case TCPCLI_EVT_CONNECTED:
// 		{
// 			printf("connected\n");
// 			break;
// 		}

// 		case TCPCLI_EVT_DISCONNECTED:
// 		{
// 			printf("disconnect\n");
// 			break;
// 		}

// 		case TCPCLI_EVT_RECVMSG:
// 		{
// 			printf("message:%s\n",(char*)data);
// 			break;
// 		}

// 		default:
// 			break;
// 	}

// 	return 0;
// }

// static tcp_client skfd = 0;

// static int tcp_svc_callback(tcpsvc *svc, tcp_client cli, TCPSVC_EVENT event, void *data, size_t size, void *userdata)
// {
// 	switch (event)
// 	{
// 		case TCPSVC_EVT_CLI_CONNECTED:
// 		{
// 			skfd = cli;
// 			printf("TCPSVC_EVT_CLI_CONNECTED\n");
// 			break;
// 		}

// 		case TCPSVC_EVT_CLI_DISCONNECTED:
// 		{
// 			printf("TCPSVC_EVT_CLI_DISCONNECTED\n");
// 			break;
// 		}

// 		case TCPSVC_EVT_CLI_MESSAGE:
// 		{
// 			printf("TCPSVC_EVT_CLI_MESSAGE, %s\n", (char*)data);
// 			tcpsvc_sendto(svc, cli, "hello c#\n", strlen("hello c#\n"));
// 			break;
// 		}

// 		case TCPSVC_EVT_CLI_FULL:
// 		{
// 			printf("TCPSVC_EVT_CLI_FULL\n");
// 			break;
// 		}

// 		case TCPSVC_EVT_ERROR:
// 		{
// 			printf("TCPSVC_EVT_ERROR\n");
// 			break;
// 		}

// 		default:
// 			break;
// 	}

// 	return 1;
// }

// int main(int argc, char **argv)
// {
// 	//char *rootdata = "root";
// 	//tool_tree *tree = new_tree();
// 	//tool_treenode *root = tree_initroot(tree, rootdata);
// 	//init_tree(tree, root);

// 	//tree_foreach2(tree, tree_foreach2_action, NULL);

// 	//tcpcli *cli = new_tcpcli("127.0.0.1", 8888);
// 	//tcpcli_setopt(cli, TCPOPT_LINE_BASED, NULL, 0);
// 	//tcpcli_set_event_callback(cli, tcp_cli_callback, NULL);
// 	//tcpcli_start(cli);

// 	int maxcli = 5;
// 	tcpsvc *svc = new_tcpsvc(NULL, 8888);
// 	tcpsvc_setopt(svc, TCPOPT_LINE_BASED, NULL, 0);
// 	tcpsvc_setopt(svc, TCPOPT_MAX_CLI, (char*)&maxcli, sizeof(int));
// 	tcpsvc_set_event_callback(svc, tcp_svc_callback, NULL);
// 	tcpsvc_start(svc);

// 	char c;
// 	scanf(&c);

// 	while (1)
// 	{
// 		tcpsvc_sendto(svc, skfd, "hello\n", 6);

// 		os_sleep(1000);
// 	}

// 	os_sleep(999999);

// 	return 0;
// }


int main(int argc, char **argv)
{
	return 0;
}