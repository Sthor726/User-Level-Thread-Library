# User-Level Thread Library (uthread)

## Overview
This project implements a user-level thread (ULT) library called `uthread`, which mimics the interface of the `pthread` library and runs in user space. The purpose of this project is to deepen understanding of thread mechanisms in operating systems and explore design trade-offs.

## Features
- Implements user-level threading without OS thread management.
- Supports cooperative and preemptive multitasking.
- Provides thread suspension, resumption, and joining mechanisms.
- Implements a round-robin scheduler with time slicing.

## uthread API
### pthread Equivalents
- `int uthread_create(void *(*start_routine)(void *), void *arg);`
- `int uthread_yield(void);`
- `int uthread_self(void);`
- `int uthread_join(int tid, void **retval);`

### uthread Control
- `int uthread_init(int time_slice);`
- `int uthread_exit(void *retval);`
- `int uthread_suspend(int tid);`
- `int uthread_resume(int tid);`
- `int uthread_once(uthread_once_t *once_control, void (*init_routine)(void));`

## Implementation Details
### Context Switching
- Uses `getcontext`, `setcontext`, and `makecontext` for context management.
- Each thread maintains a Thread Control Block (TCB) containing:
  - Thread ID (tid)
  - Stack pointer (sp)
  - Saved context (`ucontext_t`)
  - Scheduling quantum count
  - Thread state (READY, RUNNING, BLOCK, FINISHED)
- Threads switch execution via `uthread_yield` and preemptive time slicing.

### Scheduler
- Implements a round-robin scheduler with configurable time slicing.
- Uses `setitimer` with `ITIMER_VIRTUAL` for time slicing.
- Disables timer interrupts when modifying critical data structures.

### Thread Synchronization
- Implements `uthread_join` to allow a thread to wait for another thread’s termination.
- Maintains join and finished queues to manage synchronization.
- Implements `uthread_once` to ensure an initialization routine executes only once.

## Compilation and Execution
### Compilation
To compile the project, run:
```sh
make
```

### Running Tests
To execute the test cases:
```sh
./uthread_test
```

### Cleaning
To clean compiled files:
```sh
make clean
```

## Design Assumptions and Choices
- Threads are cooperatively scheduled unless preemption is enabled.
- Each thread requires its own stack for independent execution.
- The scheduler is simple round-robin but could be extended with priority scheduling.

## Performance Considerations
- The choice of time slice length affects performance.
- Critical sections are protected against interruptions using `sigprocmask`.

## Deliverables
- `writeup.pdf`: Detailed design choices and API documentation.
- Source code with a `Makefile`.
- Test cases to verify functionality.

## Grading Breakdown
- **10 points**: Intermediate submission (on-time, compiles, meets requirements)
- **25 points**: Scheduler, context switching, time slicing
- **40 points**: Correct implementation of `uthread` API
- **25 points**: Documentation and answering questions

## Intermediate Submission Requirements
- Implement `uthread_init`, `uthread_create`, and `uthread_yield`.
- Setup and manage timer interrupts.
- Ensure basic thread creation and yielding works.

## Additional Enhancements (Optional)
- Implement priority-based scheduling.
- Optimize context switching for efficiency.
- Extend with advanced debugging tools.

## Submission
Submit the project as a compressed tarball:
```sh
tar -cvzf submission.tar.gz project_folder
```
Ensure no compiled executables or unnecessary files are included.

