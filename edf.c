/*******************************************************************************
* Filename : edf.c
* Author : Ryan Piedrahita
* Date : 3 / 4 / 2024
* Description : Earliest Deadline First Scheduling Algorithm
* Pledge : I pledge my honor that I have abided by the Stevens Honor System
******************************************************************************/
#include <stdio.h>
#include <stdlib.h>

//iterators
int num = 1;
int i = 0;

// Process structures for storage
struct process {
    unsigned short pid;
    unsigned int cputime;
    int period;
};
struct runningProcess {
    struct process *proc;
    unsigned int age;
    unsigned int timeSpent;
    unsigned int timeLeft;
    int tid;
};
// Ready Queue functions 
//  Add to Ready Queue
void addQ(struct runningProcess *q, int *len, struct process *p) {
    struct runningProcess temp;
    temp.proc = p;
    temp.timeLeft = temp.proc->period;
    temp.timeSpent = 0;
    temp.age = 0;
    temp.tid = num;
    num++;
    q[*len] = temp;
    *len = *len+1;
}
//  Remove from Ready Queue
void removeQ(struct runningProcess *q, int *len, struct runningProcess *proc) {
    int reached = 0;
    for (int i = 0; i < *len; i++) { //moves every element after selection to the left (so deletes selection)
        if(reached) {
            q[i-1] = q[i];
        } else {
            if(&q[i] == proc) {
                reached = 1;
            }
        }
    }
    if(!reached) printf("not found"); //err
    else *len = *len-1;
    return;
}
// functions for finding the lcm max time
unsigned int gcd2(int a, int b) {
    if (a == 0)
        return b;
    return gcd2(b % a, a);
}
unsigned int lcm2(int a, int b) {
    return a * b / gcd2(a, b);
}
unsigned int computeLCM(int *periodArray, unsigned int len) {
    int a = periodArray[0];
    int b;
    for (int i = 1; i < len; i++) {
        b = periodArray[i];
        a = lcm2(a, b);
    }
    return a;
}
//end lcm
//  Comparison function for the DEADLINE quicksort
int compare(const void *v1, const void *v2) {
    const struct runningProcess *p1 = (struct runningProcess *)v1;
    const struct runningProcess *p2 = (struct runningProcess *)v2;
    const int p1i = p1->proc->cputime - p1->timeSpent;
    const int p2i = p2->proc->cputime - p2->timeSpent;
    if (p1->proc->pid < p2->proc->pid)
        return +1;
    else if (p1->proc->pid > p2->proc->pid)
        return -1;
    else if (p1i < p2i)
        return +1;
    else if (p1i > p2i)
        return -1;
    return 0;
}
//  Comparison functions for the PROCESS CREATION quicksort
int compare2(const void *v1, const void *v2) {
    const struct runningProcess *p1 = (struct runningProcess *)v1;
    const struct runningProcess *p2 = (struct runningProcess *)v2;
    const int p1i = p1->age;
    const int p2i = p2->age;
    if (p1i < p2i)
        return +1;
    else if (p1i > p2i)
        return -1;
    else if (p1->proc->pid > p2->proc->pid)
        return +1;
    else if (p1->proc->pid < p2->proc->pid)
        return -1;
    return 0;
}

// MAIN

int main() {
    // # of basic processes that exist
    unsigned int processNumber;
    struct process *processList;

    // Variable list ready queue and its dynamic length
    struct runningProcess *readyQueue;
    int readyQueueLength = 0;

    //  reference to currently running process
    struct runningProcess *running = NULL;

    // variables to hold perviously running process's stuff
    struct process *prevProc = NULL;
    int prevtid = -1;
    unsigned int prevTimeSpent;

    //Time and Aggregate results
    unsigned int maxTime;
    unsigned long currentTime = 0;

    unsigned int waittimeTotal = 0;
    unsigned int processesCreated = 0;

    int *periodArray; // Array for storing periods to calculate lcm
    int creations = 0; // boolean

    //User input for processes (period time and cpu time)
    printf("Enter the number of processes to schedule: ");
    scanf("%u", &processNumber);
    processList = (struct process*)malloc(processNumber * sizeof(struct process));
    readyQueue = malloc(40* sizeof(struct runningProcess));
    periodArray = (int *)malloc(processNumber * sizeof(int));
    
    for (int i = 0; i < processNumber; i++) {
        printf("Enter the CPU time of process %u: ", i+1);
        scanf("%u", &processList[i].cputime);

        printf("Enter the period of process %u: ", i+1);
        scanf("%i", &processList[i].period);

        processList[i].pid = i+1;
        periodArray[i] = processList[i].period;
    }

    //Get maxt time to end loop
    maxTime = computeLCM(periodArray, processNumber);

    while(currentTime < maxTime) { // MAIN LOOP, time quanta of 1 ms.
        
        //  Print missed deadlines
        {
            int rtidtemp = 0; 
            if (running != NULL)
                rtidtemp = running->tid;
            qsort(readyQueue, readyQueueLength, sizeof(struct runningProcess), compare);
            for(int i=readyQueueLength-1;i>=0;i--) { //creating
                if(running != NULL && readyQueue[i].tid == rtidtemp) running = &readyQueue[i];
                if(readyQueue[i].timeLeft == 0) {
                    readyQueue[i].timeLeft = readyQueue[i].proc->period;
                    printf("%lu: process %u missed deadline (%u ms left), new deadline is %lu\n", currentTime, readyQueue[i].proc->pid, readyQueue[i].proc->cputime - readyQueue[i].timeSpent, currentTime+readyQueue[i].proc->period);
                }
            }
        }

        //  Add process to ready queue at beginning of period
        for(int i=0;i<processNumber;i++) {
            if(currentTime % processList[i].period == 0) {
                processesCreated++;
                addQ(readyQueue, &readyQueueLength, &processList[i]);
                creations = 1;
            }
        }

        //  If a process has been created, then 
        //  Print ready queue processes 
        if(creations > 0) {
            readyQueue = readyQueue;
            int rtidtemp = 0; 
            if (running != NULL)
                rtidtemp = running->tid;
            qsort(readyQueue, readyQueueLength, sizeof(struct runningProcess), compare2);
            printf("%lu: processes (oldest first):", currentTime);
            creations = 0;
            for(int i=0; i<readyQueueLength; i++) {
                if(running != NULL && readyQueue[i].tid == rtidtemp) running = &readyQueue[i];
                printf(" %u (%u ms)", readyQueue[i].proc->pid, readyQueue[i].proc->cputime - readyQueue[i].timeSpent);
            }
            printf("\n");

        }

        //  Main scheduling algo
        unsigned int lowest = 0xffffffff;
        for (int i = 0; i < readyQueueLength; i++) {
            readyQueue[i].timeLeft--;
            readyQueue[i].age++;
            if(readyQueue[i].timeLeft == lowest) {
                if(readyQueue[i].age == running->age) {
                    if(readyQueue[i].proc->pid < running->proc->pid) {
                        lowest = readyQueue[i].timeLeft;
                        running = &readyQueue[i];
                    }
                }
                if(readyQueue[i].age > running->age) {
                    lowest = readyQueue[i].timeLeft;
                    running = &readyQueue[i];
                }
            }
            if(readyQueue[i].timeLeft < lowest) {
                lowest = readyQueue[i].timeLeft;
                running = &readyQueue[i];
            }
        }
        
        //  Print when a process has been preempted and/or started
        if(prevProc != NULL && running!= NULL && running->tid != prevtid && prevTimeSpent > 0 && prevTimeSpent < prevProc->cputime)
            printf("%lu: process %u preempted!\n", currentTime, prevProc->pid);
        if(running != NULL && prevtid != running->tid)
            printf("%lu: process %u starts\n", currentTime, running->proc->pid);
        
        //  Add 1 ms to the running process, and assign prev
        if(running != NULL) {
            running->timeSpent++;
            
            if(running == NULL) prevProc = NULL;
            else prevProc = running->proc;
            prevtid = running->tid;
            prevTimeSpent = running->timeSpent;
        }

        //Calculate wait times
        for(i = 0; i < readyQueueLength; i++)
            if(&readyQueue[i] != running) waittimeTotal++;

        //  Time advance
        currentTime++;

        //  Remove a process from ready queue of it has completed
        if(running != NULL && running->timeSpent >= running->proc->cputime) {
            printf("%lu: process %u ends\n", currentTime, running->proc->pid);
            removeQ(readyQueue, &readyQueueLength, running);
            running = NULL;
        }
       
    }

    //  Aggregate results
    printf("%lu: Max Time reached\n", currentTime);
    printf("Sum of all waiting times: %u\n", waittimeTotal);
    printf("Number of processes created: %u\n", processesCreated);
    double avg = (double)waittimeTotal / (double)processesCreated;
    printf("Average Waiting Time: %.2lf\n", avg);
    
    // Mem
    free(processList);
    free(readyQueue);
    free(periodArray);

    return 0;
}