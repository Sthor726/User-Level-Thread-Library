#define _XOPEN_SOURCE 700
#include "uthread.h"
#include "TCB.h"
#include <cassert>
#include <deque>
#include <ucontext.h>
#include <iostream>
#include <cstdlib>


using namespace std;

// Finished queue entry type
typedef struct finished_queue_entry
{
	TCB *tcb;	  // Pointer to TCB
	void *result; // Pointer to thread result (output)
} finished_queue_entry_t;

// Join queue entry type
typedef struct join_queue_entry
{
	TCB *tcb;			 // Pointer to TCB
	int waiting_for_tid; // TID this thread is waiting on
} join_queue_entry_t;

typedef struct runningTCB {
	int tid;
	TCB *tcb;
} runningTCB;

// You will need to maintain structures to track the state of threads
// - uthread library functions refer to threads by their TID so you will want
//   to be able to access a TCB given a thread ID
// - Threads move between different states in their lifetime (READY, BLOCK,
//   FINISH). You will want to maintain separate "queues" (doesn't have to
//   be that data structure) to move TCBs between different thread queues.
//   Starter code for a ready queue is provided to you
// - Separate join and finished "queues" can also help when supporting joining.
//   Example join and finished queue entry types are provided above

// Queues
static deque<TCB *> ready_queue;
static deque<join_queue_entry *> join_queue;
static deque<finished_queue_entry_t> finished_queue;
static runningTCB *currentTCB;


// Interrupt Management --------------------------------------------------------
struct itimerval timer;
sigset_t mask;
sigset_t empty_mask;
void handler(int signum){
	uthread_yield();
}

// Start a countdown timer to fire an interrupt
static void startInterruptTimer()
{
	setitimer(ITIMER_VIRTUAL, &timer, NULL);
}

// Block signals from firing timer interrupt
static void disableInterrupts()
{
	sigprocmask(SIG_BLOCK, &mask, nullptr);
}

// Unblock signals to re-enable timer interrupt
static void enableInterrupts()
{
	sigprocmask(SIG_SETMASK, &empty_mask, nullptr);
}

// Queue Management ------------------------------------------------------------

// Add TCB to the back of the ready queue
void addToReadyQueue(TCB *tcb)
{
	ready_queue.push_back(tcb);
}

// Removes and returns the first TCB on the ready queue
// NOTE: Assumes at least one thread on the ready queue
TCB *popFromReadyQueue()
{
	assert(!ready_queue.empty());

	TCB *ready_queue_head = ready_queue.front();
	ready_queue.pop_front();
	return ready_queue_head;
}

// Removes the thread specified by the TID provided from the ready queue
// Returns 0 on success, and -1 on failure (thread not in ready queue)
int removeFromReadyQueue(int tid)
{
	for (deque<TCB *>::iterator iter = ready_queue.begin(); iter != ready_queue.end(); ++iter)
	{
		if (tid == (*iter)->getId())
		{
			ready_queue.erase(iter);
			return 0;
		}
	}

	// Thread not found
	return -1;
}

// Helper functions ------------------------------------------------------------

// Switch to the next ready thread
static void switchThreads()
{
	static int currentThread = uthread_self();
	volatile int flag = 0;
	int ret_val = getcontext(&currentTCB->tcb->_context);

	cout << "SWITCH: currentThread = " << currentThread << endl;
	if (flag == 1) {
	return;
	}
	flag = 1;
	TCB* new_thread = popFromReadyQueue();
	if (new_thread == NULL){
		std::cout << "no thread available\n";
		return;
	}
	new_thread->setState(RUNNING);
	setcontext(&new_thread->_context);
	
}

// Library functions -----------------------------------------------------------

// The function comments provide an (incomplete) summary of what each library
// function must do

// Starting point for thread. Calls top-level thread function
void stub(void *(*start_routine)(void *), void *arg)
{
	// TODO
	start_routine(arg);
	uthread_exit(0);
}

int uthread_init(int quantum_usecs)
{
	// Inside of uthread_init you will want to create an instance of the
	// TCB class for your main thread. This thread should not create a
	// new context. How can you best achieve this? Overloaded constructor
	// that does not take a start routine? Pass in NULL for the start
	// routine when creating a TCB class
	// in uthread_init and skip the makecontext call for a null start routine?

	// Initialize any data structures
	std::deque<TCB *> ready_queue = {};
	std::deque<join_queue_entry *> join_queue = {};
	std::deque<finished_queue_entry *> finished_queue = {};

	// Setup timer interrupt and handler
	timer.it_interval.tv_usec = quantum_usecs;
	timer.it_value.tv_usec = quantum_usecs;
	signal(SIGALRM, handler);
	startInterruptTimer();

	sigemptyset(&mask);
	sigemptyset(&empty_mask);
	sigaddset(&mask, SIGALRM);

	// Create a thread for the caller (main) thread
	TCB *thread;
	currentTCB->tid = 1;
	currentTCB->tcb = thread;
	thread->setState(RUNNING);

	return 0;

}

///* Create a new thread whose entry point is f */
// Return new thread ID on success, -1 on failure
// int uthread_create(void *(*start_routine)(void *), void *arg);

int uthread_create(void *(*start_routine)(void *), void *arg)
{

	// Create a new thread and add it to the ready queue
	int current_tid = uthread_self();
	TCB *new_thread = new TCB(1 /*What should the TID be???*/,GREEN, *(void* (*)(void*)) stub, arg, READY);
	addToReadyQueue(new_thread);
	return new_thread->getId();
}

int uthread_join(int tid, void **retval)
{

	disableInterrupts();
	// If the thread specified by tid is already terminated, just return
	for(finished_queue_entry entry : finished_queue){
		if (entry.tcb->getId() == tid) return 0;
	}

	TCB* chosenTCB;
	chosenTCB = popFromReadyQueue();
	if (chosenTCB == NULL){
		//spinnnnnnn
	} else {
		// start a new thread and block this one 
	}
	
	// Set *retval to be the result of thread if retval != nullptr
	if(retval != nullptr){
		for(finished_queue_entry entry : finished_queue){
			if (entry.tcb->getId() == tid) *retval = entry.result;
		}
	}
	enableInterrupts();
}

int uthread_yield(void)
{
	finished_queue_entry finishedTCB;
	disableInterrupts();
	
	addToReadyQueue(currentTCB->tcb);
	currentTCB->tcb->setState(READY);
	switchThreads();
	
	while (!finished_queue.empty())
	{
		finishedTCB = finished_queue.front();
		finished_queue.pop_front();
		delete &finishedTCB;
	}
	enableInterrupts();
}

void uthread_exit(void *retval)
{
	// If this is the main thread, exit the program
	// Move any threads joined on this thread back to the ready queue
	// Move this thread to the finished queue
}

int uthread_suspend(int tid)
{
	// Move the thread specified by tid from whatever state it is
	// in to the block queue
	
}

int uthread_resume(int tid)
{
	// Move the thread specified by tid back to the ready queue
	for (TCB* tcb : ready_queue){
		if (tcb->getId() == tid){
			// Remove thread from the block queue?????
			ready_queue.push_back(tcb);
			tcb->setState(READY);
		}
	}
	
}

int uthread_once(uthread_once_t *once_control, void (*init_routine)(void))
{
	// Use the once_control structure to determine whether or not to execute
	// the init_routine
	// Pay attention to what needs to be accessed and modified in a critical region
	// (critical meaning interrupts disabled)
}

int uthread_self()
{
	// TODO
}

int uthread_get_total_quantums()
{
	// TODO
}

int uthread_get_quantums(int tid)
{
	// TODO
}
