/*  main.c  - main */

#include <xinu.h>
#include <stdio.h>
#include <stdlib.h>

#define N 3	//number of philosophers and forks

//TODO - locks must be declared and initialized here

//mutex_t	lock = FALSE;
//mutex_t	fork[N] = {FALSE, FALSE, FALSE, FALSE, FALSE};

lid32 lock; //NEW
lid32 fork[N]; //NEW


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
 * Eat for a random amount of time
 */
void	eat()
{
	holdup(10000);
}

/**
 * Think for a random amount of time
 */
void	think()
{
	holdup(1000);
}



/**
 * Philosopher's code
 * @param phil_id philosopher's id
 */
void	philosopher(uint32 phil_id)
{
	uint32 right = phil_id;				//right fork
	uint32 left = N - ((N-phil_id) % N) - 1;	//left fork

	while (TRUE)
	{
		//think 70% of the time
		if ((rand()*1.0)/RAND_MAX < 0.7)
		{
			//mutex_lock(&lock);
			acquire(lock); //NEW CHANGE HERE
			//kprintf("Lock value: %u\n", locktab[lock].lock);
			kprintf("Philosopher %d thinking: zzzzzZZZz\n", phil_id);
			
			//mutex_unlock(&lock);
			release(lock); //NEW CHANGE HERE


			think();
		}
		else	//eat 30% of the time
		{
			//mutex_lock(&fork[right]);	//grab the right fork (or wait)
			acquire(fork[right]); //NEW

			//kprintf("\n\nlocktab[fork[left]].lock = %u\n\n", locktab[fork[left]].lock);

			//if (locktab[fork[left]].lock == FALSE) //NEW ISSUE HERE I THINK
			//{

			//mutex_lock(&fork[left]);	//grab the left fork
			acquire(fork[left]);

			//mutex_lock(&lock);
			acquire(lock); //NEW

			kprintf("Philosopher %d eating: nom nom nom\n", phil_id);
			
			//mutex_unlock(&lock);
			release(lock); //NEW

			eat();

			//mutex_unlock(&fork[left]);
			//mutex_unlock(&fork[right]);
			release(fork[left]); //NEW
			release(fork[right]); //NEW

			//}
			/*
			else
			{
				//mutex_unlock(&fork[right]);	//relinquish right fork
				release(fork[right]);
			}*/
		}
	}
}

int	main(uint32 argc, uint32 *argv)
{
	//NEW
	lock = lock_create();
	//kprintf("Lock value: %d\n", lock);
	for(int i=0; i<N; i++){
		fork[i] = lock_create();
		//kprintf("Fork %d value: %d\n", i, fork[i]);
	}

	
	ready(create((void*) philosopher, INITSTK, 15, "Ph1", 1, 0), FALSE);
	ready(create((void*) philosopher, INITSTK, 15, "Ph2", 1, 1), FALSE);
	ready(create((void*) philosopher, INITSTK, 15, "Ph3", 1, 2), FALSE);
	//ready(create((void*) philosopher, INITSTK, 15, "Ph4", 1, 3), FALSE);
	//ready(create((void*) philosopher, INITSTK, 15, "Ph5", 1, 4), FALSE);

	return 0;
}
