/*
How it works: It makes two preference lists based on the input (one for the teams and one for the locations). Then it runs gale shapley on them and returns the stable matching result
Space Complexity is O(n^2)
Time Complexity is also O(n^2)
*/
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
ifstream fin;
ofstream fout;

struct Pair{
	int location;
	int team;
};

class Q2{
private:
	int n;
	void print2DArray(int ** grid);
	void makePrefLists(vector<int> * input);
	Pair * makeStableMatching(int ** locationPrefList,int ** teamPrefList);
	bool preffered(int a,int b,int * list,int sze);
	Pair * finalDestination;
	void printPairs(Pair * pairs,int sze);
public:
	Q2(string fileName);
	~Q2();
	void run();
	

};

Q2::Q2(string fileName){
	fin.open(fileName);
	string temp;
	fin >> temp >> n;
	vector<int> * rawInput = new vector<int>[n];


	int counterLine=-1;
	
	while(fin>>temp){
		if(temp[0]=='T'){
			counterLine++;
		}
		else if(temp[0]=='L'){
			string valString;
			if(temp[temp.length()-1]==','){
				valString=temp.substr(1,temp.length()-2);
			}
			else{
				valString=temp.substr(1,temp.length()-1);
			}
			int valInt = stoi(valString)-1;

			rawInput[counterLine].push_back(valInt);
		}
		else{
			rawInput[counterLine].push_back(-1);
		}
	}
	fin.close();
	// for(int i=0;i<n;i++){
	// 	for(int j=0;j<rawInput[i].size();j++){
	// 		cout<<rawInput[i][j]+1<<" ";
	// 	}
	// 	cout <<endl;
	// }
	makePrefLists(rawInput);
}
Q2::~Q2(){}
void Q2::print2DArray(int ** grid){
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			cout << grid[i][j]+1 << " ";
		}
		cout << endl;
	}
}
void Q2::makePrefLists(vector<int> * input){
	int ** locationPrefList=new int *[n];
	int ** teamPrefList=new int *[n];
	int * prefCount = new int [n];
	for(int i=0;i<n;i++){
		locationPrefList[i]=new int [n];
		teamPrefList[i]= new int[n];
		prefCount[i] = 0;
	}

	for(int i=0;i<n;i++){
		int counter=0;
		for(int j=input[i].size()-1;j>=0;j--){
			if(input[i][j]!=-1){
				teamPrefList[i][counter]=input[i][j];
				counter++;
			}
		}
	}
	//print2DArray(teamPrefList);

	for(int j=0;j<input[0].size();j++){
		for(int i=0;i<n;i++){
			if(input[i][j]!=-1){
				int val = input[i][j];
				locationPrefList[val][prefCount[val]]=i;
				prefCount[val]++;
			}
		}
	}
	//cout << endl;
	//print2DArray(locationPrefList);
	finalDestination =makeStableMatching(locationPrefList,teamPrefList);
}
bool Q2::preffered(int a,int b,int * list,int sze){
	for(int i=0;i<sze;i++){
		if(list[i]==a){
			return true;
		}
		else if(list[i]==b){
			return false;
		}
	}
	cout << "Unexpected preffered List" << endl;
	return false;
}
Pair * Q2::makeStableMatching(int ** locationPrefList,int ** teamPrefList){
	int * currentTeamList;
	int * currentLocationNumber;
	currentTeamList=new int[n];
	currentLocationNumber=new int[n];
	for(int i=0;i<n;i++){
		currentLocationNumber[i]=0;
		currentTeamList[i]=-1;
	}

	for(int j=0;j<n;j++){
		for(int i=0;i<n;i++){ //for each plane
			int airportToGo=teamPrefList[i][currentLocationNumber[i]];
			if(currentTeamList[airportToGo]!=i){
				if(currentTeamList[airportToGo]==-1){
					currentTeamList[airportToGo]=i;
				}
				else if (preffered(i,currentTeamList[airportToGo],locationPrefList[airportToGo],n)){ //more preffered
					currentLocationNumber[currentTeamList[airportToGo]]++;
					currentTeamList[airportToGo]=i;
				}
				else{
					currentLocationNumber[i]++;
				}
			}
		
		}
	}
	// for(int i=0;i<n;i++){
	// 	cout <<"P" <<i+1 << "-" << "A"<<currentTeamList[i]+1 << "  ";
	// }
	// cout << endl;
	Pair * matchingPairs = new Pair[n];
	for(int i=0;i<n;i++){
		matchingPairs[i].team=i;
		matchingPairs[i].location=currentTeamList[i];
	}
	delete [] currentTeamList;
	delete [] currentLocationNumber;
	return matchingPairs;
}
void Q2::run(){
	printPairs(finalDestination,n);
};
void Q2::printPairs(Pair * pairs,int sze){
	for(int i=0;i<sze;i++){
		cout <<"L" <<pairs[i].team+1 << "-"<< "T"<<pairs[i].location+1<< "  " ;
	}
	cout <<endl;
}

int main(){
	Q2 obj("q2 test case.txt");
	obj.run();
	return 0;
}
