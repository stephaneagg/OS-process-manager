#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include "linked_list.h"

// GLOBAL VARIABLES //

Node* head = NULL;

// HELPER FUNCTIONS //

void track_pstatus() {
  pid_t pid;
  int p_status;
  while (true) {
    pid = waitpid(-1, &p_status, WNOHANG | WUNTRACED | WCONTINUED);
    if (pid > 0) {
      if (WIFSIGNALED(p_status)) {
        printf("Process %d was killed\n", pid);
        head = deleteNode(head, pid);
      }
      if (WIFEXITED(p_status)) {
        printf("Process %d has exited\n", pid);
        head = deleteNode(head, pid);
      }
      if (WIFCONTINUED(p_status)) {
        printf("Process %d was started\n", pid);
      }
      if (WIFSTOPPED(p_status)) {
        printf("Process %d was stopped\n", pid);
      }
    } else {
      break;
    }
  }
}

// Opens given stat file path and writes the data into data
// used in pstat
void getstat(char *path, char **data) {
  FILE *file = fopen(path, "r");
  char contents[1024];
  if (file != NULL) {
    int i = 0;
    while (fgets(contents, sizeof(contents)-1, file) != NULL) {
      char *tok;
      tok = strtok(contents, " ");
      data[i] = tok;
      while (tok != NULL) {
        data[i] = tok;
        tok = strtok(NULL, " ");
        i++;
      }
    }
    fclose(file);
    return;
  } else {
    printf("Error: Failed in getstat()");
    return;
  }
}

// Opens given status file path and writes the data into data
// used in pstat
void getstatus(char *path, char data[][128]) {
  FILE *file = fopen(path, "r");
  if (file != NULL) {
    int i = 0;
    while (fgets(data[i] , 128, file) != NULL) {
      i++;
    }
    fclose(file);
    return;
  } else {
    printf("Error: Failed in getstatus()");
    return;
  }
}

// CORE FUNCTIONS //

// Creates process and adds it to linked list
void func_BG(char **cmd){
  char* command = cmd[1];
  pid_t pid = fork();

  if (pid == 0) { // Child Process
		if (execvp(command, &cmd[1]) < 0) {
      printf("Error: failed to execute command: %s\n", command);
      exit(-1);
    }
  }
  else if (pid > 0) { // Parent Process
    track_pstatus(); 
    printf("Executed background process, PID:%d\n", pid);
    char path[128];
    sprintf(path, "/proc/%d/%s", pid, command);
    if (head == NULL) {
      head = add_newNode(head, pid, path);
      sleep(1);
    }
    else {
      add_newNode(head, pid, path);
      sleep(1);
    }
  }
  else { // Fork Error
    printf("Error: Fork Error");
  }
}

// Prints a list of processes in the linked list
void func_BGlist(char **cmd){
  track_pstatus();
  printList(head);
}

// Sends the TERM signal to process with the given pid (terminates the process) and removes it from the linked list
void func_BGkill(char * str_pid){
  int pid = atoi(str_pid);

  if (PifExist(head, pid)) {
    int error = kill(pid, SIGTERM);
    if(error){
      printf("Error: Failed to kill PID:%d\n", pid);
    }
    else {
      printf("PID: %d killed\n", pid);
      head = deleteNode(head, pid);
      sleep(1);
    }
  }
  else {
    printf("Error: PID %d does not exist\n", pid);
  }
}

// Sends the STOP signal to process with the given PID (stops the process)
void func_BGstop(char * str_pid){
  int pid = atoi(str_pid);

  if (PifExist(head, pid)) {
    int error = kill(pid, SIGSTOP);
    if(error){
      printf("Error: Failed to stop PID:%d\n", pid);
    }
    else {
      printf("PID: %d stopped\n", pid);
      sleep(1);
    }
  }
  else {
    printf("Error: PID %d does not exist\n", pid);
  }
}

// Sends the CONT signal to process with the given PID (restarts the process)
void func_BGstart(char * str_pid){
  int pid = atoi(str_pid);

  if (PifExist(head, pid)) {
    int error = kill(pid, SIGCONT);
    if(error){
      printf("Error: Failed to start PID:%d\n", pid);
    }
    else {
      printf("PID: %d resumed\n", pid);
      sleep(1);
    }
  }
  else {
    printf("Error: PID %d does not exist\n", pid);
  }
}

// lists comm, state, utime, stime, rss, voluntary_ctxt_switches and nonvoluntary_ctxt_switches 
// for process with the given PID
void func_pstat(char * str_pid){
  int pid = atoi(str_pid);

  if (PifExist(head, pid)) {
    // Get paths to stat and status
    char statpath[128];
    char statuspath[128];
    sprintf(statpath, "/proc/%d/stat", pid);
		sprintf(statuspath, "/proc/%d/status", pid);
    
    // Populate stat and status data holders
    char *statdata[128];
    getstat(statpath, statdata);
    char statusdata[128][128];
    getstatus(statuspath, statusdata);
    char* endptr;
		long unsigned int utime = strtoul(statdata[13], &endptr, 10) / sysconf(_SC_CLK_TCK);
		long unsigned int stime = strtoul(statdata[14], &endptr, 10) / sysconf(_SC_CLK_TCK);

    // Print pstat contents
    printf("comm: %s\n", statdata[1]);
    printf("state: %s\n", statdata[2]);
    printf("utime: %lu\n", utime);
    printf("stime: %lu\n", stime);
    printf("rss: %s\n", statdata[24]);
    printf("%s", statusdata[55]);
    printf("%s", statusdata[56]);
  }
}

int main(){
    char user_input_str[50];
    while (true) {
      printf("Pman: > ");
      fgets(user_input_str, 50, stdin);
      printf("User input: %s \n", user_input_str);
      char * ptr = strtok(user_input_str, " \n");
      if(ptr == NULL){
        continue;
      }
      char * lst[50];
      int index = 0;
      lst[index] = ptr;
      index++;
      while(ptr != NULL){
        ptr = strtok(NULL, " \n");
        lst[index]=ptr;
        index++;
      }
      if (strcmp("bg",lst[0]) == 0){
        func_BG(lst);
      } else if (strcmp("bglist",lst[0]) == 0) {
        func_BGlist(lst);
      } else if (strcmp("bgkill",lst[0]) == 0) {
        func_BGkill(lst[1]);
      } else if (strcmp("bgstop",lst[0]) == 0) {
        func_BGstop(lst[1]);
      } else if (strcmp("bgstart",lst[0]) == 0) {
        func_BGstart(lst[1]);
      } else if (strcmp("pstat",lst[0]) == 0) {
        func_pstat(lst[1]);
      } else if (strcmp("q",lst[0]) == 0) {
        printf("Bye Bye \n");
        exit(0);
      } else {
        printf("Invalid input\n");
      }
    }

  return 0;
}

