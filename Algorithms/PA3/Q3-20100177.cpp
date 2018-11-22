/*
Data Structeres: No Special Data Structs used except a 2-D Array as a grid
How it works: It is a recursive divide and conquer algorithm which keepes divding the array into 4 parts and puts 3 blocked on part which 
didn't contain the blocked box orignally near the center and recursively calls itself until it can't anymore and then it has a grid of 2x2 
size and with one blocked it places the boomerang in the 3 open ones(Base Case). When it it has divided it then puts boomerang on three it blocked and returns to main
Recurrence Relation: 
Base Case T(2) = b (constant)
T(2n)=4T(n/4)+c
Running Time Complexity is O(n^2) since it is a grid of nxn and need to place on each one of them
Space Complexity is also O(n^2) since a grid of nxn needs to be stored
*/
#include <iostream>


using namespace std;



class Q3{
private:
	int n;
	char ** grid;
	void run(int startR,int startC,int blockR,int blockC,int size,char letter);
	void printGrid();
public:
	Q3(int size);
	~Q3();
	void initialize();

};
Q3::Q3(int size){
	n=size;
	initialize();
}
Q3::~Q3(){
	for(int i=0;i<n;i++){
		delete [] grid[i];
	}
	delete [] grid;
}
void Q3::initialize(){
	int row,col;
	grid = new char*[n];
	for(int i=0;i<n;i++){
		grid[i]=new char [n];
	}
	cout << "Input Blocked Row: ";
	cin >> row;
	cout << "Input Blocked Col: ";
	cin >> col;
	grid[col][row]='a';
	run(0,0,row,col,n,'A');
	printGrid();
}
void Q3::run(int startR,int startC,int blockR,int blockC,int size,char letter){
	if(size==2){
		grid[startC][startR]=letter+1;
		grid[startC+1][startR]=letter+1;
		grid[startC][startR+1]=letter+1;
		grid[startC+1][startR+1]=letter+1;
		grid[blockC][blockR]='!';
	}
	else{
		int letterGap = size/4;
		//find a fix for letters
		int newSize= size/2;
		bool containsBlocked[4];
		for(int i=0;i<4;i++){
			containsBlocked[i]=false;
		}
		//check for blocked sub request//
		if(startR+newSize <= blockR && startC+newSize<=blockC){
			containsBlocked[3]=true;
			//cout << startR << " "<< startC <<" "<< "3" << endl;		
		}
		else if(startC+newSize<=blockC){
			containsBlocked[1]=true;
			//cout << startR << " "<< startC <<" "<< "2" << endl;	
		}
		else if(startR+newSize<=blockR){
			containsBlocked[2]=true;
			//cout << startR << " "<< startC <<" "<< "1" << endl;	
		}
		else{
			containsBlocked[0]=true;
			//cout << startR << " "<< startC <<" "<< "0" << endl;	
		}
		//4 Sub Problems//
		if(!containsBlocked[0]){
			run(startR,startC,startR+newSize-1,startC+newSize-1,newSize,letter+(1*letterGap));
			grid[startC+newSize-1][startR+newSize-1]=letter;
		}
		else{
			run(startR,startC,blockR,blockC,newSize,letter);
		}
		if(!containsBlocked[1]){
			run(startR,startC+newSize,startR+newSize-1,startC+newSize,newSize,letter+(2*letterGap));
			grid[startC+newSize][startR+newSize-1]=letter;
		}
		else{
			run(startR,startC+newSize,blockR,blockC,newSize,letter);
		}
		if(!containsBlocked[2]){
			run(startR+newSize,startC,startR+newSize,startC+newSize-1,newSize,letter+(3*letterGap));
			grid[startC+newSize-1][startR+newSize]=letter;
		}
		else{
			run(startR+newSize,startC,blockR,blockC,newSize,letter);
		}
		if(!containsBlocked[3]){
			run(startR+newSize,startC+newSize,startR+newSize,startC+newSize,newSize,letter+(4*letterGap));
			grid[startC+newSize][startR+newSize]=letter;
		}
		else{
			run(startR+newSize,startC+newSize,blockR,blockC,newSize,letter);
		}
	}
}
void Q3::printGrid(){
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			cout << grid[i][j] << " ";
		}
		cout << endl;
	}
}

int main(){
	int m;
	cout << "Input Grid Size: ";
	cin >> m;
	Q3 obj(m);
	return 0;
}