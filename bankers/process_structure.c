
#define RESOURCE_TYPES 5
#define HELPER_SIZE 10

struct process {
        int proc_num;
	int finished;
	int malloced;
        int res_needed[RESOURCE_TYPES];
        int start_resources[RESOURCE_TYPES];
	struct res_group * available;

	// where in the list is the process
	int index;
};

void process_constructor(struct process * my_process, struct res_group * available) {
	my_process->finished = 0;
	my_process->available = available;
	my_process->malloced = 0;

	for(int i = 0; i < RESOURCE_TYPES; i++) {
		my_process->res_needed[i] = my_process->start_resources[i];
	}
}

struct process * bankers_copy_process( struct process * old_process, struct res_group * available) {
	if(old_process == NULL) {
		return NULL;
	}

	struct process * new_process = (struct process * ) malloc(sizeof(struct process));
        *new_process = *old_process;
	new_process->available = available;
	new_process->malloced = 1;

	return new_process;
}

static int change_resources(int * res_avail, int change) {
	*res_avail += change;
	return *res_avail;
}

static int change_res_safe(pthread_mutex_t * lock, int * res_needed, int * res_avail, int available_up, int amount, int malloced) {
	int change_avail = 0;

	if(available_up) {
		change_avail += amount;
	} else {
		change_avail -= amount; 
	}
	if(!malloced) {
		pthread_mutex_lock(lock);
	}
		change_resources(res_avail, change_avail);
		change_resources(res_needed, change_avail);
	if(!malloced) {
		pthread_mutex_unlock(lock);
	}
}

void print_process(struct process * my_process) {
	char process_string[100];
	char available_string[100];
	char process_helper[HELPER_SIZE];
	char available_helper[HELPER_SIZE];
	int a = 0;
	int i = 0;

	sprintf(process_helper, "%d", my_process->proc_num);
	int n = 0;

	while(process_helper[n] != '\0' && n < HELPER_SIZE) {
		process_string[i] = process_helper[n];
		n++;
		i++;
	}
	
	process_string[i] = ',';
	i++;
	available_string[a] = ' ';
	a++;
	available_string[a] = ' ';
	a++;

	for(int m = 0; m < RESOURCE_TYPES; m++) {
		sprintf(process_helper, "%d", my_process->res_needed[m]);
		sprintf(available_helper, "%d", my_process->available->resources[m].amount_avail);

		int x = 0;
		process_string[i] = ' ';
		i++;
		while(process_helper[x] != '\0' && x < HELPER_SIZE) {
			process_string[i] = process_helper[x];
			i++;
			x++;
		}

		int y = 0;
		available_string[a] = ' ';
		a++;
		while(available_helper[y] != '\0' && y < HELPER_SIZE) {
			available_string[a] = available_helper[y];
			a++;
			y++;
		}
	}

	process_string[i] = '\0';
	available_string[a] = '\0';
	printf("ps 112: res_need process: %s\n", process_string);
	printf("ps 113: available amount: %s\n", available_string);
}

int is_process_finished(struct process * this_process) {
	 int process_finished = 1;
	 int error = 0;
         for (int i = 0; i < RESOURCE_TYPES && !error; i++) {
         	if (this_process->res_needed[i] > 0) {
	                process_finished = 0;
                } else if(this_process->res_needed[i] < 0) {
			printf("process_structure.c:63 ERROR: negative resources\n");
			error = 1;
			process_finished = -1;
		}
	 }

	 return process_finished;
}

int decrease_res_needed(struct process * this_process, int resource_type, int amount) {
	int available_up = 0;
	int * res_needed;
	int * res_avail;
	pthread_mutex_t * lock = this_process->available->resources[resource_type].lock;
	
	res_needed = &this_process->res_needed[resource_type];
       	res_avail = &this_process->available->resources[resource_type].amount_avail;
	int malloced = this_process->malloced;
	if(*res_needed > 0) {
		change_res_safe(lock, res_needed, res_avail, available_up, amount, malloced);
	} else {
		printf("process_structure.c:116: can't decrease resource needed\n");
	}
	if(*res_avail < 0) {
		int e = 1;
	}
	if(this_process->res_needed[resource_type] <= 0) {
		this_process->finished = is_process_finished(this_process);
	}
}

int decrement_res_needed(struct process * this_process, int resource_type) {
	int amount = 1;	
	decrease_res_needed(this_process, resource_type, amount);
}

int increase_res_needed(struct process * this_process, int resource_type, int amount) {
	int available_up = 1;
	int * res_avail = &this_process->available->resources[resource_type].amount_avail;
	pthread_mutex_t * lock = this_process->available->resources[resource_type].lock;
	int * res_needed = &this_process->res_needed[resource_type];
	int malloced = this_process->malloced;

	change_res_safe(lock, res_needed, res_avail, available_up, amount, malloced);
}

int deallocate_resources(struct process * this_process) {
	int available_up = 1;
	
	for(int i = 0; i < RESOURCE_TYPES; i++) {
		int resources_used = this_process->start_resources[i] - this_process->res_needed[i];
		increase_res_needed(this_process, i, resources_used);		 
	}
}

int resources_available(struct process * this_process) {
        int resources_sufficient = 1;

        for(int i = 0; i < RESOURCE_TYPES && resources_sufficient; i++) {
                if(this_process->available->resources[i].amount_avail < this_process->res_needed[i]) {
                        resources_sufficient = 0;
                }
        }

        return resources_sufficient;
}

void free_process( struct process * my_process) {
	if(my_process->malloced) {
		free(my_process);
	}
}	

void do_process(struct process * my_process, int resource_type) {
	int * amount_avail = &my_process->available->resources[resource_type].amount_avail;
	useconds_t usec = my_process->available->resources[resource_type].microseconds;
	usleep(usec);
	decrement_res_needed(my_process, resource_type);

	if(*amount_avail < 0) {
		int e = 1;
	}
	
}
