#include <stdio.h>
#include <stdlib.h>

#define MEMSIZE = 16384; //16KB
#define VIRTUALSIZE = 65536; // 64KB
#define PAGESIZE = 256; // 256 bytes
#define PAGETABLESIZE = 512 // 512 bytes 


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
	
	free(mainMemory);
	return 0;
}
