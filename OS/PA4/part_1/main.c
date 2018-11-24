-#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE 16384 //16KB
#define VIRTUALSIZE 65536 // 64KB
#define PAGESIZE 256 // 256 bytes
#define PAGETABLESIZE 512 // 512 bytes
int getPageNumber(int address){		//gets the page entry number in the page table
	int mask = 256;
	address>>=8;				
	return address & mask;
}
int getOffset(int address){		//gets the offset from an address 
	int mask = 256;
	return address & mask;		
}
int checkInMemory(char val){	//checks if the page you're asking for is now in memory					
	return 2 & val;
}	
int checkDirty(char val){		//checks if dirty bit is on					
	return 2& val;	
}
int checkModified(char val){	//check if value is modifued
	return 4 & val;
}	
int checkUsed(char val){	//check if recently Used
	return 8 & val;
}
void setModified(char *val){
	*val |= 4;
}
void setUsed(char * val){		//sets the used bit to 1 in enchanhed page replacement technique
	*val |= 8;
}
int getEnchanhedReplacementValue(char val){
	return val & 12;
}
void reduceUsed(char * val){
	char mask = 8;
	mask ~= mask;
	* val & = mask;
}
int findReplacement(char * pageTable){
	int i,j;
	int secondOption =-1;
	for(i=0;i<2;i++){
		for(j=0;j<PAGETABLESIZE;j+=2){
			if(!checkUsed(pageTable[i]) && !checkModified(pageTable[i])){
				int temp = i<<8;
				return pageTable[i] | i;
			}
			else if (secondOption==-1 && !checkUsed(pageTable[i]) && checkModified(pageTable[i])){
				int temp = i<<8;
				secondOption=pageTable[i] | i;
			}
			else{
				reduceUsed(&pageTable[i]);
			}
		}
		if(secondOption!=-1){
			return secondOption;
		}
	}
}//use a better technique
int bringPageIntoMemory(int address,char * pageTable,int pageNum){ //brings the page from backing store to memory
	//read from backing store
	//

	int freeFrame = findReplacement(pageTable);



	return 0;
}

int getFrame(int address,char * pageTable){
	if (address>=VIRTUALSIZE || address<0 ){
		return 0;
		printf("Invalid Address Access\n");
	}
	else{
		int pageNum=2*getPageNumber(address);
		int check,frameAddress;
		check = pageTable[pageNum];
		if(checkInMemory(check)){		
			frameAddress = pageTable[pageNum+1];		//In memory so return the frame address	
		}
		else{
			frameAddress=bringPageIntoMemory(address,pageTable,pageNum); //bring the page table into memory and update the page table accordingly
 		}
 		setUsed(&pageTable[pageNum]); 
 		return frameAddress;
	}
}
int readFromMemory(int frame,int address){		//prototypes can be changed
	//read function
	return 0;
}
int writeToMemory(int frame,int address,int valToWrite){ //prototypes can be changed
	//write function
	return 0;
}
int main() {
	printf("This Works!\n");
	
	char * mainMemory = malloc(sizeof(char) * MEMSIZE); // mm = mainmemory
	char * startPtr = mainMemory + PAGETABLESIZE; //initial it has pagetable in start
	int i;
	for (i=0;i<MEMSIZE;i++){
		mainMemory[i]=0;
	}

	int add[4];
	add[0]=0;
	add[1]=65535;
	add[2]=65536;
	add[3]= 20;
	int frame = getFrame(add[0],mainMemory);
	if (frame ==0 || 1){
		printf("Error frame is supposed to be used to store page table\n");
	}
	else{
		//read or write at that frame whatever you want to do;
	}



	free(mainMemory);
	return 0;
}
