#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <unistd.h>

#define NUM_PHILOSOPHERS 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (id + 4) % NUM_PHILOSOPHERS
#define RIGHT (id + 1) % NUM_PHILOSOPHERS

int state[NUM_PHILOSOPHERS];
int phil[NUM_PHILOSOPHERS] = {0, 1, 2, 3, 4};

sem_t mutex;
sem_t forks[NUM_PHILOSOPHERS];

void print_status() 
{
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
    {
        if (state[i] == THINKING) 
        {
            printf("P%d THINKING   ", i);
        } 
        else if (state[i] == HUNGRY) 
        {
            printf("P%d HUNGRY     ", i);
        } 
        else if (state[i] == EATING) 
        {
            printf("P%d EATING     ", i);
        }
    }
    printf("\n");
    sleep(2);  
}

void eat(int id) 
{
    if (state[id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING)
     {
        state[id] = EATING;
        print_status();
        sem_post(&forks[id]);  
    }
}

void pick_up_fork(int id) 
{
    sem_wait(&mutex); 

    state[id] = HUNGRY;
    print_status();
    eat(id);  

    sem_post(&mutex); 
    sem_wait(&forks[id]);  
}

void put_down_fork(int id) 
{
    sem_wait(&mutex);  

    state[id] = THINKING;
    print_status();

    eat(LEFT);  
    eat(RIGHT); 

    sem_post(&mutex);  
}

void* philosopher(void* num) 
{
    while (1) 
    {
        int id = *(int*)num;

        sleep(1);  

        pick_up_fork(id);  

        sleep(1);  
        put_down_fork(id);  
    }
}

int main() 
{
    pthread_t philosophers[NUM_PHILOSOPHERS];

    sem_init(&mutex, 0, 1);  
    for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
    {
        sem_init(&forks[i], 0, 0);  
    }

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
    {
        pthread_create(&philosophers[i], NULL, philosopher, &phil[i]);
        printf("P%d THINKING   ", i);
    }
    printf("\n");

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
    {
        pthread_join(philosophers[i], NULL); 
    }

    return 0;
}
