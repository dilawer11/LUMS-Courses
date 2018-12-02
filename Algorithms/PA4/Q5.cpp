#include <iostream>
#include <vector>
#include <fstream>
#include <ctime>

using namespace std;
ifstream fin;
ofstream fout;
struct points{
	int row;
	int col;
	points(){
		row=0;
		col=0;
	}
	points(int pRow,int pCol){
		row=pRow;
		col=pCol;
	}
};
struct retVal{
	int val;
	points startPos;
	int size;
	retVal(){
		val=0;
		size=-1;
	}
	retVal(int pVal,points pStart,int pSize){
		val=pVal;
		startPos=pStart;
		size=pSize;
	}
};
class Q5{
public:
	Q5();
	Q5(int sizeN);
	Q5(int * arr,int sizeN);
	void run();
	

private:
	int n;
	int ** grid;
	int ** sumGrid;
	void printGrid(int ** arr, int sizeN);
	retVal maxGridSum(points start,int sizeN);
	void printResult(retVal temp);
	int ** makeSum(int ** grid,int sizeN);
	
};
Q5::Q5(){
	cout << "Input Grid Size" << endl;
	cin >> n;
	grid = new int * [n];
	cout << "Input Grid Values" << endl;
	for(int i=0;i<n;i++){
		grid[i]=new int[n];
		for(int j=0;j<n;j++){
			cin >> grid[i][j];
		}
		cout << endl;
	}
	run();

}
Q5::Q5(int sizeN){//random
	n=sizeN;
	grid = new int * [n];
	srand((unsigned)time(0));
	for(int i=0;i<sizeN;i++){
		grid[i]=new int[n];
		for(int j=0;j<sizeN;j++){
			grid[i][j]=(rand()%200)-100;
		}
	}
	run();
}
Q5::Q5(int * arr,int sizeN){//from an array of size nxn
	n=sizeN;
	grid = new int * [n];
	for(int i=0;i<sizeN;i++){
		grid[i]= new int[n];
		for(int j=0;j<sizeN;j++){
			grid[i][j]=arr[(i*sizeN)+j];
		}
	}
	run();
}
void Q5::run(){
	printGrid(grid,n);
	cout << endl;
	sumGrid = makeSum(grid,n);
	printGrid(sumGrid,n);
	cout << endl;
	points startpoints(0,0);
	retVal temp = maxGridSum(startpoints,1);
	printResult(temp);
}

retVal Q5::maxGridSum(points start,int sizeN){
	//cout << "Called With Points : " << start.row << " " << start.col << " Size : " << sizeN << endl;
	if(start.row>=n || start.col>=n || start.row+sizeN-1 >= n || start.col+sizeN-1 >= n){ 
		//cout << "here" << endl;
		return retVal(0,points(0,0),-1);
	}
	else{
		retVal sumValues[4];
		int colVal=0,rowVal=0,diagVal=0;
		if(start.row-1>=0 && start.col-1>=0){
			diagVal = sumGrid[start.row-1][start.col-1];
			rowVal=sumGrid[start.row-1][start.col+sizeN-1];
			colVal=sumGrid[start.row+sizeN-1][start.col-1];
		}
		else if(start.row-1>=0){
			rowVal=sumGrid[start.row-1][start.col+sizeN-1];
		}
		else if(start.col-1>=0){
			colVal=sumGrid[start.row+sizeN-1][start.col-1];
		}
		sumValues[0].val=sumGrid[start.row+sizeN-1][start.col+sizeN-1] + diagVal - rowVal - colVal;
		sumValues[0].size=sizeN;
		sumValues[0].startPos=start;
		sumValues[1]=maxGridSum(start,sizeN+1);
		sumValues[2]=maxGridSum(points(start.row,start.col+1),sizeN);
		sumValues[3]=maxGridSum(points(start.row+1,start.col),sizeN);
		retVal max=sumValues[0];

		for(int i=0;i<4;i++){
			if(sumValues[i].val>max.val && sumValues[i].size!=-1){
				max=sumValues[i];
			}
		}
		return max;
	}
}
int ** Q5::makeSum(int ** pGrid,int sizeN){

	for(int i=0;i<sizeN;i++){
		for(int j=0;j<sizeN;j++){
			pGrid[i][j]=pGrid[i][j];
			if(i-1>=0){
				pGrid[i][j]+=pGrid[i-1][j];
			}
			if(j-1>=0){
				pGrid[i][j]+=pGrid[i][j-1];
			}
			if(i-1>=0 && j-1>=0){
				pGrid[i][j]-=pGrid[i-1][j-1];
			}
		}
	}
	return pGrid;
}
void Q5::printGrid(int ** arr,int sizeN){
	for(int i=0;i<sizeN;i++){
		for(int j=0;j<sizeN;j++){
			cout << arr[i][j] << " ";
		}
		cout << endl;
	}
}
void Q5::printResult(retVal temp){ //temp output
	cout << "Max Sum : " << temp.val << endl;
	cout << "Start Point  : (" << temp.startPos.row << "," << temp.startPos.col <<")" <<endl;
	cout << "End   Point  : (" << temp.startPos.row+temp.size-1 << "," << temp.startPos.col+temp.size-1 <<")" <<endl;
	cout << "Size of Square : " << temp.size << endl;
}
int main(){
	cout << "Question 5" << endl;
	int option;
	
	cout << "Choose Option: 1 for Random Array, 2 for Input, 3 for Custom Array" << endl;
	cin >> option;
	
	if(option==1){
		int size;
		cout << "Input Size: " << endl;
		cin >> size;
		Q5 obj(size);
	}
	else if(option==2){
		Q5 obj;
	}
	else if(option==3){
		int size=0;
		int * array =NULL;
		Q5 obj(array,size);
	}
	
	return 0;
}