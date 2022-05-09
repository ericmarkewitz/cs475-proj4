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
        //kprintf("Outside loop: %d\n", i);
        
        int visited[NPROC];
        for(j=0; j<NPROC; j++){
            visited[j] = 0;
        }

        int order[NPROC + NLOCK];
        for(j=0; j<NPROC+NLOCK; j++){
            
            order[j] = -1;
        }

        rag_print();
        if(deadlock_detect_Helper(i, visited, order)){
            return;
        }
    }
    kprintf("NO CYCLE\n");
    return;
}

void deep_copy_arr(int arr[], int copyArr[]){
    for(int i=0; i<NPROC; i++){
        copyArr[i] = arr[i];
    }
}

void deep_copy_order(int arr[], int copyArr[]){
    for(int i=0; i<NPROC + NLOCK; i++){
        copyArr[i] = arr[i];
    }
}

void print_arr(int arr[]){
    kprintf("[");
    for(int i=0; i<NPROC; i++){
        kprintf("%d, ", arr[i]);
    }
    kprintf("]");
}

void print_order(int arr[]){
    int i=0;
    while(arr[i] != -1){
        if((i % 2) ==0){
            kprintf("PID: %d wants lock ", arr[i]);
        }
        else{
            kprintf("%d owned by ", arr[i]);
        }
        
        i++;
    }
}

int deadlock_detect_Helper(int pid, int visited[], int order[]){
    //printf("Inside %d\n", pid);
   
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
    
    
    //print_arr(visitedCopy);
    //printf("\n");

    //Check if it's requesting any locks
    for(int i=0; i<NLOCK; i++){
        if(RAG[pid+NLOCK][i] == 1){
            //printf("    Requests lock %d\n", i);
            
            //If this process is requesting a lock, see if another process has that lock
            for(int j=0; j<NPROC; j++){
                
                //If that lock is held by another thread explore it
                if(RAG[i][j+NLOCK] == 1){
                    visitedCopy[pid] = 1;
                    for(int i=0; i<NPROC; i++){
                        if(orderCopy[i] == -1){
                            orderCopy[i] = pid;
                            orderCopy[i+1] = j;
                            break;
                        }
                    }

                    if(visitedCopy[j] == 1){
                        kprintf("THERE IS A CYCLE\n");
                        print_order(orderCopy);
                        kprintf("PID: %d\n", j);
                        return 1;
                    }
                    else{
                        return deadlock_detect_Helper(j, visitedCopy, orderCopy);
                    }
                }
            }
        }
    }
    return 0;
}

void deadlock_recover(){

}

/*
void clear_rag(){
    for(int i=0; i<NLOCK+NPROC; i++){
        for(int j=0; j<NLOCK+NPROC; j++){
            RAG[i][j] = 0;
        }
    }
}*/
