#include <unistd.h>

#define RESOURCE_INFO_SIZE 3

struct resource {
	int resource_info[RESOURCE_INFO_SIZE];
	int * amount_avail;
	int * microseconds;
	int * probability;
	useconds_t * usec;
};

void resource_constructor(struct res_avail * my_resources) {
	my_resources->amount_avail = &my_resources->resource_info[0];
	my_resources->microseconds = &my_resources->resource_info[1];
	my_resources->probability = &my_resources->resource_info[2];
}
