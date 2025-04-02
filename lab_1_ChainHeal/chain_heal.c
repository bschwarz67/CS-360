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
	int adj_size;
	struct node **adj;

} Node;


int main(int argc, char **argv)
{
	int x, y, z, cur_PP, max_PP, node_number = 0, node_size = sizeof(Node), x_distance, y_distance, initial_range, jump_range, num_jumps, initial_power;
	double distance, power_reduction; 
	Node *new_node = NULL, *temp_node = NULL;
	Node **player_list;
	char name[100], *name_cpy;

	initial_range = atoi(argv[1]);
	jump_range = atoi(argv[2]);
	num_jumps = atoi(argv[3]);
	initial_power = atoi(argv[4]);
	power_reduction = atol(argv[5]);

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
		new_node->adj_size = 0;
		node_number++;
		printf("%d %d %d %d %s\n", new_node->x, new_node->y, new_node->cur_PP, new_node->max_PP, new_node->name);
	}
	printf("node number is %d\n", node_number);
	player_list = (Node **) malloc(sizeof(Node *) * node_number);
	x = 0;
	while(new_node != NULL) {
		player_list[x] = new_node;
		new_node = new_node->prev;
		x++;
	}

	for(x = 0; x < node_number; x++) printf("%s\n", player_list[x]->name);
	for(x = 0; x < node_number; x++) {
		if(strcmp(player_list[x]->name, "Urgosa_the_Healing_Shaman") == 0) {
			for(y = 0; y < node_number; y++) {
				x_distance = player_list[x]->x - player_list[y]->x;
				y_distance = player_list[x]->y - player_list[y]->y;
				distance = sqrt(x_distance*x_distance + y_distance*y_distance);
				if(distance <= initial_range) player_list[x]->adj_size++;
			}
			printf("%d\n", player_list[x]->adj_size);
			player_list[x]->adj = (Node **) malloc(sizeof(Node *) * player_list[x]->adj_size);
			z = 0;
			for(y = 0; y < node_number; y++) {
				x_distance = player_list[x]->x - player_list[y]->x;
				y_distance = player_list[x]->y - player_list[y]->y;
				distance = sqrt(x_distance*x_distance + y_distance*y_distance);
				if(distance <= initial_range) {
					player_list[x]->adj[z] = player_list[y];
					z++;
				}
			}
		}
		else {
			for(y = 0; y < node_number; y++) {
				if(x != y) {
					x_distance = player_list[x]->x - player_list[y]->x;
					y_distance = player_list[x]->y - player_list[y]->y;
					distance = sqrt(x_distance*x_distance + y_distance*y_distance);
					if(distance <= jump_range) player_list[x]->adj_size++;
				}
			}
			printf("%d\n", player_list[x]->adj_size);
			player_list[x]->adj = (Node **) malloc(sizeof(Node *) * player_list[x]->adj_size);
			z = 0;
			for(y = 0; y < node_number; y++) {
				if(x != y) {
					x_distance = player_list[x]->x - player_list[y]->x;
					y_distance = player_list[x]->y - player_list[y]->y;
					distance = sqrt(x_distance*x_distance + y_distance*y_distance);
					if(distance <= jump_range) {
						player_list[x]->adj[z] = player_list[y];
						z++;
					}
				}
			}
		}
		
	}
	for(x = 0; x < node_number; x++) {
		printf("adj list %s\n\n", player_list[x]->name);
		for(y = 0; y < player_list[x]->adj_size; y++) {
			printf("%s\n", player_list[x]->adj[y]->name);
		}
	}
	
	exit(0);
}




