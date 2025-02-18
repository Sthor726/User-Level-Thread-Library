#include "TCB.h"


// private:
// 	int _tid;               // The thread id number.
// 	Priority _pr;           // The priority of the thread (Red, orange or green)
// 	int _quantum;           // The time interval, as explained in the pdf.
// 	State _state;           // The state of the thread
// 	char* _stack;           // The thread's stack


TCB::TCB(int tid, void *(*start_routine)(void *arg), void *arg, State state)
{
    _tid = tid;
    _start_routine = start_routine;
    _arg = arg;
    _state = state;
    _quantum = 0;
    _stack = new char[STACK_SIZE];


}

TCB::~TCB()
{
    delete _stack;
}

void TCB::setState(State state)
{
    _state = state;
}

State TCB::getState() const
{
    return _state;

}

int TCB::getId() const
{
    return _tid;
}

void TCB::increaseQuantum()
{
    //TODO
}

int TCB::getQuantum() const
{
    return _quantum;
}

// int TCB::saveContext()
// {
// }

// void TCB::loadContext()
// {
// }
