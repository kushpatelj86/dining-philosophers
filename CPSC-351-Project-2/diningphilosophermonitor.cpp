#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <chrono>

using namespace std;

#define NUM_PHILOSOPHERS 5
#define THINKING 2
#define HUNGRY 1
#define EATING 0
#define LEFT (id + 4) % NUM_PHILOSOPHERS
#define RIGHT (id + 1) % NUM_PHILOSOPHERS

class Monitor
{
    private:
        int state[NUM_PHILOSOPHERS];
        condition_variable conditions[NUM_PHILOSOPHERS];
        mutex mtx;

        void print_status() 
        {
            for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
            {
                if (state[i] == THINKING) 
                {
                    cout << "P" << i << " THINKING   ";
                } 
                else if (state[i] == HUNGRY) 
                {
                    cout << "P" << i << " HUNGRY     ";
                } 
                else if (state[i] == EATING) 
                {
                    cout << "P" << i << " EATING     ";
                }
            }
            cout << endl;
        }

        void try_to_eat(int id) 
        {
            if (state[id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) 
            {
                state[id] = EATING;
                this_thread::sleep_for(std::chrono::milliseconds(2000));
                print_status();
                conditions[id].notify_one();
            }
        }

public:
    Monitor() 
    {
        for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
        {
            state[i] = THINKING;
        }
    }

    void pick_up_fork(int id)
    {
        unique_lock<mutex> lock(mtx);

        state[id] = HUNGRY;
        print_status();
        try_to_eat(id);

        while (state[id] != EATING) 
        {
            conditions[id].wait(lock);
        }
        this_thread::sleep_for(std::chrono::milliseconds(1000));
    }

    void put_down_fork(int id) 
    {
        unique_lock<mutex> lock(mtx);

        state[id] = THINKING;
        print_status();

        try_to_eat(LEFT);
        try_to_eat(RIGHT);
    }
};

void philosopher(Monitor& monitor, int id) 
{
    while (true) 
    {
        this_thread::sleep_for(chrono::seconds(1)); 
        monitor.pick_up_fork(id);

        this_thread::sleep_for(chrono::seconds(1)); 
        monitor.put_down_fork(id);
    }
}

int main() {
    Monitor monitor;
    thread philosophers[NUM_PHILOSOPHERS];

    for (int i = 0; i < NUM_PHILOSOPHERS; i++) 
    {
        philosophers[i] = thread(philosopher, ref(monitor), i);
        cout << "P" << i << " THINKING   ";
    }
    cout << endl;

    for (int i = 0; i < NUM_PHILOSOPHERS; i++)
    {
        philosophers[i].join();
    }

    return 0;
}
