#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE 131072 //128KB (2^17)
#define VIRTUALSIZE 16777216 // 16MB (2^24)
#define PAGESIZE 1024 // 1 KB (2^10)
#define PAGETABLE1SIZE 256
#define TOTALFRAMES 128
#define TOTALPAGES 16384 
#define ENTRYSIZE 4
#define BACKINGSTORE "BACKING_STORE.bin"
#define PAGESTORE "PAGE_STORE.bin"

//Checking Functions


//File Parsing Functions
unsigned int parseChar(unsigned char c){
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

int freeFramePointer=33;
int freePagePointer=1;
int getPage1Number(int address){
	address>=18;
	return address & 0x3F;
}
int getPage2Number(int address){		//gets the page entry number in the page table
	address>>=10;				
	return address & 0xFF;
}
int getOffset(int address){		//gets the offset from an address 
	return address & 0x3FF;		
}
int getFrameNumber(unsigned char * ptr){
	int frameNum = ptr[1];
	frameNum<<=8;
	frameNum | = ptr[2];
	return frameNum;
}
unsigned char * divideIntoChar(int number){
	unsigned char[2] divided;
	divided[0]=number & 0xFF00;
	divided[1]=number & 0xFF;
	return divided;
}
int getInt(unsigned char first,unsigned char second){
	return (first << 8) | second;
}
// unsigned char getUp8(int address){		//gets the page entry number in the page table
// 	address>>=8;			
// 	return address & 0xFF;
// }
// unsigned char getLow8(int val){		//gets the offset from an address 
// 	return (char)val & 0xFF;		
// }
int checkInMemory(unsigned char val){	//checks if the page you're asking for is now in memory					
	return 1 & val;
}	
int checkDirty(unsigned char val){		//checks if dirty bit is on					
	return 2& val;	
}
int checkUsed(unsigned char val){	//check if recently Used
	return 4 & val;
}
void setInMemory(unsigned char * val){
	*val |= 1;
}
void removeInMemory(unsigned char *val){
	unsigned char mask =1;
	mask = ~mask;
	*val &= mask; //sets the 1st bit to zero
}
void setDirty(unsigned char *val){
	*val |= 2;
}
void setUsed(unsigned char * val){		//sets the used bit to 1 in enchanhed page replacement technique
	*val |= 4;
}
void decreaseUsed(unsigned char * val){
	unsigned char mask = 4;
	mask =~mask;
	*val &= mask;
}


void readPage(unsigned char * memory,int frameNum,int pageIndex){		
	//READ PAGE FROM PAGESTORE STORE
	int pageNum=pageIndex/ENTRYSIZE;
	//printf("%04X %04X\n",frameNum,pageNum );
	FILE *fptr;
	fptr=fopen(PAGESTORE,"ab+");
	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	unsigned int frameStartingPos=frameNum*PAGESIZE;
	fseek(fptr,pageNum*PAGESIZE,SEEK_SET);
	fread(memory+frameStartingPos,sizeof(unsigned char),PAGESIZE,fptr);
	fclose(fptr);
}
void readFrame(unsigned char * memory,int frameNum,int address){		
	//READ FRAME FROM BACKING STORE
	int pageNum=(address>>10) & 0x3FFF;

	//printf("%04X %04X\n",frameNum,pageNum );
	FILE *fptr;
	fptr=fopen(BACKINGSTORE,"ab+");
	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	unsigned int frameStartingPos=frameNum*PAGESIZE;
	fseek(fptr,pageNum*PAGESIZE,SEEK_SET);
	fread(memory+frameStartingPos,sizeof(unsigned char),PAGESIZE,fptr);
	fclose(fptr);
}
//above this okay
int bringPageIntoMemory(int page1Num,unsigned char * memory){
//completed
	int pageIndex = page1Num * ENTRYSIZE;
	if(freePagePointer<33){		//if any frame is free assign that
		char * divided divideIntoChar(freePagePointer);
		memory[pageIndex+1]=divided[0];
		memory[pageIndex+2]=divided[1];
		readPage(memory,freePagePointer,pageIndex);
		memory[pageIndex]=0;
		setInMemory(&memory[pageIndex+3]);
		return freePagePointer++;
	}
	else{
		int i, j;
		for(j=0;j<2;j++){
			int secondOption=-1;
			for(i=0;i<256;i+=4){
				if(checkInMemory(memory[i+3])){
					if(!checkUsed(memory[i+3]) && !checkDirty(memory[i+3])){
						memory[pageIndex+1]=memory[i+1];	//copy the frame number
						memory[pageIndex+2]=memory[i+2];
						int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
						removeInMemory(&memory[i+3]);
						readPage(memory,frame,pageIndex);
						memory[pageIndex+3]=0;
						setInMemory(&memory[pageIndex+3]);
						return frame;
					}
					else if (checkUsed(memory[i+3])){
						decreaseUsed(&memory[i+3]);			//give a second chance
					}
					else if (secondOption==-1){
						secondOption=i;
					}
				}
			}
			if(secondOption!=-1){
				memory[pageIndex+1]=memory[secondOption+1];
				memory[pageIndex+2]=memory[secondOption+2];
				int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
				removeInMemory(&memory[secondOption+3]);
				readPage(memory,frame,pageIndex);
				memory[pageIndex+3]=0;
				setInMemory(&memory[pageIndex+3]);
				return frame;
			}
		}
	}
	printf("Error Something Went Wrong\n");		//Program Shouldn't come here
	exit(1);
}
int bringFrameIntoMemory(int address,unsigned char* memory,int pageIndex){ 
	//completed
	if(freeFramePointer<TOTALFRAMES){		//if any frame is free assign that
		char * divided divideIntoChar(freeFramePointer);
		memory[pageIndex+1]=divided[0];
		memory[pageIndex+2]=divided[1];
		readFrame(pageTable,freeFramePointer,pageIndex);
		memory[pageIndex]=0;
		setInMemory(&memory[pageIndex+3]);
		return freeFramePointer++;
	}
	else{
		int i, j;
		for(j=0;j<2;j++){
			int secondOption=-1;
			for(i=PAGESIZE;i<PAGESIZE*33;i+=4){
				if(checkInMemory(memory[i+3])){
					if(!checkUsed(memory[i+3]) && !checkDirty(memory[i+3])){
						memory[pageIndex+1]=memory[i+1];	//copy the frame number
						memory[pageIndex+2]=memory[i+2];
						int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
						removeInMemory(&memory[i+3]);
						readFrame(memory,frame,address);
						memory[pageIndex+3]=0;
						setInMemory(&memory[pageIndex+3]);
						return frame;
					}
					else if (checkUsed(memory[i+3])){
						decreaseUsed(&memory[i+3]);			//give a second chance
					}
					else if (secondOption==-1){
						secondOption=i;
					}
				}
			}
			if(secondOption!=-1){
				memory[pageIndex+1]=memory[secondOption+1];
				memory[pageIndex+2]=memory[secondOption+2];
				int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
				removeInMemory(&memory[secondOption+3]);
				readFrame(memory,frame,address);
				memory[pageIndex+3]=0;
				setInMemory(&memory[pageIndex+3]);
				return frame;
			}
		}
	}
	printf("Error Something Went Wrong\n");		//Program Shouldn't come here
	exit(1);
}

int getFrame(int address,unsigned char * memory){

	int page1Num = getPage1Number(address);
	int page2Num = getPage2Number(address);
	int page1Index = page1Num * ENTRYSIZE;
	int page2Index = page2Num * ENTRYSIZE;
	int frame = getInt(memory[page1Index+1],memory[page1Index+2]);
	int index = (frame*PAGESIZE)+page2Index;
	int pageFault;
	if(checkInMemory(memory[index])){		//..//w
		frameAddress = getInt(memory[index+1],memory[index+2]);
		pageFault=0;		//In memory so return the frame address	
		setUsed(&memory[index+3]); 
	}
	else{
		frameAddress=bringFrameIntoMemory(address,memory,index); //bring the page table into memory and update the page table accordingly
		pageFault=1;
	}
	pageFault<<=16;
		
	return pageFault | frameAddress;
}
int getPageTable(int address,unsigned char * memory){
	int page1Num=getPage1Number(address);
	int index = ENTRYSIZE * page1Num;
	int pageFault=0;
	int frame;
	if(!checkInMemory(memory[index+3])){
		frame = bringPageIntoMemory(page1Num,memory);
		pageFault=1;
	}
	else{
		frame = getInt(memory[index+1],memory[index+2]);
		setUsed(&memory[index]);
	}
	return (pageFault<<16) | frame;
}
int readFromMemory(int address,unsigned char * pageTable){	
	int pageTable=getPageTable(address,pageTable);
	int offset = getOffset(address);
	int frame = getLow8(rawFrame);
	int pageFault = getUp8(rawFrame);
	unsigned char value=pageTable[frame*PAGESIZE+offset];
	int physicalAddress = (frame<<8)| offset;
	if(pageFault){
		printf(" 0x%04X      	 0x%04X           0x%04X       Yes\n",address, physicalAddress,value);
	}
	else{
		printf(" 0x%04X      	 0x%04X           0x%04X       No\n",address, physicalAddress,value);
	}	
	return pageFault;
}
int writeToMemory(int address,unsigned char * pageTable){ //prototypes can be changed
	int retVal=readFromMemory(address,pageTable);
	int pageIndex = ENTRYSIZE*getPageNumber(address);
	setDirty(&pageTable[pageIndex]);
	return retVal;
}
int main() {
	unsigned char * mainMemory = (unsigned char*)malloc(sizeof(unsigned char) * MEMSIZE); // mm = mainmemory
	
	int i;
	for (i=0;i<MEMSIZE;i++){
		mainMemory[i]=0;
	}
	

	printf("Logical Addr    Physical Addr     Value     PageFault\n");
	FILE * fp = fopen("addresses.txt","r");
	unsigned char buffer[9];
	unsigned char option;
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
	  	unsigned int hex = parseString(buffer+2,4);
	  	pageFaultCount+=readFromMemory(hex,mainMemory);
	  }
	  else{
	  	unsigned int hex = parseString(buffer+2,4);
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
