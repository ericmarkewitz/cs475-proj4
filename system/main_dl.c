/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

#define N 2

lid32	printer_lock;
lid32	mylock[N];


/**
 * Delay for a random amount of time
 * @param alpha delay factor
 */
void	holdup(int32 alpha)
{
	long delay = rand() * alpha;
	while (delay-- > 0)
		;	//no op
}

/**
 * Work for a random amount of time
 * @param id ID of worker
 */
void	work(uint32 id)
{
	acquire(printer_lock);
	//kprintf("got printer lock!\n");
	kprintf("Worker %d: Buzz buzz buzz\n", id);
	release(printer_lock);
	holdup(10000);
}


/**
 * Worker code
 * @param id ID of worker
 */
void	worker(uint32 id)
{
	if (id == 0)
	{
		//DELETE THESE
		//kprintf("Worker id 0\n");
		//kprintf("	workerid0 - mylock[0] = %d\n", mylock[0]);
		//kprintf("	workerid0 - mylock[1] = %d\n", mylock[1]);
		//kprintf("about to get mylock[0]\n");
		acquire(mylock[0]);
		work(id);
		acquire(mylock[1]);
		work(id);
		release(mylock[1]);
		release(mylock[0]);
	}
	else
	{
		//kprintf("Worker id 1\n");
		//kprintf("	workerid1 - mylock[0] = %d\n", mylock[0]);
		//kprintf("	workerid1 - mylock[1] = %d\n", mylock[1]);
		acquire(mylock[1]);
		work(id);
		acquire(mylock[0]);
		work(id);
		release(mylock[0]);
		release(mylock[1]);
	}
}

int	main(uint32 argc, uint32 *argv)
{
	int i;
	printer_lock = lock_create();

	//kprintf("Printer lock = %d\n", printer_lock); //DELETE
	
	for (i=0; i<N; i++){
		mylock[i] = lock_create();
		//kprintf("mylock[%d] = %d\n", i, mylock[i]);
	}
	

	ready(create((void*) worker, INITSTK, 15, "Worker 0", 1, 0), FALSE);
	ready(create((void*) worker, INITSTK, 15, "Worker 1", 1, 1), FALSE);

	return 0;
}
