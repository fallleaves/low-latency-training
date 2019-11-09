#include <thread>
#include <iostream>

volatile int X = 0;
volatile int Y = 0;
volatile int r1 = 0;
volatile int r2 = 0;

void thread1()
{

    X = 1;
    r1 = Y;
}

void thread2()
{
    Y = 1;
    r2 = X;
}

void reset()
{
    X = 0;
    Y = 0;
    r1 = 0;
    r2 = 0;
}

int main()
{
    int iterations = 0;

    while (true)
    {
        reset();
        std::thread t1{thread1};
        std::thread t2{thread2};

        t1.join();
        t2.join();

        ++iterations;

        if (r1 == 0 && r2 == 0)
        {
            std::cout << "Fires after " << iterations << " iterations" << std::endl;
        }
    }

    return 0;
}