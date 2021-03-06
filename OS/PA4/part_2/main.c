#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE 131072 //128KB (2^17)
#define VIRTUALSIZE 16777216 // 16MB (2^24)
#define PAGESIZE 1024 // 1 KB (2^10)
#define TOTALFRAMES 128

#define ENTRYSIZE 4
// #define BACKINGSTORE "BACKING_STORE.bin"
#define BACKINGSTORE "DATA_STORE.bin"
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

int freeFramePointer=34;
int freePagePointer=1;
unsigned int getPage1Number(int address){
	address>>=18;
	return address & 0x3F;
}
unsigned int getPage2Number(int address){		//gets the page entry number in the page table
	address>>=10;				
	return address & 0xFF;
}
unsigned int getOffset(int address){		//gets the offset from an address 
	return address & 0x3FF;		
}
unsigned int getFrameNumber(unsigned char * ptr){
	int frameNum = ptr[1];
	frameNum<<=8;
	frameNum |= ptr[2];
	return frameNum;
}

unsigned char divUp(int number){
	return (number >> 8) & 0xFF;
}
unsigned char divLow(int number){
	return number & 0xFF;
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
	int pageNum=pageIndex/ENTRYSIZE;
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

	int pageNum=(address>>10) & 0x3FFF;

	//printf("%04X %04X\n",frameNum,pageNum );
	FILE *fptr;
	fptr=fopen(BACKINGSTORE,"rb");
	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	unsigned int frameStartingPos=frameNum*PAGESIZE;
	fseek(fptr,pageNum*PAGESIZE,SEEK_SET);
	fread(memory+frameStartingPos,sizeof(unsigned char),PAGESIZE,fptr);
	fclose(fptr);
}

void writePageBack(int frame,unsigned char * memory){
	FILE *fptr;
	fptr=fopen(PAGESTORE,"r+b");

	unsigned int frameStartingPos=frame*PAGESIZE;
	if(frame==34){
		for(int i=frameStartingPos;i<frameStartingPos+PAGESIZE;i+=4){
			memory[i+3]=0;
		}
	}

	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	
	fseek(fptr,frame*PAGESIZE,SEEK_SET);
	fwrite(memory+frameStartingPos,sizeof(unsigned char),PAGESIZE,fptr);
	fclose(fptr);
}
//above this okay
int bringPageIntoMemory(int page1Num,unsigned char * memory){
//completed
	int pageIndex = page1Num * ENTRYSIZE;
	if(freePagePointer<34){		//if any frame is free assign that
		memory[pageIndex+1]=divUp(freePagePointer);
		memory[pageIndex+2]=divLow(freePagePointer);
		readPage(memory,freePagePointer,pageIndex);
		memory[pageIndex+3]=0;
		setInMemory(&memory[pageIndex+3]);
		return freePagePointer++;
	}
	else{
		int i, j;
		for(j=0;j<2;j++){
			int secondOption = -1;
			for(i=0;i<256;i+=4){
				if(checkInMemory(memory[i+3])){
					if(!checkUsed(memory[i+3]) && !checkDirty(memory[i+3])){
						memory[pageIndex+1]=memory[i+1];	//copy the frame number
						memory[pageIndex+2]=memory[i+2];
						int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
						writePageBack(frame,memory);
						memory[i+3]=0;
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
				memory[secondOption+3]=0;
				int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
				writePageBack(frame,memory);
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
	//printf("%04X %d\n",address,pageIndex);
	if(freeFramePointer<TOTALFRAMES){	
		//if any frame is free assign that
		memory[pageIndex+1]=divUp(freeFramePointer);
		memory[pageIndex+2]=divLow(freeFramePointer);
		readFrame(memory,freeFramePointer,pageIndex);
		memory[pageIndex+3]=0;
		setInMemory(&memory[pageIndex+3]);
		return freeFramePointer++;
	}
	else{

		int i, j;
		for(j=0;j<2;j++){
			int secondOption=-1;
			for(i=PAGESIZE;i<(PAGESIZE*33);i+=4){
				if(checkInMemory(memory[i+3])){
					
					if(!checkUsed(memory[i+3]) && !checkDirty(memory[i+3])){
						memory[pageIndex+1]=memory[i+1];	//copy the frame number
						memory[pageIndex+2]=memory[i+2];
						memory[i+3]=0;
						int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
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
				memory[secondOption+3]=0;
				int frame = getInt(memory[pageIndex+1],memory[pageIndex+2]);
				readFrame(memory,frame,address);
				memory[pageIndex+3]=0;
				setInMemory(&memory[pageIndex+3]);
				return frame;
			}
		}
	}

	memory[pageIndex+1]=divUp(34);
	memory[pageIndex+2]=divLow(34);
	readFrame(memory,34,pageIndex);
	memory[pageIndex+3]=0;
	setInMemory(&memory[pageIndex+3]);
	return 34;

}

int getFrame(int address,unsigned char * memory){

	int page1Num = getPage1Number(address);
	int page2Num = getPage2Number(address);
	int page1Index = page1Num * ENTRYSIZE;
	int page2Index = page2Num * ENTRYSIZE;
	int frame = getInt(memory[page1Index+1],memory[page1Index+2]);

	int index = (frame*PAGESIZE)+page2Index;
	

	int pageFault;
	int frameAddress=0;
	if(checkInMemory(memory[index+3])){		//..//w
		frameAddress = getInt(memory[index+1],memory[index+2]);
		pageFault=0;		//In memory so return the frame address	
		setUsed(&memory[index+3]); 
	}
	else{
		//printf("%06X %d\n",address,index );

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
		setUsed(&memory[index+3]);
	}
	return (pageFault<<16) | frame;
}
int readFromMemory(int address,unsigned char * memory){	
	int pageTable=getPageTable(address,memory);
	int pageFault1 = (pageTable >> 16) & 1;

	int rawFrame = getFrame(address,memory);

	int frame = rawFrame & 0xFFFF;
	int offset = getOffset(address);
	int pageFault2 = (rawFrame >> 16) & 1;
	
	unsigned char value=memory[(frame*PAGESIZE)+offset];

	
	int physicalAddress = (frame<<10)| offset;
	int innerFrame = (pageTable & 0xFFFF);

	printf(" 0x%06X      	 0x%04X           0x%06X       0x%04X",address, innerFrame, physicalAddress, value);
	if(pageFault1){
		printf("       Yes");
	}
	else{
		printf("       No ");
	}	
	if(pageFault2){
		printf("       Yes\n");
	}
	else{
		printf("       No\n");
	}	

	return pageFault1+pageFault2;

}
int writeToMemory(int address,unsigned char * memory){ //prototypes can be changed
	
	int pageTable=getPageTable(address,memory);
	int innerFrame = (pageTable & 0xFFFF);
	int pageFault1 = (pageTable >> 16) & 1;

	int rawFrame = getFrame(address,memory);

	int frame = rawFrame & 0xFFFF;
	
	int offset = getOffset(address);
	int pageFault2 = (rawFrame >> 16) & 1;

	unsigned char value=memory[(frame*PAGESIZE)+offset];
	int physicalAddress = (frame<<10)| offset;
	
	printf(" 0x%06X      	 0x%04X           0x%06X       0x%04X",address, innerFrame, physicalAddress, value);
	if(pageFault1){
		printf("       Yes");
	}
	else{
		printf("       No ");
	}	
	if(pageFault2){
		printf("       Yes\n");
	}
	else{
		printf("       No\n");
	}	
	int innerPageIndex = (innerFrame*PAGESIZE)+(getPage2Number(address)*ENTRYSIZE);
	setDirty(&memory[innerPageIndex+3]);

	return pageFault1+pageFault2;
}

void initStore(){
	unsigned char * temp = (unsigned char *)malloc(sizeof(unsigned char) * 64*PAGESIZE);
	FILE *fptr;
	fptr=fopen(PAGESTORE,"wb");
	if(fptr==NULL){
		printf("Error While Opening File\n");
		exit(1);
	}
	fwrite(temp,sizeof(unsigned char),PAGESIZE*64,fptr);
	fclose(fptr);
	free(temp);
}
int main(int argc, char *argv[]) {
	unsigned char * mainMemory = (unsigned char*)malloc(sizeof(unsigned char) * MEMSIZE); // mm = mainmemory
	
	int i;
	for (i=0;i<MEMSIZE;i++){
		mainMemory[i]=0;
	}
	initStore();

	
	FILE * fp;
	printf("Logical Addr    Inner Frame   Physical Addr     Value     PageFaultPT     PageFaultPage\n");
	if(argc==1){
		fp = fopen("addresses.txt","r");
	}
	else{
		fp= fopen(argv[1],"r");
	}
	
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
	  unsigned int hex = parseString(buffer,6);
	  fscanf(fp, "%s", &option);
	  if(option=='0'){
	  	pageFaultCount+=readFromMemory(hex,mainMemory);
	  }
	  else{
	  	pageFaultCount+=writeToMemory(hex,mainMemory);
	  }
	  counter+=2;
	}
	int pageFaultRate = (pageFaultCount / counter)*100;	
	printf("Page Fault Count = %d  Page Fault Rate = %d%%\n",pageFaultCount,pageFaultRate);
	printf("\n");
	//printPageTable(mainMemory);
	//printMemoryFrame(mainMemory+(2*256));
	free(mainMemory);
	return 0;
}
