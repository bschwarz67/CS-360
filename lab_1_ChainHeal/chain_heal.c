#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

typedef struct node
{
	char *name;
	int x, y;
	int cur_PP, max_PP;
	struct node *prev;
} Node;


int main()
{
	int x, y, cur_PP, max_PP, node_number = 1, node_size = sizeof(Node); 
	Node *new_node = NULL, *temp_node = NULL;
	char name[100], *name_cpy;
	while (scanf("%d", &x) == 1 && scanf("%d", &y) == 1 && scanf("%d", &cur_PP) == 1 && scanf("%d", &max_PP) == 1 && scanf("%s", &name) == 1) {
		temp_node = new_node;
		new_node = (Node *) malloc(node_size);	
		new_node->x = x;
		new_node->y = y;
		new_node->cur_PP = cur_PP;
		new_node->max_PP = max_PP;
		name_cpy = (char *) malloc(sizeof(name) / sizeof(name[0]));
		new_node->name = strcpy(name_cpy, name);
		new_node->prev = temp_node;
		if(node_number > 1) printf("%s\n", new_node->prev->name);
		node_number++;
		printf("%d %d %d %d %s\n", new_node->x, new_node->y, new_node->cur_PP, new_node->max_PP, new_node->name);
	}
	exit(0);
}




