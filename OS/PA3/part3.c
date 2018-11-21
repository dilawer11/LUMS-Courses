#include <pthread.h>
#include "main.h"
#include "part3.h"
#include <stdio.h>

const int totalCars = 100;


struct lane_controller{
	int id;
	struct queueEntry * leftQueue;
	struct queueEntry * rightQueue;
};

struct lc_arg{
	int ID;
}

//Variables
struct lane_controller * laneControllers;
int * greenSignal; //tells which lane is on
pthread_cond_t * carSignal; //signals cars wait on
pthread_mutex_t * carSignalMutex; //Associated mutexes	
pthread_cond_t laneCond[4];			//laneController Waiting
pthread_cond_t laneCondMutex[4];	//Associated Mutexes
//End Variables


void * sideController(void * argu){
	struct lc_arg * arg = (struct lc_arg *)(argu);
	struct lane_controller * self = laneController[arg->ID];
	self->id=arg->ID;
	self->leftQueue=NULL;
	self->rightQueue=NULL;
	while(1){
		if(pthread_cond_wait(&laneCond[self->id],&laneCondMutex[self->id])!=0){
			printf("Wait Failed At Side %d \n",self->id );
			exit(1)
		}
		int i=0;
		int carID;
		printf("Queue State of Side controller ID %d\n",self->id);
		printQueue(self->leftQueue);
		printQueue(self->rightQueue);
		//empty upto 5 people from left lane
		while(i<5 && self->leftQueue!=NULL){
			carID=dequeue(&self->leftQueue);
			if(pthread_cond_signal(&carSignal[carID])!=0){
				printf("Signal Failed ID %d",carID);
				exit(1);
			}
		}
		sleep(1);
		/*
		if(self->leftQueue==NULL){
			if(pthread_cond_broadcast()!=0){
				printf("Broadcast Left Queue Failed");		//if left queue is empty 
				exit(1);
			}
		}
		*/
		i=0;

		//empty upto 5 people from right lane
		while(i<5 && rightQueue!=NULL){
			carID=dequeue(&rightQueue);
			if(pthread_cond_signal(&carSignal[carID])!=0){
				printf("Signal Failed ID %d",carID);
				exit(1);
			}
		}
		//Send the signal to next laneController
		sleep(2);
		printf("\n\n\n");
		int nextID=(self->ID+1)%4;
		if(pthread_cond_signal(&laneCond[nextID])!=0){
			printf("Signal Failed\n");									
		}
	}
}

void initializeP3() {
	laneControllers = (struct lane_controller*)(malloc(sizeof(lane_controller)*4));
	//create threads and send in controller function
	carSignal=(pthread_cond_t*)(malloc(sizeof(pthread_cond_t)*totalCars));
	carSignalMutex=(pthread_mutex_t*)(malloc(sizeof(pthread_mutex_t)*totalCars));
	int i;
	for(i=0;i<totalCars;i++){
		if(pthread_cond_init(&carSignal[i])!=0){
			printf("Init Cond Car Failed\n");
			exit(1);
		}
		if(pthread_mutex_init(&carSignalMutex[i])!=0){
			printf("Init Mutex Car Failed\n");
			exit(1);
		}
	}
	greenSignal = (int*)(malloc(sizeof(int)*4));
	for(i=0;i<4;i++){
		if(pthread_cond_init(&laneCond[i])!=0){
			printf("Init Cond Lane Failed\n");
			exit(1);
		}
		if(pthread_mutex_init(&laneCondMutex[i])!=0){
			printf("Init Mutex Lane Failed\n");
			exit(1);
		}
		greenSignal[i]=0;
	}
	



}

int getDirection(int from,int to){		//straigh =0 left= -1 right =1
	if(from==0){ //north;
		if(to==1){
			return 0;
		}
		else if (to==2){
			return -1;
		}
		else if(to==3){
			return 1;
		}
	}
	else if(from==1){
		if(to==0){
			return 0;
		}
		else if (to==2){
			return 1;
		}
		else if(to==3){
			return -1;
		}
	}
	else if(from==2){
		if(to==3){
			return 0;
		}
		else if (to==0){
			return 1;
		}
		else if(to==2){
			return -1;
		}
	}

	else if(from==3){
		if(to==2){
			return 0;
		}
		else if (to==0){
			return -1;
		}
		else if(to==2){
			return 1;
		}
	}
	printf("Enexpected Turn Taken\n");
	return -2;


}
/**
 * If there is a car going from SOUTH to NORTH, from lane LEFT,
 * print 
 * SOUTH NORTH LEFT
 * Also, if two cars can simulateneously travel in the two lanes,
 * first print all the cars in the LEFT lane, followed by all the
 * cars in the right lane
 */
void* goingFromToP3(void *argu){				
	//find the laneController struct
	struct argumentP3 * arg = (struct argumentP3*)(argu);
 	arg->from = getEnum(cars[i][0]);
    arg->to = getEnum(cars[i][1]);
    arg->lane = getEnum(cars[i][2]);
    arg->user_id = i;
    struct lane_controller * enterLane=laneControllers[arg->from];
    printf("ID %d Waiting on Place %d\n",arg->user_id,arg->from);
    if(arg->lane==1){
    	enqueue(arg->user_id,&(enterLane->rightQueue));
    	printf("Inserted In right queue\n");
    	if(pthread_cond_wait(&carSignalMutex[arg->user_id],&carSignalMutex[arg->user_id])!=0){
    		printf("Wait Failed ID %d",arg->user_id);
    		exit(1);
    	}
    }
    int carDirection = getDirection(arg->from,arg->to);
    else if(carDirection==-1){
    	printf("Checking Left \n");
    	if(enterLane->leftQueue!=NULL){
    		printf("Waiting on left\n");
    		while(enterLane->leftQueue!=NULL){
    			sleep(1); // remove this
    		}
    	}

    }
    else{
    	enqueue(arg->user_id,&enterLane->leftQueue);
    	printf("Inserted In left/Straight queue\n");
    	if(pthread_cond_wait(&carSignalMutex[arg->user_id],&carSignalMutex[arg->user_id])!=0){
    		printf("Wait Failed ID %d",arg->user_id);
    		exit(1);
    	}
    }
    printf("ID %d Exited\n",arg->user_id);
	//exit printing things
}

void startP3(){
	printf("Starting Traffic Controller\n");
	if(pthread_cond_signal(&laneCond[0])!=0){
		printf("Signal Failed\n");
		exit(1);
	}
	printf("Initial Signal Sent\n");
	return;
}
