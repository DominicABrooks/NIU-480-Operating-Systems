/********************************************************************
CSCI 480 - Assignment 4 - Semester Fall 2023
Programmer: Dominic Brooks
Section: 0001
TA: Sai Dinesh Reddy Bandi, Ajay Kuma Reddy Kandula, Yuva Krishna Thanneru
Date Due: 9/27/23
Purpose: Solve the Reader-Writer problem using the PThreads library.
*********************************************************************/
#include <time.h>
#include <string>
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#include <semaphore.h>
#include <bits/stdc++.h>

// Initialize shared string, which is a global variable shared by all threads
char sharedString[] = "All work and no play makes Jack a dull boy.";

// rw_sem is used by both readers and writers
// cs_sem is used for protecting critical sections of readers
sem_t rw_sem, cs_sem;
int read_count = 0;  // Counter to track the number of readers.

/**
 * Simulates a reader thread that reads from a shared resource.
 *
 * @param param A pointer to the reader's ID.
 */
void *reader(void *param) {
    int tid = *((int *)param); // Reader ID

    while (strlen(sharedString) > 0) {
        // Request permission to read
        sem_wait(&cs_sem);

        // Increment the read_count
        read_count++;
        printf("read_count increments to: %d.\n", read_count);

        // If it's the first reader, block writers
        if (read_count == 1) {
            sem_wait(&rw_sem);
        }

        // Release permission to read
        sem_post(&cs_sem);

        // Read operation
        printf("reader %d is reading ... content : %s\n", tid, sharedString);

        // Request permission to read
        sem_wait(&cs_sem);

        // Decrement the read_count
        read_count--;
        printf("read_count decrements to: %d.\n", read_count);

        // If it's the last reader, allow writers
        if (read_count == 0) {
            sem_post(&rw_sem);
        }

        // Release permission to read
        sem_post(&cs_sem);

        // Sleep for a short period to simulate work
        sleep(1);
    }
    printf("reader %d is exiting ...\n", tid);
    pthread_exit(NULL);
}

/**
 * Simulates a writer thread that writes to a shared resource.
 *
 * @param param A pointer to the writer's ID.
 */
void *writer(void *param) {
    int tid = *((int *)param);
    while (strlen(sharedString) > 0) {
        // Wait for the writer to have exclusive access
        sem_wait(&rw_sem);

        // Write operation
        if (strlen(sharedString) > 0) {
            sharedString[strlen(sharedString) - 1] = '\0';
        }
        printf("writer %d is writing ...\n", tid);

        // Release the writer's exclusive access
        sem_post(&rw_sem);

        // Check if the shared string is empty, and if so, exit
        if (strlen(sharedString) == 0) {
            printf("writer %d is exiting ...\n", tid);
            break;  // Exit the loop
        }

        // Sleep for a short period to simulate work
        sleep(1);
    }
    pthread_exit(NULL);
}

/**
 * Entry point of the program. Simulates the reader-writer problem
 * with multiple reader and writer threads.
 *
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line argument strings.
 * @return Exit status of the program.
 */
int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Invalid Arguments\n");
        return 1;
    }

    int NUM_READERS = atoi(argv[1]);  // Get the number of reader threads from command line.
    int NUM_WRITERS = atoi(argv[2]);  // Get the number of writer threads from command line.

    if (NUM_READERS < 1 || NUM_WRITERS < 1) {
        fprintf(stderr, "Both the number of readers and writers must be greater than or equal to 1.\n");
        return 1;
    }

    printf("*** Reader-Writer Problem Simulation ***\nNumber of reader threads: %d\nNumber of writer threads: %d\n", NUM_READERS, NUM_WRITERS);

    pthread_t* readerThreads = new pthread_t[NUM_READERS];  // Array of reader thread IDs.
    pthread_t* writerThreads = new pthread_t[NUM_WRITERS];  // Array of writer thread IDs.

    int* readerThreadIDs = new int[NUM_READERS];  // Array to store reader thread IDs.
    int* writerThreadIDs = new int[NUM_WRITERS];  // Array to store writer thread IDs.

    // Initialize semaphores
    sem_init(&rw_sem, 0, 1);  // Initialize the read-write semaphore with initial value 1.
    sem_init(&cs_sem, 0, 1);  // Initialize the critical section semaphore with initial value 1.

    int i;

    // Create reader threads
    for (i = 0; i < NUM_READERS; i++) {
        readerThreadIDs[i] = i;
        pthread_create(&readerThreads[i], NULL, reader, &readerThreadIDs[i]);
    }

    // Create writer threads
    for (i = 0; i < NUM_WRITERS; i++) {
        writerThreadIDs[i] = i;
        pthread_create(&writerThreads[i], NULL, writer, &writerThreadIDs[i]);
    }

    // Main thread will wait for reader and writer threads to finish
    for (i = 0; i < NUM_READERS; i++) {
        pthread_join(readerThreads[i], NULL);
    }

    // Wait for each writer thread to complete its execution
    for (i = 0; i < NUM_WRITERS; i++) {
        pthread_join(writerThreads[i], NULL);
    }

    // Cleanup and exit
    sem_destroy(&rw_sem);  // Clean up the read-write semaphore.
    sem_destroy(&cs_sem);  // Clean up the critical section semaphore.

    delete[] readerThreads;   // Clean up the reader thread IDs array.
    delete[] writerThreads;   // Clean up the writer thread IDs array.
    delete[] readerThreadIDs; // Clean up the reader thread ID array.
    delete[] writerThreadIDs; // Clean up the writer thread ID array.

    printf("All threads are done.\nResources cleaned up.\n");

    pthread_exit(NULL);
}