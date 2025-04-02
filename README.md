__1. Shell Script:__ I tried to check my basic understanding of shell scripts here.  

__2. xv6 System Call:__ I implemented a new system call in xv6, which helped me understand its functionality and provided insight into the internal workings of the xv6 kernel. 

__3. xv6 MLFQ Scheduler:__ I implemented a new scheduler for the xv6 operating system based on the Multilevel Feedback Queue (MLFQ) algorithm. The scheduler used multiple queues with different scheduling algorithms and time slice limits. There are two scheduling algorithm used, lottery scheduling and round robin.New processes were added to the tail of the topmost queue, and processes were scheduled starting from the highest-priority queue. Processes either voluntarily relinquished the CPU or were preempted and moved to lower-level queues. 

__4. Inter Process Communication:__ This task extends the xv6 operating system to implement Inter-Process Communication (IPC) mechanisms for process synchronization and coordination.  

__Implemented Features:__  
- **Synchronization & Mutual Exclusion** – Ensures safe access to shared resources.  
- **Semaphore-based Mechanism** – Inspired by the **dining philosopher's problem** to prevent deadlocks.  
- **Blocking Mechanism** – Enables processes to wait efficiently without busy-waiting.  
- **Process Coordination** – Manages resource allocation among multiple concurrent processes.  

__5. xv6 Threading and Synchronization:__ It extends the xv6 operating system to support user-level threading and POSIX-like synchronization primitives.  

__Implemented Features:__  

- **User-Level Threading**: Added system calls to support thread creation, joining, and termination while sharing the same address space.  
- **Thread Synchronization**: Implemented spinlocks and mutex locks to ensure safe access to shared resources.  
- **Condition Variables & Semaphores**: Introduced synchronization primitives for proper thread coordination in concurrent scenarios.  
- **Kernel Modifications**: Extended process structures to differentiate between threads and processes.  

