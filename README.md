# process-manager
Process Manager (Pman) supports commands to run, stop, start, kill and list processes.

To compile code enter "make" in the terminal.
To run the executable enter ./pman in the terminal.

Pman is a Process Manager which supports the following commands
1. bg \<cmd\>: starts \<cmd\> in the background
2. bgstart \<pid\>: sends the CONT signal to process id \<pid\> (restarts the process)
3. bgstop \<pid\>: sends the STOP signal to process id <\pid\> (stops the process)
4. bgkill \<pid\>: sends the TERM signal to process id \<pid\> (terminates the process)
5. bglist: prints the list of processes managed by Pman
6. pstat \<pid\>: lists comm, state, utime, stime, rss, voluntary_ctxt_switches and nonvoluntary_ctxt_switches for process id /<pid/>
7. q: quits Pman
  

  
  
inf.c is a program used to test Pman. It will continuously print a specified string at a specified interval. This is useful as it is a process that can be easily identified as running or not.
to call inf.c enter in the terminal "./inf <string-to-print> <interval>"

This project was an assignment in CSC 360 Operating Systems intended to get farmiliar with system calls related to process management, and the process control block (PCB).
