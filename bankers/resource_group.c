
#define RESOURCE_TYPES 5

struct res_group {
	struct resource resources[RESOURCE_TYPES];
};

void resources_constructor(struct res_group * available) {
	for(int i = 0; i < RESOURCE_TYPES; i++) {
		resource_constructor(&available->resources[i]);
	}
}

struct res_group * bankers_copy_available(struct res_group * old_available) {
	struct res_group * new_available = (struct res_group *) malloc(sizeof(struct res_group));
	for(int i = 0; i < RESOURCE_TYPES; i++) {
		new_available->resources[i] = old_available->resources[i];
	}

	return new_available;
}

void free_bankers_available(struct res_group * available) {
	free(available);	
}

void acquire_locks(struct res_group * available) {
	for (int i = 0; i < RESOURCE_TYPES; i++) {	
		acquire_lock(&available->resources[i]);
	}
}

void release_locks(struct res_group * available) {
	for (int i = 0; i < RESOURCE_TYPES; i++) {	
		release_lock(&available->resources[i]);
	}
}

void destroy_locks(struct res_group * available) {
	for (int i = 0; i < RESOURCE_TYPES; i++) {	
		destroy_lock(&available->resources[i]);
	}
}

void free_resources(struct res_group * available) {
	for(int i = 0; i < RESOURCE_TYPES; i++) {
		free_resource(&available->resources[i]);
	}
}
