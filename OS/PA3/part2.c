#include <pthread.h>
#include "main.h"
#include "part2.h"
#include <stdio.h>
#include <stdlib.h>

const int INTER_ARRIVAL_TIME = 5;
const int NUM_TRAINS = 5;
struct Node {
	int id;
	struct argument *data;
	struct Node *next;
};
 
struct Queue {
	struct Node *front;
	struct Node *last;
	unsigned int size;
};
 
void init(struct Queue *q) {
	q->front = NULL;
	q->last = NULL;
	q->size = 0;
}
 
struct Node *front(struct Queue *q) {
	return q->front;
}
 
void pop(struct Queue *q) {
	q->size--;
 
	struct Node *tmp = q->front;
	q->front = q->front->next;
	free(tmp);
}
 
void push(struct Queue *q, struct argument *data, int i) {
	q->size++;
 
	if (q->front == NULL) {
		q->front = (struct Node *) malloc(sizeof(struct Node));
		q->front->data = data;
		q->front->id = i;
		q->front->next = NULL;
		q->last = q->front;
	} else {
		q->last->next = (struct Node *) malloc(sizeof(struct Node));
		q->last->next->data = data;
		q->last->next->id = i;
		q->last->next->next = NULL;
		q->last = q->last->next;
	}
}

struct node{
	int userid;
	struct argument *arg;
	pthread_cond_t *condition;
	struct node *next;
};
struct List{
	struct node *front;
	unsigned int size;
};

void initList(struct List *l){
	l->front = NULL;
	l->size = 0;
}

pthread_cond_t* insertList(struct List *l, int data, struct argument *argu){
	l->size++;
	if(l->front == NULL) {
		l->front = (struct node *)malloc(sizeof(struct node));
		l->front->userid = data;		
		l->front->arg = argu;
		l->front->condition = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
		pthread_cond_init(l->front->condition,NULL);
		l->front->next = NULL;
	}
	else{
		struct node *temp = (struct node *) malloc(sizeof(struct node));
		temp->userid = data;
		temp->arg = argu;
		temp->condition = (pthread_cond_t *)malloc(sizeof(pthread_cond_t));
		pthread_cond_init(temp->condition,NULL);
		temp->next = l->front;
		l->front = temp;
	}
	return l->front->condition;
}
struct argument* searchDelete(struct List *l,int data){
	struct argument *ans = NULL;
	if(l->front->userid == data){//the head is the data		
		ans = l->front->arg;		
		struct node *temp = l->front;
		l->front = l->front->next;
		free(temp); 
		l->size--;
		return ans;
	}
	struct node *temp = l->front;
	while(temp->next){
		if(temp->next->userid == data){ //delete temp->next
			ans = temp->next->arg;
			struct node *del = temp->next;
			temp->next = temp->next->next;
			free(del);
			l->size--;
			return ans;
		}
	}
}

void searchSignal(struct List *l,int data,int trainid){
	if(l->front->userid == data){//the head is the data
		l->front->arg->id = trainid;		
		pthread_cond_signal(l->front->condition);		
		return ;
	}
	struct node *temp = l->front;
	while(temp->next){
		if(temp->next->userid == data){ //delete temp->next
			temp->next->arg->id = trainid;			
			pthread_cond_signal(temp->next->condition);
		}
	}
}


struct train{
	int station;
	int id;
	int passengers_inside;
	pthread_mutex_t capacity;	
	pthread_cond_t *getOn;
	pthread_cond_t *getOff;
	
};

struct transfer{
	struct train n;
	int stat;
	struct argument *arg;
	int userid;
};

int totalStations, maxPeople;
struct Queue *peopleWait; //array of queues. one for each station
pthread_mutex_t queuelock;
struct List *peopleExist;
pthread_mutex_t listlock;
int counterId;
pthread_mutex_t counterlock;
struct train *t; //array of trains and their attributes
int *trains_at_station; //which train is at which station.no train = -1
//struct List *people;

/**
 * Do any initial setup work in this function.
 * numStations: Total number of stations. Will be >= 5. Assume that initially
 * the first train is at station 1, the second at 2 and so on.
 * maxNumPeople: The maximum number of people in a train
 */
void initializeP2(int numStations, int maxNumPeople) {	
	int i;	
	int j;
	totalStations = numStations;
	maxPeople = maxNumPeople;
	counterId = 0;
		
	pthread_mutex_init(&counterlock,NULL);
	pthread_mutex_init(&queuelock,NULL);
	pthread_mutex_init(&listlock,NULL);
	
	peopleExist = (struct List*)malloc(sizeof(struct List)); 	
	initList(peopleExist);

	t = (struct train*)malloc(sizeof(struct train)*NUM_TRAINS);
	trains_at_station = (int*)malloc(sizeof(int)*totalStations);
	
	peopleWait = (struct Queue*)(malloc(sizeof(struct Queue)*totalStations));
	
	for(i = 0; i< totalStations; i++){
		init(&peopleWait[i]);
		trains_at_station[i] = -1;
	}
	
	for(i =0; i < NUM_TRAINS; i++){
		trains_at_station[i] = i;

		t[i].station = i;
		t[i].id = i;
		t[i].passengers_inside = 0;
		pthread_mutex_init(&t[i].capacity,NULL);
		t[i].getOn = malloc(sizeof(pthread_cond_t)*totalStations);
		t[i].getOff = malloc(sizeof(pthread_cond_t)*totalStations);
		for(j = 0; j<totalStations; j++){
			pthread_cond_init(&(t[i].getOn[j]),NULL);
			pthread_cond_init(&(t[i].getOff[j]),NULL);
		}
		
	}
	
}

/**
 * Print in the following format:
 * If a user borads on train 0, from station 0 to station 1, and another boards
 * train 2 from station 2 to station 4, then the output will be
 * 0 0 1
 * 2 2 4
 */
void *goingFromToP2(void *argu) {
	struct argument *arg = (struct argument*)(argu);
	
	if(arg->to >totalStations || arg->to<0 || arg->from <0 || arg->from >totalStations){
    		printf("Wrong Station Requested\n");
    		exit(1);
    	}
	//add urself to the station queue and get ur lock from people list
	//first see if u can enter train by looking at train's size of passengers. if can
	//then call wait. make this a while loop so can be confirmed about it
	int From = arg->from;
	int id;
	pthread_cond_t *c = (pthread_cond_t*)malloc(sizeof(pthread_cond_t)); 

	pthread_mutex_lock(&counterlock);
	id = counterId;
	counterId++;
	pthread_mutex_unlock(&counterlock);
	
	pthread_mutex_lock(&listlock);	
	c = insertList(peopleExist,id,arg);
	pthread_mutex_unlock(&listlock);
	
	pthread_mutex_lock(&queuelock);
	//printf("station %d id %d\n",From,id);	
	push(&peopleWait[From],arg,id);
	pthread_mutex_unlock(&queuelock);
	
	pthread_mutex_t lock;
	pthread_mutex_init(&lock,NULL);
	//printf("hello %d\n", From);	
	pthread_cond_wait(c,&lock);

	pthread_mutex_lock(&listlock);
	arg = searchDelete(peopleExist,id);
	pthread_mutex_unlock(&listlock);

	
	printf("%d %d %d\n",arg->id, arg->from, arg->to);
	pthread_exit(NULL);	
}

void* Request(void* value){ //id of user
	//when make req first increment people on train
	//when leaving make sure to decremnet
	struct transfer *v = (struct transfer*)(value);
	int From = v->arg->from;
	int To = v->arg->to;
	pthread_mutex_t lock;
	pthread_mutex_init(&lock,NULL);
	//printf("ok so request\n");
	
	pthread_mutex_lock(&v->n.capacity);
	if(v->n.passengers_inside < maxPeople) v->n.passengers_inside++;
	pthread_mutex_unlock(&v->n.capacity);	
	
	pthread_cond_wait(&v->n.getOn[From],&lock);
	//printf("I userid %d have got on\n",v->userid);	
	pthread_cond_wait(&v->n.getOff[To],&lock);

	pthread_mutex_lock(&v->n.capacity);
	if(v->n.passengers_inside > 0) v->n.passengers_inside--;
	pthread_mutex_unlock(&v->n.capacity);
	//printf("I userid %d have got off\n",v->userid);
	
	searchSignal(peopleExist,v->userid,v->n.id);
	//printf("I userid %d have signaled\n",v->userid);
	pthread_exit(NULL);
}

void controller(struct train n, int stat){
	//get the size of passengers in train and pop that many people and send them to 
	//func where they make req to get on and off
	int count, available,reqsize, min;	
	//by now anyone who had to get off is prob off
	//check size of queue as well and compare with size of people on train
	pthread_mutex_lock(&n.capacity);
	count = n.passengers_inside;
	pthread_mutex_unlock(&n.capacity);
	//write this bloack again. available is supposed to be people space inside train
	//then another variable of how many req there are is compared
	available = maxPeople - count;
	
	pthread_mutex_lock(&queuelock);
	reqsize = peopleWait[stat].size;
	pthread_mutex_unlock(&queuelock);

	if(reqsize <= available)min = reqsize;
	else min = available;
	//printf("controller station %d train %d available %d reqsize %d min %d\n",stat,n.id,available,reqsize,min);

	while(min){
		pthread_t tid;
		pthread_mutex_lock(&queuelock);
		struct Node *lucky = front(&peopleWait[stat]);
		//printf("station %d lucky->id %d\n",stat,lucky->id);
		
		struct transfer *tran = malloc(sizeof(struct transfer));
		tran->n = n;
		tran->stat = stat;
		tran->arg = lucky->data;
		tran->userid = lucky->id; 
		
		pop(&peopleWait[stat]);
		pthread_create(&tid,NULL,Request,tran);
		pthread_mutex_unlock(&queuelock);
		min--;
	} 
	sleep(1);
	
}

void* runTrains(){
	sleep(1);
	while(1){
		int i;		
		for(i = 0; i < NUM_TRAINS; i++){
			int s = t[i].station;			
			if(pthread_cond_broadcast(&t[i].getOff[s]) != 0)exit(1);//now space is made
			controller(t[i],s);
			//printf("about to give signal\n");
			if(pthread_cond_broadcast(&t[i].getOn[s]) != 0)exit(1);
			//printf("gave signal\n");
			t[i].station = (s+1)%totalStations;
			trains_at_station[t[i].station] = i;
		}
		sleep(INTER_ARRIVAL_TIME);	
		//frist signal people waiting to get off. then call function taht will 
		//signal people to get on
		//so the trains just move to the next station and call their own cond signal
		//for that station. the max people condition is covered by controller	
	
	}
}

void startP2(){
	pthread_t tid;
	pthread_create(&tid,NULL,runTrains,NULL);
	
}

