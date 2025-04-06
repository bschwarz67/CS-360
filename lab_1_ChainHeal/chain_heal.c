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
	int healing;
} Node;


typedef struct global_variables
{
	int initial_range, jump_range, num_jumps, initial_power, best_healing;
	double power_reduction;
	int best_path_length;
	Node **best_path;
	int *healing;


} Global;

int DFS(Node *node, int jump_number, Global *global, int total_healing) {
	
	node->visited = 1;
	jump_number++;
	int healing;
	if(node->max_PP - node->cur_PP <= global->initial_power) {
		healing = (node->max_PP - node->cur_PP);
	}
	else {
		healing = global->initial_power;
	}
	total_healing += healing;
	node->healing = healing;
	if(total_healing > global->best_healing) {
		global->best_healing = total_healing;
		global->best_path_length = 0;
		Node *n = node;
		while(n != NULL) {
			global->best_path[global->best_path_length] = n;
			global->healing[global->best_path_length] = n->healing;
			global->best_path_length++;
			n = n->prev;
		}
	}
	int initial_power_temp = global->initial_power;
	global->initial_power = rint(global->initial_power * (1 - global->power_reduction));
	for(int x = 0; x < node->adj_size; x++) {
		if(node->adj[x]->visited != 1 && jump_number <= global->num_jumps) {
			node->adj[x]->prev = node;
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
	}


	Global *global = (Global *) malloc(sizeof(Global));
	global->initial_range = initial_range;
	global->jump_range = jump_range;
	global->num_jumps = num_jumps;
	global->initial_power = initial_power;
	global->power_reduction = power_reduction;
	global->best_path = (Node **) malloc(sizeof(Node *) * num_jumps);
	global->healing = (int *) malloc(sizeof(int) * num_jumps);

	player_list = (Node **) malloc(sizeof(Node *) * node_number);
	x = 0;
	while(new_node != NULL) {
		player_list[x] = new_node;
		new_node = new_node->prev;
		x++;
	}

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
		if(player_list[x]->starting_node == 1) {
			player_list[x]->prev = NULL;
			int res = DFS(player_list[x], 1, global, 0);
		}
	}
	for(x = global->best_path_length - 1; x >= 0; x--) {
		printf("%s %d\n", global->best_path[x]->name, global->healing[x]);
	}
	printf("Total_Healing %d\n", global->best_healing);
	exit(0);
}




