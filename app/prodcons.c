#include <ucx.h>

#define N 10

struct sem_s *empty, *full, *mutex;
int32_t in = 0, out = 0, buffer[N];

void producer(void)
{
	int32_t item;

	ucx_task_init();

	for (;;) {
		item = _random();
		ucx_wait(empty);
		ucx_wait(mutex);
		buffer[in] = item;
		_printf("\nproducer %d putting at %ld (%ld)", ucx_task_id(), in, item);
		in = (in + 1) % N;
		ucx_signal(mutex);
		ucx_signal(full);
	}
}

void consumer(void)
{
	int32_t item;

	ucx_task_init();

	for (;;) {
		ucx_wait(full);
		ucx_wait(mutex);
		item = buffer[out];
		_printf("\nconsumer %d getting from %ld (%ld)", ucx_task_id(), out, item);
		out = (out + 1) % N;
		ucx_signal(mutex);
		ucx_signal(empty);
	}
}

int32_t app_main(void)
{
	ucx_task_add(producer, DEFAULT_GUARD_SIZE);
	ucx_task_add(consumer, DEFAULT_GUARD_SIZE);
	ucx_task_add(consumer, DEFAULT_GUARD_SIZE);

	empty = ucx_seminit(N);
	full = ucx_seminit(0);
	mutex = ucx_seminit(1);
	
	return 1;
}
