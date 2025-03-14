#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
    int pid, arrivalTime, burstTime, priority;
    int remainingTime, completionTime, waitingTime, turnaroundTime;
} Process;

typedef struct {
    int id, size, allocatedProcess;
    bool allocated;
} MemoryBlock;

// Function to read processes from file
int readProcesses(const char* filename, Process p[]) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error opening file.\n");
        exit(1);
    }
    int i = 0;
    while (fscanf(file, "%d %d %d %d", &p[i].pid, &p[i].arrivalTime, &p[i].burstTime, &p[i].priority) != EOF) {
        p[i].remainingTime = p[i].burstTime;
        i++;
    }
    fclose(file);
    return i; // return number of processes
}

// Function for FCFS Scheduling
void fcfs(Process p[], int n) {
    printf("\n--- FCFS Scheduling ---\n");
    int time = 0;
    float totalWT = 0, totalTAT = 0;

    printf("\nGantt Chart:\n|");

    for (int i = 0; i < n; i++) {
        if (time < p[i].arrivalTime) time = p[i].arrivalTime;
        p[i].waitingTime = time - p[i].arrivalTime;
        time += p[i].burstTime;
        p[i].turnaroundTime = p[i].waitingTime + p[i].burstTime;
        p[i].completionTime = time;
        printf(" P%d |", p[i].pid);
    }
    printf("\n0");
    for (int i = 0; i < n; i++) printf("  %d", p[i].completionTime);
    printf("\n");

    printf("\nPID\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        totalWT += p[i].waitingTime;
        totalTAT += p[i].turnaroundTime;
        printf("%d\t%d\t%d\n", p[i].pid, p[i].waitingTime, p[i].turnaroundTime);
    }
    printf("Average WT: %.2f\n", totalWT / n);
    printf("Average TAT: %.2f\n", totalTAT / n);
}

// Function for Round Robin Scheduling
void roundRobin(Process p[], int n, int tq) {
    printf("\n--- Round Robin Scheduling ---\n");
    int time = 0, done = 0;
    float totalWT = 0, totalTAT = 0;

    printf("\nGantt Chart:\n|");

    while (done < n) {
        int allIdle = 1;
        for (int i = 0; i < n; i++) {
            if (p[i].remainingTime > 0 && p[i].arrivalTime <= time) {
                allIdle = 0;
                printf(" P%d |", p[i].pid);
                int execTime = (p[i].remainingTime > tq) ? tq : p[i].remainingTime;
                time += execTime;
                p[i].remainingTime -= execTime;
                if (p[i].remainingTime == 0) {
                    p[i].turnaroundTime = time - p[i].arrivalTime;
                    p[i].waitingTime = p[i].turnaroundTime - p[i].burstTime;
                    done++;
                }
            }
        }
        if (allIdle) time++;
    }
    printf("\n");

    printf("\nPID\tWT\tTAT\n");
    for (int i = 0; i < n; i++) {
        totalWT += p[i].waitingTime;
        totalTAT += p[i].turnaroundTime;
        printf("%d\t%d\t%d\n", p[i].pid, p[i].waitingTime, p[i].turnaroundTime);
    }
    printf("Average WT: %.2f\n", totalWT / n);
    printf("Average TAT: %.2f\n", totalTAT / n);
}

// First-Fit Memory Allocation
void firstFit(MemoryBlock blocks[], int m, int processSizes[], int p) {
    printf("\n--- First-Fit Memory Allocation ---\n");
    for (int i = 0; i < p; i++) {
        bool allocated = false;
        for (int j = 0; j < m; j++) {
            if (!blocks[j].allocated && blocks[j].size >= processSizes[i]) {
                blocks[j].allocated = true;
                blocks[j].allocatedProcess = i + 1;
                allocated = true;
                break;
            }
        }
        if (!allocated) printf("Process %d of size %d cannot be allocated.\n", i + 1, processSizes[i]);
    }
    printf("\nMemory Block Allocation:\n");
    for (int j = 0; j < m; j++) {
        printf("Block %d (%d): ", blocks[j].id, blocks[j].size);
        if (blocks[j].allocated) printf("Process %d\n", blocks[j].allocatedProcess);
        else printf("Free\n");
    }
}

// Best-Fit Memory Allocation
void bestFit(MemoryBlock blocks[], int m, int processSizes[], int p) {
    printf("\n--- Best-Fit Memory Allocation ---\n");
    for (int i = 0; i < p; i++) {
        int bestIdx = -1, minSize = 99999;
        for (int j = 0; j < m; j++) {
            if (!blocks[j].allocated && blocks[j].size >= processSizes[i] && blocks[j].size < minSize) {
                bestIdx = j;
                minSize = blocks[j].size;
            }
        }
        if (bestIdx != -1) {
            blocks[bestIdx].allocated = true;
            blocks[bestIdx].allocatedProcess = i + 1;
        } else printf("Process %d of size %d cannot be allocated.\n", i + 1, processSizes[i]);
    }
    printf("\nMemory Block Allocation:\n");
    for (int j = 0; j < m; j++) {
        printf("Block %d (%d): ", blocks[j].id, blocks[j].size);
        if (blocks[j].allocated) printf("Process %d\n", blocks[j].allocatedProcess);
        else printf("Free\n");
    }
}

int main() {
    Process processes[100];
    int n = readProcesses("processes.txt", processes);
    int choice, tq;
    printf("Select Scheduling Algorithm:\n1. FCFS\n2. Round Robin\n3. First-Fit Memory\n4. Best-Fit Memory\nChoice: ");
    scanf("%d", &choice);

    if (choice == 1) {
        fcfs(processes, n);
    } else if (choice == 2) {
        printf("Enter Time Quantum: ");
        scanf("%d", &tq);
        roundRobin(processes, n, tq);
    } else if (choice == 3 || choice == 4) {
        int processSizes[] = {212, 417, 112, 426};
        MemoryBlock blocks[] = {{1, 100, 0, false}, {2, 500, 0, false}, {3, 200, 0, false}, {4, 300, 0, false}, {5, 600, 0, false}};
        int m = 5, p = 4;
        if (choice == 3) firstFit(blocks, m, processSizes, p);
        else bestFit(blocks, m, processSizes, p);
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}
