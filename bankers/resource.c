
#define RESOURCE_INFO_SIZE 3

struct resource {
	int resource_info[RESOURCE_INFO_SIZE];
	int amount_avail;
	int microseconds;
	int probability;
	pthread_mutex_t * lock;
	pthread_mutexattr_t * attr;
	int * res_avail;
};


void initialize_lock(struct resource * my_resource) {
	pthread_mutexattr_t * attr = my_resource->attr;
	pthread_mutexattr_init(attr);
	pthread_mutexattr_settype(attr, PTHREAD_MUTEX_RECURSIVE);
	pthread_mutex_init(my_resource->lock, attr);
}

void resource_constructor(struct resource * my_resource) {
	my_resource->lock = (pthread_mutex_t *) malloc(sizeof(pthread_mutex_t));
	my_resource->attr = (pthread_mutexattr_t *) malloc(sizeof(pthread_mutexattr_t *));
	my_resource->res_avail = (int *) malloc(sizeof(int));

	initialize_lock(my_resource);
	my_resource->amount_avail = my_resource->resource_info[0];
	my_resource->microseconds = my_resource->resource_info[1];
	my_resource->probability = my_resource->resource_info[2];
	*my_resource->res_avail = 1;
}


void free_resource(struct resource * my_resource) {
	free(my_resource->lock);
	free(my_resource->attr);
	free(my_resource->res_avail);
}


void acquire_lock(struct resource * my_resource) {
	pthread_mutex_lock(my_resource->lock);	
}

void release_lock(struct resource * my_resource) {
	pthread_mutex_unlock(my_resource->lock);	
}

void destroy_lock(struct resource * my_resource) {
	pthread_mutex_destroy(my_resource->lock);	
}

int is_resource_available(struct resource * my_resource) {
	int * res_avail = my_resource->res_avail;
	int * amount_avail = &my_resource->amount_avail;
	int resource_was_available = 0;

	if(*res_avail && *amount_avail > 0) {
		*res_avail = 0;		
		resource_was_available = 1;
	}

	return resource_was_available;
}

void make_resource_available(struct resource * my_resource) {
	*my_resource->res_avail = 1;
}
