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

// Adds a new node to the linked list of nodes. Returns the new node.
// When calling this function on an empty list make sure to set the return value to head
Node * add_newNode(Node* head, pid_t new_pid, char * new_path){
	Node* nn = (Node*)malloc(sizeof(Node));
	nn -> pid = new_pid;
	nn -> path = strdup(new_path);
	nn -> next = NULL;
	if (head == NULL) {
		head = nn;
		return head;
	} else {
		Node* curr = head;
		while (curr->next != NULL) {
			curr = curr->next;
		}
		curr->next = nn;
		return nn;
	}
}

// Searches linked list for node with the corresponding pid and deletes that node
Node * deleteNode(Node* head, pid_t pid){
	Node* curr1 = head;
	Node* curr2 = NULL;

	while (curr1 != NULL) {
		if (curr1->pid == pid) {
			if (curr1 == head) {
				if (curr1->next == NULL) {
					return NULL;
				}
				head = head->next;
			} else {
				curr2->next = curr1->next;
			}
			free(curr1);
			return head;
		}
		curr2 = curr1;
		curr1 = curr1->next;
	}
	return head;
}

// Prints the PIDs of each node in the linked list
void printList(Node* head){
	if (head == NULL) {
		printf("Total background jobs: 0\n");
		return;
	}
	Node* curr = head;
	printf("%d: %s\n",curr->pid, curr->path);
	int i = 1;
	while (curr->next != NULL) {
		curr = curr->next;
		printf("%d: %s\n",curr->pid, curr->path);
		i++;
	}
	printf("Total background jobs: %d\n", i);
}

// Check if the given PID exists. Returns 1 if pid exists else 0
int PifExist(Node *head, pid_t pid){
	if (head == NULL) {
		return 0;
	}
	Node* curr = head;

	if (curr->pid == pid) {
		return 1;
	}
	while (curr->next != NULL) {
		curr = curr->next;
		if (curr->pid == pid) {
			return 1;
		}
	}
  	return 0;
}