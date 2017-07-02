/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   main.cpp
 * Author: nikita
 *
 * Created on 15 марта 2017 г., 22:11
 */

#include <cstdlib>
#include <chrono>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <mutex>
#include <unistd.h>
typedef std::chrono::system_clock Clock;
typedef std::chrono::milliseconds milliseconds;
int numberOfProcessors = 1;

#define OPERATIONS 2500000
#define REPEAT 5
bool REG  = false;
std::atomic<int> counter;

    void* cas(void* args) {
        {
            int value = counter.load();
            for(int i = 0; i < OPERATIONS; ++i)
            {
                do
                {}
                while(!counter.compare_exchange_weak(value,value+1));
            }
        }
        return 0;
    }

    void* ffa(void* args)
    {
        int value = counter.load();
        for(int i = 0; i < OPERATIONS; ++i)
        {
            !counter.fetch_add(1);
        }
    }
    
    
    void* llsc(void* args)
    {
        int value ;
        do
        {
            value = counter.load();
        }
        while(!counter.compare_exchange_weak(value,value+1));
    }
    
    void bench_cas(int Threads)
    {
        double time = 0;
        counter.store(0);
        if(REG)
        {
           
            pthread_t threads[numberOfProcessors];
            pthread_attr_t attr;
            cpu_set_t cpus;
            pthread_attr_init(&attr);
            Clock::time_point t0 = Clock::now();
            for (int i = 0; i < numberOfProcessors; i++)
            {
                CPU_ZERO(&cpus);
                CPU_SET(i, &cpus);
                pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
                pthread_create(&threads[i], &attr,cas, NULL);
            }
            Clock::time_point t1 = Clock::now();
            double ms = (std::chrono::duration_cast<milliseconds>(t1 - t0)).count();
            time += ms/(numberOfProcessors * OPERATIONS);
        }
        else
        {  
            for(int i = 0; i < REPEAT; ++i)
            {

                counter.store(0);

                std::vector<std::thread> threads;

                Clock::time_point t0 = Clock::now();

                for(int i = 0; i < Threads; ++i)
                {
                    threads.push_back(std::thread([&]()
                    {
                        int value = counter.load();
                    for(int i = 0; i < OPERATIONS; ++i)
                        {
                            do
                            {
                            }
                            while(!counter.compare_exchange_weak(value,value+1));
                            //std::cout << std::endl << value;
                        }
                    }));
                }

                for(auto& thread : threads)
                {
                    thread.join();
                }

                Clock::time_point t1 = Clock::now();

                double ms = (std::chrono::duration_cast<milliseconds>(t1 - t0)).count();
                time += ms/(Threads * OPERATIONS);
            }
        }
//        std::cout << "CAS with " << Threads << "threads  time = " <<
//                (time / REPEAT) << " counter is " << counter << std::endl;
        std::cout << " CAS " << Threads << " " << (time / REPEAT) << std::endl ;
    }

    
    void bench_ffa(int Threads)
    {
        double time = 0;
        
        std::atomic<int> counter;
       if(REG)
       {
           
            pthread_t threads[numberOfProcessors];
            pthread_attr_t attr;
            cpu_set_t cpus;
            pthread_attr_init(&attr);
            Clock::time_point t0 = Clock::now();
            for (int i = 0; i < numberOfProcessors; i++)
            {
                CPU_ZERO(&cpus);
                CPU_SET(i, &cpus);
                pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
                pthread_create(&threads[i], &attr, ffa , NULL);
            }
            Clock::time_point t1 = Clock::now();

            double ms = (std::chrono::duration_cast<milliseconds>(t1 - t0)).count();
            time += ms/(numberOfProcessors * OPERATIONS);
       }

      
        for(int i = 0; i < REPEAT; ++i)
        {
            
            counter.store(0);

            std::vector<std::thread> threads;

            Clock::time_point t0 = Clock::now();

            for(int i = 0; i < Threads; ++i)
            {
                threads.push_back(std::thread([&]()
                {
                for(int i = 0; i < OPERATIONS; ++i)
                    {
                        !counter.fetch_add(1);
                        //std::cout << std::endl << value;
                    }
                }));
            }

            for(auto& thread : threads)
            {
                thread.join();
            }

            Clock::time_point t1 = Clock::now();

            double ms = (std::chrono::duration_cast<milliseconds>(t1 - t0)).count();
            time += ms/(Threads * OPERATIONS);
        }
//        std::cout << "FFA with " << Threads << " threads time = " <<
//                (time / REPEAT) << " counter is " << counter.load() << std::endl ;
        std::cout << " FFA " << Threads << " " << (time / REPEAT) << std::endl ;
    }
  
    void bench_llsc(int Threads)
    {
        double time = 0;
        std::atomic<int> counter;
         if(REG)
       {
           
            pthread_t threads[numberOfProcessors];
            pthread_attr_t attr;
            cpu_set_t cpus;
            pthread_attr_init(&attr);
            Clock::time_point t0 = Clock::now();
            for (int i = 0; i < numberOfProcessors; i++)
            {
                CPU_ZERO(&cpus);
                CPU_SET(i, &cpus);
                pthread_attr_setaffinity_np(&attr, sizeof(cpu_set_t), &cpus);
                pthread_create(&threads[i], &attr,llsc , NULL);
            }
            Clock::time_point t1 = Clock::now();

            double ms = (std::chrono::duration_cast<milliseconds>(t1 - t0)).count();
            time += ms/(numberOfProcessors * OPERATIONS);
       }        
        for(int i = 0; i < REPEAT; ++i)
        {
            
            counter.store(0);

            std::vector<std::thread> threads;

            Clock::time_point t0 = Clock::now();

            for(int i = 0; i < Threads; ++i)
            {
                threads.push_back(std::thread([&]()
                {
                for(int i = 0; i < OPERATIONS; ++i)
                    {
                    int value ;
                        do
                        {
                            value = counter.load();
                        }
                        while(!counter.compare_exchange_weak(value,value+1));
                        //std::cout << std::endl << value;
                    }
                }));
            }

            for(auto& thread : threads)
            {
                thread.join();
            }

            Clock::time_point t1 = Clock::now();

            double ms = (std::chrono::duration_cast<milliseconds>(t1 - t0)).count();
            time += ms/(Threads * OPERATIONS);
        }
//        std::cout << "LLSC with " << Threads << " threads time = " <<
//                (time / REPEAT) << " counter is " << counter << std::endl ;
        std::cout << " LLSC " << Threads << " " << (time / REPEAT) << std::endl ;
    }
    
    int main(int argc, char** argv) 
    {
        if(REG)
        {
            numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);
            printf("Number of processors: %d\n", numberOfProcessors);
        }
        for(int i =1; i < 8; i++)
        {
            bench_cas(i);
            bench_ffa(i);
            bench_llsc(i);
        }
        return 0;
    }



