#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE 16384 //16KB
#define VIRTUALSIZE 65536 // 64KB
#define PAGESIZE 256 // 256 bytes
#define PAGETABLESIZE 512 // 512 bytes
#define TOTALFRAMES 64
#define TOTALPAGES 256
#define BACKINGSTORE "BACKING_STORE.bin"

int freeFramePointer=2;
int getPageNumber(int address){		//gets the page entry number in the page table
	address>>=8;				
	return address & 0xFF;
}
int getOffset(int address){		//gets the offset from an address 
	return address & 0xFF;		
}

char getUp8(int address){		//gets the page entry number in the page table
	address>>=8;			
	return address & 0xFF;
}
char getLow8(int val){		//gets the offset from an address 
	return (char)val & 0xFF;		
}
int checkInMemory(char val){	//checks if the page you're asking for is now in memory					
	return 1 & val;
}	
int checkDirty(char val){		//checks if dirty bit is on					
	return 2& val;	
}
int checkUsed(char val){	//check if recently Used
	return 4 & val;
}
void setInMemory(char * val){
	*val |= 1;
}
void removeInMemory(char *val){
	*val &= -2; //sets the 1st bit to zero
}
void setDirty(char *val){
	*val |= 2;
}
void setUsed(char * val){		//sets the used bit to 1 in enchanhed page replacement technique
	*val |= 4;
}
void decreaseUsed(char * val){
	char mask = 4;
	mask =~mask;
	*val &= mask;
}

void readFrame(char * memory,int frameNum,int pageNum){
	//READ FRAME FROM BACKING STORE
	FILE *fptr;
	fptr=fopen(BACKINGSTORE,"ab+");
	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	unsigned int frameStartingPos=frameNum*PAGESIZE;
	fseek(fptr,pageNum*PAGESIZE,SEEK_SET);
	fread(memory+frameStartingPos,sizeof(char),PAGESIZE,fptr);
	fclose(fptr);
}
void writeBackToStore(char * memory, int frameNum,int pageNum){		
	FILE *fptr;
	fptr=fopen(BACKINGSTORE,"ab+");
	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	unsigned int frameStartingPos=frameNum*PAGESIZE;
	fseek(fptr,pageNum*PAGESIZE,SEEK_SET);
	fwrite(memory+frameStartingPos,sizeof(char),PAGESIZE,fptr);
	fclose(fptr);
}
int bringPageIntoMemory(int address,char * pageTable,int pageNum){ 		//completed
	if(freeFramePointer<TOTALFRAMES){		//if any frame is free assign that
		pageTable[pageNum+1]=freeFramePointer;
		readFrame(pageTable+PAGETABLESIZE,freeFramePointer,pageNum);
		setInMemory(&pageTable[pageNum]);
		freeFramePointer++;
		return freeFramePointer;
	}
	else{
		int secondOption=-1;
		int i, j;
		for(j=0;j<2;j++){
			for(i=0;i<PAGETABLESIZE;i+=2){
				if(checkInMemory(pageTable[i])){
					if(!checkUsed(pageTable[i]) && !checkDirty(pageTable[i])){
						pageTable[pageNum+1]=pageTable[i+1];
						removeInMemory(&pageTable[i]);
						readFrame(pageTable+PAGETABLESIZE,(int)pageTable[i+1],pageNum);
						setInMemory(&pageTable[pageNum]);
						return pageTable[i+1];
					}
					else if (checkUsed(pageTable[i])){
						decreaseUsed(&pageTable[i]);			//give a second chance
					}
					else if (secondOption!=-1){
						secondOption=i;
					}
				}
			}
			if(secondOption!=-1){
				writeBackToStore(pageTable+PAGETABLESIZE,pageTable[secondOption+1],pageNum);
				pageTable[pageNum+1]=pageTable[secondOption+1];
				removeInMemory(&pageTable[secondOption]);
				readFrame(pageTable+PAGETABLESIZE,(int)pageTable[secondOption+1],pageNum);
				setInMemory(&pageTable[pageNum]);
				return pageTable[secondOption+1];
			}
		}
	}
	printf("Error Something Went Wrong\n");		//Program Shouldn't come here
	return 0;
}

int getFrame(int address,char * pageTable){
	int pageFault;
	if (address>=VIRTUALSIZE || address<0 ){
		return 0;
		printf("Invalid Address Access\n");
	}
	else{
		int pageNum=2*getPageNumber(address);
		int check,frameAddress;
		check = pageTable[pageNum];
		if(checkInMemory(check)){		
			frameAddress = pageTable[pageNum+1];
			pageFault=0;		//In memory so return the frame address	
		}
		else{
			frameAddress=bringPageIntoMemory(address,pageTable,pageNum); //bring the page table into memory and update the page table accordingly
			pageFault=1;
 		}
 		pageFault<<=8;
 		setUsed(&pageTable[pageNum]); 
 		return pageFault | frameAddress;
	}
}
int readFromMemory(int address,char * pageTable){	
	int rawFrame=getFrame(getPageNumber(address),pageTable);
	int offset = getOffset(address);
	int frame = getLow8(rawFrame);
	int pageFault = getUp8(rawFrame);
	char * memory = pageTable+PAGETABLESIZE;
	char value=memory[frame*PAGESIZE+offset];
	int physicalAddress = (frame<<8)| offset;
	char * pfault;
	if(pageFault){
		pfault=(char*)(malloc(sizeof(char)*3));
		pfault="Yes";
	}
	else{
		pfault=(char*)(malloc(sizeof(char)*2));
		pfault="No";
	}
	printf("%d %d %d %s\n",address, physicalAddress,value,pfault);
	return pageFault;
}
int writeToMemory(int address,char * pageTable){ //prototypes can be changed
	int rawFrame=getFrame(getPageNumber(address),pageTable);
	int pageNum = getPageNumber(address);
	setDirty(&pageTable[pageNum]);
	int pageFault = getUp8(rawFrame);
	/*
	int offset = getOffset(address);
	int frame = getLow8(RawFrame);
	int pageFault = getUp8(RawFrame);
	char * memory = pageTable+PAGETABLESIZE;
	char value=memory[frame*PAGESIZE+offset];
	int physicalAddress = (frame<<8)| offset
	char * pfault;
	if(pageFault){
		pfault=(char*)(malloc(sizeof(char)*3));
		pfault="Yes";
	}
	else{
		pfault=(char*)(malloc(sizeof(char)*2));
		pfault="No";
	}
	printf("%d %d %d %s\n",address, physicalAddress,value,pfault);
	*/
	return pageFault;

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
	add[2]=30;
	add[3]= 20;
	int pageFaultCount=0;
	for(i=0;i<4;i++){
		pageFaultCount+=readFromMemory(add[i],mainMemory);
	}
	printf("Page Fault Count = %d\n",pageFaultCount);



	free(mainMemory);
	return 0;
}
