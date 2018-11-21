
#include <stdio.h>
#include <stdlib.h>

struct queueEntry{
	int data;
	struct queueEntry *next;
};

int dequeue(struct queueEntry ** head){
	if(*head==NULL){
		return -1;
	}
	if ((*head)->next==NULL){
		int tempData =(*head)->data;
		free(*head);
		*head=NULL;
		return tempData;
	}
	else{
		struct queueEntry* temp=*head;
		int tempData=(*head)->data;
		(*head)=(*head)->next;
		free(temp);
		return tempData;

	}
}
void enqueue(int pData,struct queueEntry ** head){
	if(*head!=NULL){
		struct queueEntry* temp=*head;
		while(temp->next!=NULL){
			temp=temp->next;
		}
		temp->next=(struct queueEntry*)(malloc(sizeof(struct queueEntry)));
		temp=temp->next;
		temp->data=pData;
		temp->next=NULL;
		return;
	}
	else{
		*head = (struct queueEntry*)(malloc(sizeof(struct queueEntry)));
		(*head)->data=pData;
		(*head)->next=NULL;
		return;
	}	

}
int checkEmpty(struct queueEntry* head){
	if(head==NULL){
		return 0;
	}
	else{
		return 1;
	}
}
void printQueue(struct queueEntry * head){
	struct queueEntry* temp=head;
	if(temp==NULL){
		//printf("Queue Empty\n");
		return;
	}
	else{
		while(temp!=NULL){
			printf("%d ",temp->data);
			temp=temp->next;
		}
		printf("\n");
		return;
	}
}