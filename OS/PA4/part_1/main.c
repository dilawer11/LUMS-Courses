#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE 16384 //16KB
#define VIRTUALSIZE 65536 // 64KB
#define PAGESIZE 256 // 256 bytes
#define PAGETABLESIZE 512 // 512 bytes
#define TOTALFRAMES 64
#define TOTALPAGES 256
#define BACKINGSTORE "BACKING_STORE.bin"

// #define TOTALFRAMES 8
// #define MEMSIZE 2048
// #define VIRTUALSIZE 4096 // 64KB
// #define PAGETABLESIZE 32 // 512 bytes

//Checking Functions
void printPageTable(char * pageTable){
	int i;
	printf("\n");
	for(i=0;i<PAGETABLESIZE;i+=2){
		printf("%04X %04X\n",pageTable[i],pageTable[i+1]);
	}
	printf("\n");
}
void printMemoryFrame(char * frame){
	int i;
	for(i=0;i<PAGESIZE;i++){
		if(i%4==0){
			printf("\n");
		}
		printf("%02X ",frame[i]);
		
	}
}

//File Parsing Functions
unsigned int parseChar(char c){
	if(c>='0' && c<='9'){
		return c-'0';
	}
	else if(c>='a' && c<='f'){
		return 10+c-'a';
	}
	else if(c>='A' && c<='F'){
		return 10+c-'A';
	}
	else{
		printf("Error while parsing character\n");
		exit(1);
	}
}
unsigned int parseString(char * c,int size){
	int i;
	//printf("%s\n",c );
	unsigned int hexValue=0;
	for(i=0;i<size;i++){
		//unsigned int parsedChar = parseChar(c[i]);
		//printf("%d\n",parsedChar );
		hexValue<<=4;
		hexValue+=parseChar(c[i]);
	}
	return hexValue;
}

//

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
	char mask =1;
	mask = ~mask;
	*val &= mask; //sets the 1st bit to zero
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

void readFrame(char * memory,int frameNum,int pageIndex){		//checked
	//READ FRAME FROM BACKING STORE
	int pageNum=pageIndex/2;
	//printf("%04X %04X\n",frameNum,pageNum );
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
void writeBackToStore(char * memory, int frameNum,int pageIndex){	//checkthis
	int pageNum=pageIndex/2;	
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
int bringPageIntoMemory(int address,char * pageTable,int pageIndex){ 
	//completed
	if(freeFramePointer<TOTALFRAMES){		//if any frame is free assign that
		pageTable[pageIndex+1]=freeFramePointer;
		readFrame(pageTable,freeFramePointer,pageIndex);
		pageTable[pageIndex]=0;
		setInMemory(&pageTable[pageIndex]);
		return freeFramePointer++;
	}
	else{
		int secondOption=-1;
		int i, j;
		for(j=0;j<2;j++){
			for(i=0;i<PAGETABLESIZE;i+=2){
				if(checkInMemory(pageTable[i])){
					if(!checkUsed(pageTable[i]) && !checkDirty(pageTable[i])){
						pageTable[pageIndex+1]=pageTable[i+1];
						removeInMemory(&pageTable[i]);
						readFrame(pageTable,(int)pageTable[i+1],pageIndex);
						pageTable[pageIndex]=0;
						setInMemory(&pageTable[pageIndex]);
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
				writeBackToStore(pageTable,pageTable[secondOption+1],pageIndex);
				pageTable[pageIndex+1]=pageTable[secondOption+1];
				removeInMemory(&pageTable[secondOption]);
				readFrame(pageTable,(int)pageTable[secondOption+1],pageIndex);
				pageTable[pageIndex]=0;
				setInMemory(&pageTable[pageIndex]);
				return pageTable[secondOption+1];
			}
		}
	}
	printf("Error Something Went Wrong\n");		//Program Shouldn't come here
	exit(1);
}

int getFrame(int address,char * pageTable){

	int pageFault;
	//printPageTable(pageTable);
	if (address>=VIRTUALSIZE || address<0 ){
		printf("Invalid Address Access\n");
		exit(1);
	}
	else{
		int pageIndex=2*getPageNumber(address);
		char check,frameAddress;
		check = pageTable[pageIndex];

		if(checkInMemory(check)){		
			frameAddress = pageTable[pageIndex+1];
			pageFault=0;		//In memory so return the frame address	
			setUsed(&pageTable[pageIndex]); 
		}
		else{
			frameAddress=bringPageIntoMemory(address,pageTable,pageIndex); //bring the page table into memory and update the page table accordingly
			pageFault=1;
 		}
 		pageFault<<=8;
 		
 		return pageFault | frameAddress;
	}
}
int readFromMemory(int address,char * pageTable){	
	int rawFrame=getFrame(address,pageTable);
	int offset = getOffset(address);
	int frame = getLow8(rawFrame);
	int pageFault = getUp8(rawFrame);
	char value=pageTable[frame*PAGESIZE+offset];
	int physicalAddress = (frame<<8)| offset;
	if(pageFault){
		printf(" 0x%04X      	 0x%04X           0x%04X       Yes\n",address, physicalAddress,value);
	}
	else{
		printf(" 0x%04X      	 0x%04X           0x%04X       No\n",address, physicalAddress,value);
	}	
	return pageFault;
}
int writeToMemory(int address,char * pageTable){ //prototypes can be changed
	int rawFrame=getFrame(address,pageTable);
	int pageNum = getPageNumber(address);
	int pageIndex = 2 * pageNum;
	setDirty(&pageTable[pageIndex]);
	int pageFault = getUp8(rawFrame);
	return pageFault;
}
int main() {
	char * mainMemory = malloc(sizeof(char) * MEMSIZE); // mm = mainmemory
	
	int i;
	for (i=0;i<MEMSIZE;i++){
		mainMemory[i]=0;
	}
	

	printf("Logical Addr    Physical Addr     Value     PageFault\n");
	FILE * fp = fopen("addresses.txt","r");
	char buffer[9];
	char option;
	// while(fread(buffer,sizeof(buffer),1,fp)){
	// 	printf("%s\n",buffer );
	// }
	// if(feof(fp)){
	// 	printf("End of File\n");
	// }
	// else{
	// 	printf("Error\n");
	// }
	// fclose(fp);
	int add[8];
	double counter=0.0;
	int pageFaultCount=0.0;
	while(!feof(fp))
	{
	  fscanf(fp, "%s", buffer);
	  unsigned int hex = parseString(buffer+2,4);
	  fscanf(fp, "%s", &option);
	  if(option=='0'){
	  	pageFaultCount+=readFromMemory(hex,mainMemory);
	  }
	  else{
	  	pageFaultCount+=writeToMemory(hex,mainMemory);
	  }
	  counter++;
	}
	int pageFaultRate = (pageFaultCount / counter)*100;	
	printf("Page Fault Count = %d  Page Fault Rate = %d%%\n",pageFaultCount,pageFaultRate);
	printf("\n");
	//printPageTable(mainMemory);
	//printMemoryFrame(mainMemory+(2*256));
	free(mainMemory);
	return 0;
}
