#define MAX_PROCESSES 10

struct bank_arr {
	//snapshot in time for using bankers algorithm
	struct custom_array running;
	struct custom_array queue;

	//location to keep track of the actual processes running
	struct res_group * available;
	struct custom_array * general_processes;
	struct custom_array * running_address;
	struct custom_array * queue_address;
};


struct custom_array copy_list(struct custom_array * list, struct res_group * updated_available) {
	struct custom_array copy_list;
	custom_array_constructor(&copy_list);

	for(int i = 0; i < MAX_PROCESSES; i++) {
		if(index_has_element(list, i)) {
			struct process * new_process;
		      	struct process * old_process;
			old_process = get_process(list, i);

			new_process = bankers_copy_process(old_process, updated_available);
//			struct process copy_process = bankers_copy_process(this_process); 
//			process_constructor( &copy_process, updated_available);
			int place_in_list = add_element( &copy_list, new_process);
			new_process->index = place_in_list;
			if(i == list->index_to_track) {
				copy_list.index_to_track = place_in_list;
			}
		}
	}
	return copy_list;
}

struct bank_arr copy_banker_arrays(struct bank_arr * banker_arrays) {
	struct bank_arr copy_banker_arrays;
	struct custom_array copy_running;
		
	custom_array_constructor(&copy_running);

	copy_banker_arrays.available = bankers_copy_available(banker_arrays->available);
	copy_banker_arrays.running = copy_list( banker_arrays->running_address, copy_banker_arrays.available);
	copy_banker_arrays.general_processes = banker_arrays->general_processes;
	
	return copy_banker_arrays;
}

void free_custom_array(struct custom_array * list) {	
	for(int i = 0; i < MAX_PROCESSES; i++) {
		if(index_has_element(list, i)) {
			struct process * this_process = get_process(list, i);
			free_process(this_process);
		}
	}
}

void free_banker_arrays(struct bank_arr * banker_arrays) {
	free_custom_array(&banker_arrays->running);
//	free_custom_array(&banker_arrays->queue);
	free_bankers_available(banker_arrays->available);
}

void set_up_array_values(struct bank_arr * bank_arrays) {
	
}
