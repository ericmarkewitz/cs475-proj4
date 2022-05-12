#include <xinu.h>
#include "deadlock.h"
//#include <stdio.h>
//#include <stdlib.h>

//TODO - add in your functions

/*
* adds a request edge from pid to lockid
*/
void rag_request(int pid, int lockid){
    //index of the pid is pid + NLOCK
    RAG[pid+NLOCK][lockid] = 1;
}

/*
* adds an allocation edge to RAG from lockid to pid and 
* removes the request edge from pid to lockid
*/
void rag_alloc(int pid, int lockid){
    //kprintf("RAG_ALLOC- pid: %d, lockid: %d\n", pid, lockid);
    RAG[lockid][pid+NLOCK] = 1;
    RAG[pid+NLOCK][lockid] = 0;
}

/*
* removes the request or allocation edge from lockid to pid
*/
void rag_dealloc(int pid, int lockid){
    RAG[lockid][pid+NLOCK] = 0;
    RAG[pid+NLOCK][lockid] = 0;
}

/*
* prints the RAG
*/
void rag_print(){
    int i,j;
    for(i=0; i<NLOCK+NPROC; i++){
        if (i == NLOCK){
            for(j=0; j<NLOCK+NPROC; j++){
                kprintf("_");
            }
            kprintf("\n");
        }
        for(j=0; j<NLOCK+NPROC; j++){
            if (j == NLOCK){
                kprintf("|");
            }
            kprintf("%d", RAG[i][j]);
        }
        kprintf("\n");
    }
    kprintf("\n\n\n\n");
}


/*
* checks the RAG for cycles. for each cycle prints out deadlock
* followed by the involved nodes, if there are no cycles then no
* output. The cycle detection is a depth first search.
*/
void deadlock_detect(){
    int i,j;
    for (i=0; i<NPROC; i++){
        
        int visited[NPROC];
        for(j=0; j<NPROC; j++){
            visited[j] = 0;
        }

        int order[NPROC + NLOCK];
        for(j=0; j<NPROC+NLOCK; j++){
            
            order[j] = -1;
        }

        if(deadlock_detect_Helper(i, visited, order)){
            //Cycle if we're in here
            return;
        }
    }
    //No cycle if we got here
    return;
}

/*
* Does a deep copy of the allocation array
*/
void deep_copy_arr(int arr[], int copyArr[]){
    for(int i=0; i<NPROC; i++){
        copyArr[i] = arr[i];
    }
}

/*
* Does a deep copy of the deadlock order array
*/
void deep_copy_order(int arr[], int copyArr[]){
    for(int i=0; i<NPROC + NLOCK; i++){
        copyArr[i] = arr[i];
    }
}

/*
* Prints the allocation array
*/
void print_arr(int arr[]){
    kprintf("[");
    for(int i=0; i<NPROC; i++){
        kprintf("%d, ", arr[i]);
    }
    kprintf("]");
}

/*
* Prints the deadlock order array
*/
void print_order(int arr[]){
    int i=0;
    while(arr[i] != -1){
        if((i % 2) ==0){
            kprintf("PID %d wants lock ", arr[i]);
        }
        else{
            kprintf("%d owned by ", arr[i]);
        }
        
        i++;
    }
    //kprintf("\n");
}

/*
* Recursive helper function called by the deadlock detection function
* Basically sees what process its in, looks at the locks its requesting
* checks if those locks have been allocated, and if so check if we have
* already looked at them, if not recurse on them, if we have there is a cycle
* if we reach the end every time, there has been no cycle
*/
int deadlock_detect_Helper(int pid, int visited[], int order[]){
 
    //Create a copy of the array and set the pid value to visited
    int visitedCopy[NPROC];
    for(int i=0; i<NPROC; i++){
        visitedCopy[i] = 0;
    }
    deep_copy_arr(visited, visitedCopy);
    

    //Create a copy of the order with the most recent value being the current element
    int orderCopy[NPROC+NLOCK];
    for(int i=0; i<NPROC+NLOCK; i++){
        orderCopy[i] = -1;
    }
    deep_copy_order(order, orderCopy);
    

    //Check if it's requesting any locks
    for(int i=0; i<NLOCK; i++){
        if(RAG[pid+NLOCK][i] == 1){
            
            //If this process is requesting a lock, see if another process has that lock
            for(int j=0; j<NPROC; j++){
                
                //If that lock is held by another thread explore it
                if(RAG[i][j+NLOCK] == 1){
                    visitedCopy[pid] = 1;
                    for(int k=0; k<NPROC; k++){
                        if(orderCopy[k] == -1){
                            orderCopy[k] = pid;
                            orderCopy[k+1] = i;
                            break;
                        }
                    }

                    //Uh oh! We've visited this node before
                    if(visitedCopy[j] == 1){
                        kprintf("DEADLOCK DETECTED      ");
                        print_order(orderCopy);
                        kprintf("PID %d\n", j);
                        deadlock_recover(orderCopy);
                        return 1;
                    }
                    else{
                        //If not recurse!
                        return deadlock_detect_Helper(j, visitedCopy, orderCopy);
                    }
                }
            }
        }
    }
    return 0; //No cycle in this process
}

void deadlock_recover(int order[]){
    //Find first lock in the cycle (index 1 of order) & get its lockentry
    int indexOfLock = 1;
    int lockid = order[indexOfLock];
    struct lockentry *wantedLock = &locktab[lockid];

    //Kill the process that is currently holding it
    int procId;
    for(procId=0; procId<NPROC; procId++){
        if(RAG[lockid][procId+NLOCK] == 1){
            kill(procId);
            break;
        }
    }

    //Remove the process from the wait queue of other locks
    for(int i=0; i<NLOCK; i++){
        struct lockentry *currLock = &locktab[i];
        struct queue *waitQ = currLock->wait_queue;

        if(getbypid(procId, waitQ) != NULL){
            remove(procId, waitQ);
        }
    }

    //Call mutex_unlock on lock's mutex
    mutex_unlock(&wantedLock->lock);

    //Reset RAG allocs and requests to 0 for that process
    for(int i=0; i<NLOCK; i++){
        RAG[i][procId+NLOCK] = 0;
        RAG[procId+NLOCK][i] = 0;
    }

    //Let the user know what you just did
    kprintf("DEADLOCK RECOVER       killing pid %d to release lockid %d\n", procId, lockid);

}
