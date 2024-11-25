#define MAX_PROCESSES 10
struct thread_args {
	struct bank_arr * banker_arrays;
	struct process * next_address;
	int index;
};
