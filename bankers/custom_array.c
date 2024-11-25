#define MAX_ELEMENTS 10

struct custom_array {
	void * array[MAX_ELEMENTS];
	int size;
	int index_to_track;
};

void custom_array_constructor(struct custom_array * array_wrapper) {
	array_wrapper->size = 0;
	array_wrapper->index_to_track = -1;
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		array_wrapper->array[i] = NULL;
	}
}

int get_size(struct custom_array * array_wrapper) {
	int size = 0; 
	for(int i = 0; i < MAX_ELEMENTS; i++) {
		if(array_wrapper->array[i] != NULL) {
			size++;
		}
	}
	return size;
}

void * get_element(struct custom_array * array_wrapper, int i) {
	if (i < 0 || i > 9) {
		int e = 1;
	}
	return array_wrapper->array[i];
}

struct process * get_process(struct custom_array * array_wrapper, int i) {
	return (struct process *) get_element(array_wrapper, i);
}

void remove_element(struct custom_array * array_wrapper, int i) {
	array_wrapper->array[i] = NULL;
	array_wrapper->size = get_size(array_wrapper);	
}

int index_has_element(struct custom_array * array_wrapper, int i) {
	int has_element = 1;
	if(array_wrapper->array[i] == NULL) {
		has_element = 0;
	}	
	
	return has_element;
}

/*
int add element_value( struct custom_array * array_wrapper, void * element) {
	add_element(array_wrapper, 
}
*/

int add_element(struct custom_array * array_wrapper, void * element) {
	int placed = 0;
	int i = 0;
	
	for(; i < MAX_ELEMENTS && !placed && element != NULL; i++) {
		if(array_wrapper->array[i] == NULL) {
			array_wrapper->array[i] = element;
			array_wrapper->size = get_size(array_wrapper);
			placed = 1;
		}
	}

	if(!placed) {
		i = -1;
	} else {
		i--;
	}

	return i;
}

int check_unique_address(struct custom_array * array_wrapper, void * element) {
//	printf("element pointer : %p\n", element);
	
	for(int i = 0; i < MAX_ELEMENTS; i++) {
//		printf("array[%d] pointer : %p\n", i, array_wrapper->array[i]);
		if(array_wrapper->array[i] == element) {
			printf("ERROR matching pointers\n");
		}
	}
}

