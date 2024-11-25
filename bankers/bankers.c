#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "custom_array.h"
#include "resource.h"
#include "resource_group.h"
#include "process_structure.h"
#include "thread_functions.h"
#include "bankers_outcome.h"
#include "banker_arrays.h"

#define AVAILABLE_INFO 3
#define LINE_LENGTH 40
#define MAX_PROCESSES 10
#define PROCESSES 10
#define RESOURCE_TYPES 5

struct timespec seed; 
struct resource available[RESOURCE_TYPES];
int processes_started = 0;
int processes_finished = 0;

int approve_all = 0;

int print_processes = 0; 

int debug = 0;
void debug_printf(char * message) {
	if(debug)
		printf("%s", message);
}
/*
void get_allocated(struct resource * allocated) {

	for(int i = 0; i < RESOURCE_TYPES; i++) {
		allocated->amount_avail = 0;
		resource_constructor(allocated);
	}
}
*/

void get_processes(struct custom_array * general_processes, struct res_group *available) {
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	char * token;
	char str[LINE_LENGTH];
	const char * delimiter = " ";	 

	fp = fopen("/home/dshunter/coding/learn/learn_c/deadlock/p_resource", "r");

	if(fp == NULL) {
		exit(EXIT_FAILURE);
	}
	int proc_i = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		struct process * this_process = (struct process * ) malloc(sizeof(struct process));
		
		this_process->proc_num = proc_i;
		
		token = strtok(line, delimiter);
		
		for(int i = 0; i < RESOURCE_TYPES; i++) {
			token = strtok(NULL, delimiter);
			this_process->start_resources[i] = atoi(token);
		}

		process_constructor(this_process, available);
		add_element(general_processes, this_process);
		proc_i++;
	}
	fclose(fp);
}

void get_resources(struct res_group * available) {	
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	ssize_t read;

	char * token;
	char str[LINE_LENGTH];
	const char * delimiter = " ";	 
	resources_constructor(available);

	fp = fopen("/home/dshunter/coding/learn/learn_c/deadlock/resources_available", "r");

	if(fp == NULL) {
		exit(EXIT_FAILURE);
	}
	int proc_i = 0;

	while ((read = getline(&line, &len, fp)) != -1) {
		
		token = strtok(line, delimiter);
		
		for(int i = 0; i < AVAILABLE_INFO; i++) {

			token = strtok(NULL, delimiter);
			available->resources[proc_i].resource_info[i] = atoi(token);
			
		}
		resource_constructor(&available->resources[proc_i]);
		proc_i++;
	}
	fclose(fp);
}

int bankers(struct bank_arr * banker_arrays, struct process * original_next) {
	acquire_locks(banker_arrays->available);
	struct bank_arr copy_of_arrays = copy_banker_arrays(banker_arrays);
	struct res_group * available = copy_of_arrays.available;
	struct process * next = bankers_copy_process(original_next, available);
	release_locks(banker_arrays->available);	

	struct custom_array * running_processes = &copy_of_arrays.running;
;
	struct process * current_process_address = (struct process *) malloc(sizeof(struct process));
	int safe_state = 1;	
	int all_processes_finished = 0;
	int size = running_processes->size;
	
	int bankers_running[MAX_PROCESSES];

	int t_index = add_element( running_processes, next);
	for(int i = 0; i < MAX_PROCESSES; i++) {
		bankers_running[i] = index_has_element(running_processes, i);
		if(bankers_running[i] !=  0  && bankers_running[i] != 1) {
			int e = 1;
		}
	}
		
//	check_unique_address(&running_processes, next);
	int loop_counter = 0;

	while(!all_processes_finished && safe_state) {
	loop_counter++;
		int a_process_finished = 0;
		int a_process_running = 0;
		for(int pi = 0; pi < MAX_PROCESSES; pi++) {
			if(bankers_running[pi]) {
				*current_process_address = *get_process( running_processes, pi);
				a_process_running = 1;
				int proc_num = current_process_address->proc_num;
				
				if(resources_available(current_process_address) ) {
					     int proc_n = current_process_address->proc_num;

					deallocate_resources(current_process_address);	
					bankers_running[pi] = 0;
					a_process_finished = 1;
				}
			}
		}


		if(!a_process_finished && a_process_running) {
			safe_state = 0;
		}
		if(!a_process_running) {
			all_processes_finished = 1;
		}
	}

	free(current_process_address);

	return (safe_state && all_processes_finished) || approve_all;
}

struct ban_out identify_next_process( struct bank_arr * banker_arrays) {
	struct custom_array * general_processes = banker_arrays->general_processes;
	struct custom_array * running_processes_address = banker_arrays->running_address;
	struct custom_array * queue_address = banker_arrays->queue_address;
	struct res_group * available_address = banker_arrays->available;
	int running_full = running_processes_address->size >= 10;
	int next_found = 0;
	int process_to_create;
	int bankers_outcome = 0;
	struct process * next_process;
	int bankers_fail = 0;
	struct process * potential_next;
// while next not found and waiting for resources to be freed up
// probably use a semaphore
	
	// 0.1 seconds
	usleep(10000);
	debug_printf("about to scan queue\n");
	for(int i = 0; i < MAX_PROCESSES && !next_found && !running_full; i++) {
		if(index_has_element( queue_address, i) ) {
			potential_next = get_process( queue_address, i);

			if(potential_next->proc_num < 0 || potential_next->proc_num > 9) {
				printf("out of bounds at %p\n", potential_next);
			}
			// run banker's algorithm
			if(bankers(banker_arrays, potential_next ) ) {
			       	next_process = potential_next;
				next_found = 1;
				remove_element(queue_address, i);
			} else {
				bankers_fail = 1;
			}
		}
	}

	if(!next_found && queue_address->size < MAX_PROCESSES && !running_full) {
		potential_next = (struct process *) malloc(sizeof(struct process));

		debug_printf("inf 305: in create next process\n");
		
		check_unique_address(running_processes_address, potential_next);
		check_unique_address(queue_address, potential_next);

		struct timespec seed;
		clock_gettime(CLOCK_REALTIME, &seed);
		srand(seed.tv_nsec);
		int process_index = rand() % MAX_PROCESSES;


			//potential segmentation fault
		*potential_next = *get_process(general_processes, process_index); 
		debug_printf("got potential\n");
	
		if(!bankers(banker_arrays, potential_next) ) {
			add_element( queue_address, potential_next);
			debug_printf("bankers fail created\n");
			bankers_fail = 1;
		} else {
			next_process = potential_next;
			debug_printf("inf 411: bankers success created\n");
			next_found = 1;	
		}
	} 

	if(debug && next_found) {
		int out_of_bounds = next_process->proc_num < 0 || next_process->proc_num > 10;
		if(out_of_bounds) {
			int d = 1;
		}
		
		check_unique_address(running_processes_address, next_process);
		check_unique_address(queue_address, next_process);
	}

	struct ban_out b_outcome;
	b_outcome.outcome = next_found && !bankers_fail;	
	if(next_found) {
		b_outcome.next_process = next_process;
	}
	return b_outcome;
}

int safe_to_allocate(struct bank_arr * banker_arrays, int index, int resource_type) {
        struct res_group * available = banker_arrays->available;

	banker_arrays->running_address->index_to_track = index;
        struct bank_arr copy_of_arrays = copy_banker_arrays(banker_arrays); 
        struct process * current_process = get_process( &copy_of_arrays.running, copy_of_arrays.running.index_to_track);

	do_process(current_process, resource_type);
	copy_of_arrays.running_address = &copy_of_arrays.running;
        int safe = bankers(&copy_of_arrays, NULL);
        
	free_banker_arrays(&copy_of_arrays);
        return safe;
}

int a_process_can_finish(struct custom_array * running) {
	int finished = 0;
	int index = -1;
	for(int i = 0; i < MAX_PROCESSES && !finished; i++) {
		if(index_has_element(running, i)) {
			struct process * next = get_process(running, i);
			finished = resources_available(next);
			if(finished) {
				index = i;
			}
		}
	}

	return index;
}

void * finish_process(void * arg) {
	struct thread_args *args = (struct thread_args * ) arg;
	int index = args->index;
	struct bank_arr * banker_arrays = args->banker_arrays;
	struct custom_array * running_processes = args->banker_arrays->running_address;
	struct custom_array * general_processes = args->banker_arrays->general_processes;
	struct res_group * available = args->banker_arrays->available;
	struct process * next = args->next_address;
	int next_probability = 0;
	int finished = 0;
	int loop_counter = 0;

	debug_printf("in finish process\n");	

	while (!finished && loop_counter < 50000 ) {

		int can_finish = a_process_can_finish(running_processes);

		//prevent looping on rand variable associated with time
		usleep(100);
		int resources_needed = 0;
		int total_resources = 0;	
		int cumulative_probability = 0;
		int levels[RESOURCE_TYPES];
		
		// figure out probabilty distribution for next resource to process
		for (int i = 0; i < RESOURCE_TYPES; i++) {
			levels[i] = 0;
			if (next->res_needed[i] > 0 && *available->resources[i].res_avail) {
				cumulative_probability += available->resources[i].probability;
				levels[i] = cumulative_probability;
				total_resources += next->res_needed[i];	
				resources_needed++;	
			}
		}
		if(cumulative_probability == 0) {
			continue;	
		}

		if(next->proc_num < 0 || next->proc_num > 9) {
			int d = 1;
		}
		struct timespec seed;
		clock_gettime( CLOCK_REALTIME, &seed);

		// allocate resources for process
		srand(seed.tv_nsec);
		int prob_chosen = rand() % cumulative_probability;
		
		int chosen = 0;
		int i = 0;
		int decision_made = 0;
		for(int i = 0; i < RESOURCE_TYPES && !chosen; i++) {
			chosen = levels[i] > prob_chosen;
			if(chosen) {
				struct resource * this_resource = &available->resources[i];
				int index = next->index;
								
				acquire_locks(available);
				int resource_available = is_resource_available(this_resource);
				int allocatable = safe_to_allocate(banker_arrays, index, i);
				if(resource_available && allocatable) {
					do_process(next, i);
				}

				release_locks(available);

				if(resource_available) {
					make_resource_available(this_resource);
				} 
				release_locks(available);
				if(!allocatable) {
					usleep(30000);
				}
			}	
		}

		if(print_processes) {
			print_process(next);
		}

		finished = next->finished;
		loop_counter++;
	}

	if(next->finished == 1) {
		processes_finished++;
	}
	
	deallocate_resources(next);

	if(print_processes) {
		printf("after deallocation\n");
		print_process(next);
	}

	free(next);
	
	remove_element( running_processes, index );
}

int main(int argc, char *argv[]) {
	struct bank_arr banker_arrays;
	struct res_group available_value;
	struct custom_array general_processes_value;
	struct custom_array running_value;
	struct custom_array queue_value;
	
	banker_arrays.available = &available_value;
	banker_arrays.general_processes = &general_processes_value;
	banker_arrays.running_address = &running_value;
	banker_arrays.queue_address = &queue_value;

	struct res_group *available = banker_arrays.available;
	struct custom_array * general_processes = banker_arrays.general_processes;
	struct custom_array * running = banker_arrays.running_address;	
	struct custom_array * queue = banker_arrays.queue_address;
	
	// set up 		
	get_resources(available);

	custom_array_constructor(general_processes);
	custom_array_constructor(running);
	custom_array_constructor(queue);

	get_processes(general_processes, available);

	// while the program is unfinished, (1) take process from queue or create it
	struct timespec start;
	struct timespec end;
	clock_gettime( CLOCK_REALTIME, &start);
	clock_gettime( CLOCK_REALTIME, &end);
	
	pthread_t thread_id[MAX_PROCESSES];
	int highest_thread_index = 0;

	debug_printf("set up complete\n");	

       	while(end.tv_sec - start.tv_sec <= 10) {
		struct ban_out b_outcome  = identify_next_process( &banker_arrays);
		struct process * next_process_address;

		if(b_outcome.outcome) {
			processes_started++;
			next_process_address = b_outcome.next_process;
		} else {
			usleep(10000);
			continue;
		}
		debug_printf("next found\n");

		// put next running into running stack and account for this
		int running_index = add_element( running, next_process_address);
		next_process_address->index = running_index;
		if(running_index > highest_thread_index) {
			highest_thread_index = running_index;
		}
		debug_printf("element added \n");
	
		// allocate for added process
		int can_finish = a_process_can_finish(running);

		debug_printf("allocated resources\n");
	
		struct thread_args args;
		args.banker_arrays = &banker_arrays;
		args.next_address = next_process_address;
		args.index = running_index;

		debug_printf("about to do multi-threading\n");
		pthread_create(&thread_id[running_index], NULL, finish_process, (void *)&args);
		debug_printf("after multi-threading\n");
		clock_gettime( CLOCK_REALTIME, &end);
	}	

	for(int i = 0; i <= highest_thread_index; i++) {
		pthread_join(thread_id[i], NULL);
	}
	
	//give all processes a second to finish up
	sleep(1);	

	destroy_locks( available);
	free_resources(available);
	printf("processes started: %d\n", processes_started);
	printf("processes ended: %d\n", processes_finished);
	//account for finished process
	exit(EXIT_SUCCESS);
}

