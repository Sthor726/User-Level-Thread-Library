#include "TCB.h"

TCB::TCB(int tid, void *(*start_routine)(void *arg), void *arg, State state)
{
    _tid = tid;
    _state = state;
}

TCB::~TCB()
{
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
