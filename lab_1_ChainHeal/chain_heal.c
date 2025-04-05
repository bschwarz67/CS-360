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
	int visited;
	int starting_node;
} Node;


typedef struct global_variables
{
	int initial_range, jump_range, num_jumps, initial_power, best_healing;
	double power_reduction;

} Global;

int DFS(Node *node, int jump_number, Global *global, int total_healing) {
	printf("Node:%s Jump %d\n", node->name, jump_number);
	node->visited = 1;
	jump_number++;
	if(node->max_PP - node->cur_PP <= global->initial_power) {
		total_healing += (node->max_PP - node->cur_PP);
	}
	else {
		total_healing += global->initial_power;
	}
	int initial_power_temp = global->initial_power;
	global->initial_power = rint(global->initial_power * (1 - global->power_reduction));
	if(total_healing > global->best_healing) global->best_healing = total_healing;
	for(int x = 0; x < node->adj_size; x++) {
		if(node->adj[x]->visited != 1 && jump_number <= global->num_jumps) {
			int res = DFS(node->adj[x], jump_number, global, total_healing);
		}
	}
	global->initial_power = initial_power_temp;
	node->visited = 0;
	return 0;
}


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
	power_reduction = atof(argv[5]);

	Global *global = (Global *) malloc(sizeof(Global));
	global->initial_range = initial_range;
	global->jump_range = jump_range;
	global->num_jumps = num_jumps;
	global->initial_power = initial_power;
	global->power_reduction = power_reduction;
	printf("argv[5] %s\n", argv[5]);
	printf("initial power %d\n", initial_power);
	printf("power reduction %f\n", power_reduction);

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
		new_node->visited = 0;
		new_node->starting_node = 0;
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
	//check if it qualifies as a starting node, set that on node, then fill adj list
	for(x = 0; x < node_number; x++) {
		if(strcmp(player_list[x]->name, "Urgosa_the_Healing_Shaman") == 0) {
			for(y = 0; y < node_number; y++) {
				x_distance = player_list[x]->x - player_list[y]->x;
				y_distance = player_list[x]->y - player_list[y]->y;
				distance = sqrt(x_distance*x_distance + y_distance*y_distance);
				if(distance <= initial_range) player_list[y]->starting_node = 1;
			}
		}
	}
	for(x = 0; x < node_number; x++) {
		
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
	for(x = 0; x < node_number; x++) {
		printf("adj list %s\n", player_list[x]->name);
		for(y = 0; y < player_list[x]->adj_size; y++) {
			printf("%s\n", player_list[x]->adj[y]->name);
		}
	}
	for(x = 0; x < node_number; x++) {
		if(player_list[x]->starting_node == 1) {
			printf("starting node found\n");
			int res = DFS(player_list[x], 1, global, 0);
		}
	}
	printf("best healing: %d\n", global->best_healing);

	exit(0);
}




