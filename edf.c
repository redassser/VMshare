#include <stdio.h>
#include <stdlib.h>
struct process {
    unsigned short pid;
    unsigned int cputime;
    unsigned int period;

    unsigned int isReady;
    unsigned int timeSpent;
    unsigned int timeLeft;

    unsigned int waittimeTotal;
    unsigned int processesCreated;
};

unsigned int gcd2(unsigned int a, unsigned int b) {
    if (a == 0)
        return b;
    return gcd2(b % a, a);
}
unsigned int lcm2(unsigned int a, unsigned int b) {
    return a * b / gcd2(a, b);
}
unsigned int computeLCM(unsigned int *periodArray, unsigned int len) {
    unsigned int a = periodArray[0];
    unsigned int b;
    for (unsigned int i = 1; i < len; i++) {
        b = periodArray[i];
        a = lcm2(a, b);
    }
    return a;
}

int main(int argc, char *argv[]) {
    unsigned int processNumber;
    struct process *processArray;
    int creations = 0;
    unsigned int prevRunningID = 0;
    unsigned int runningID;
    unsigned int *periodArray;
    unsigned int **timeLeftArray;
    unsigned int **readyArray;
    unsigned int maxTime;
    unsigned long currentTime = 0;

    printf("Enter the number of processes to schedule: ");
    scanf("%u", &processNumber);
    processArray  = (struct process*)malloc(processNumber * sizeof(struct process));
    periodArray   = (unsigned int *)malloc(processNumber * sizeof(unsigned int));
    timeLeftArray = (unsigned int **)malloc(processNumber * sizeof(unsigned int*));
    readyArray    = (unsigned int **)malloc(processNumber * sizeof(unsigned int*));

    for (unsigned int i = 0; i < processNumber; i++) {
        printf("Enter the CPU time of process %u: ", i+1);
        scanf("%u", &processArray[i].cputime);

        printf("Enter the period of process %u: ", i+1);
        scanf("%u", &processArray[i].period);

        processArray[i].pid = i+1;
        periodArray[i] = processArray[i].period;
        timeLeftArray[i] = &processArray[i].timeLeft;
        readyArray[i] = &processArray[i].isReady;
    }
    maxTime = computeLCM(periodArray, processNumber);
    printf("maxtime: %u\n", maxTime);

    while(currentTime < maxTime) {
        if(currentTime == 8)
            printf("%u, %u, %u\n", *timeLeftArray[0] * *readyArray[0], *timeLeftArray[1] * *readyArray[1], *timeLeftArray[2] * *readyArray[2]);
        if(processArray[runningID].timeSpent == processArray[runningID].cputime) { //ends
            printf("%lu: process %u ends\n", currentTime, processArray[runningID].pid);

            processArray[runningID].timeSpent = 0;
        }
        
        for(int i=0;i<processNumber;i++) {
            if(currentTime % processArray[i].period == 0) {
                if(*readyArray[i] && currentTime!=0) {
                    printf("%lu: process %u missed deadline (%u ms left), new deadline is %lu\n", currentTime, processArray[i].pid, processArray[i].cputime - processArray[i].timeSpent, currentTime+periodArray[i]);
                    processArray[i].timeSpent = 0;
                }
                creations = 1;
                processArray[i].isReady = 1;
                processArray[i].timeLeft = processArray[i].period;
            }
        }
        
        if(creations == 1) {
            printf("%lu: processes (oldest not first):", currentTime);
            creations = 0;
            for(int i=0; i<processNumber; i++) {
                if(processArray[i].isReady)
                    printf(" %u (%u ms)", processArray[i].pid, processArray[i].cputime - processArray[i].timeSpent);
            }
            printf("\n");
        }

        unsigned int lowest = 0xffffffff;
        for (int i = 0; i < processNumber; i++) {
            printf("step %u\n", i);
            processArray[i].timeLeft = processArray[i].timeLeft = processArray[i].period - currentTime % processArray[i].period;
            if(*timeLeftArray[i] * *readyArray[i] == 0) {
                continue;
            }
            printf("tl %u, %u\n", *timeLeftArray[i], lowest);
            if(*timeLeftArray[i] == lowest) {
                printf("ages %u, %u\n", periodArray[i]-*timeLeftArray[i],periodArray[runningID]-*timeLeftArray[runningID]);
                if(periodArray[i]-*timeLeftArray[i] == periodArray[runningID]-*timeLeftArray[runningID]) {
                    if (i < runningID) runningID = i;
                } else if(periodArray[i]-*timeLeftArray[i] > periodArray[runningID]-*timeLeftArray[runningID]) {
                    printf("here2 %u\n", runningID);
                    runningID = i;
                    
                }
            } else if(*timeLeftArray[i] < lowest) {
                lowest = *timeLeftArray[i];
                printf("here3 %u, %u\n", runningID, i);
                runningID = i;
            }  
        }
        if(prevRunningID != runningID && *readyArray[prevRunningID]) {
            printf("%lu: process %u preempted!\n", currentTime, processArray[prevRunningID].pid);
        }
        if(prevRunningID != runningID && *readyArray[runningID]) {
            printf("%lu: process %u starts\n", currentTime, processArray[runningID].pid);
        } else if(processArray[runningID].timeSpent == 0 && processArray[runningID].isReady)
            printf("%lu: process %u starts\n", currentTime, processArray[runningID].pid);
        prevRunningID = runningID;

        if(processArray[runningID].isReady) {
            processArray[runningID].timeSpent++;
        }
        currentTime++;
    }
    printf("%lu: Max Time reached", currentTime);
    return 0;
}