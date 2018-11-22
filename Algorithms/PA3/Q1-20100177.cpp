/*
How it works: 
Running Time Complexity is 0(n^2)
Space Compexity is also O(n^2)
*/

#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>

using namespace std;
ifstream fin;
ofstream fout;

struct Pair{
	int plane;
	int airport;
};


class Q1{
public:
	Q1(string fileName);
	~Q1();
	void run();
private:
	int n;
	vector<Pair> initialSetX;
	int ** planePrefList;
	int ** airportPrefList;
	Pair * makeStableMatching();
	bool preffered(int a,int b,int * list,int sze);
	Pair * removeSetXPairs(Pair * currentPairs);
	void print2DArray(int ** arr,int sze);
	void printPairs(Pair * pairs,int sze);
	int findPlaneIndex(int ap,Pair* pairs);
	int findReplacement(int pln,Pair * pairs);
	int findAirport(int pln,Pair * pairs);
	int  findPlane(int ap,Pair * pairs);
	bool placementPossible(int pln,int airport);
	bool pairEqual(Pair pair1, Pair pair2);

};

Q1::Q1(string fileName){
	fin.open(fileName);
	string temp;
	fin >> temp >> n;
	planePrefList=new int * [n];
	for(int i=0;i<n;i++){
		fin>>temp;
		planePrefList[i]=new int[n];
		for(int j=0;j<n;j++){
			string valString;
			int val;
			fin >> valString;
			if (j==n-1){
				val=stoi(valString.substr(1,valString.length()))-1;
			}
			else{
				val=stoi(valString.substr(1,valString.length()-1))-1;
			}
			planePrefList[i][j]=val;
		}
	}
	airportPrefList=new int *[n];
	for(int i=0;i<n;i++){
		fin>>temp;
		airportPrefList[i]=new int[n];
		for(int j=0;j<n;j++){
			string valString;
			int val;
			fin >> valString;
			if (j==n-1){
				val=stoi(valString.substr(1,valString.length()))-1;
			}
			else{
				val=stoi(valString.substr(1,valString.length()-1))-1;
			}
			airportPrefList[i][j]=val;
		}
	}
	fin >> temp;
	string splane,sairport;
	while (fin>>splane){
		splane=splane.substr(1,splane.length());
		fin >> sairport;
		sairport=sairport.substr(1,sairport.length());
		
		if(sairport[sairport.length()-1]==','){
			sairport=sairport.substr(0,sairport.length()-1);

		}

		Pair tempPair;
		tempPair.plane=stoi(splane)-1;
		tempPair.airport=stoi(sairport)-1;
		initialSetX.push_back(tempPair);

	}
	// print2DArray(planePrefList,n);
	// print2DArray(airportPrefList,n);
	// for( int i=0;i<initialSetX.size();i++){
	// 	cout <<initialSetX[i].plane << "-" << initialSetX[i].airport << " ";
	// }
	// cout << endl;
	fin.close();
}
Q1::~Q1(){
	for(int i=0;i<n;i++){
		delete [] planePrefList[i];
		delete [] airportPrefList[i];
	}
	delete [] planePrefList;
	delete [] airportPrefList;
}
bool Q1::pairEqual(Pair pair1, Pair pair2){
	if(pair1.plane==pair2.plane && pair1.airport==pair2.airport){
		return true;
	}
	else{
		return false;
	}
}
void Q1::run(){
	//make initial stable matching
	Pair * matchingPairs=makeStableMatching();
	printPairs(matchingPairs,n);
	matchingPairs=removeSetXPairs(matchingPairs);
	//deal with setX and return
	printPairs(matchingPairs,n);
	delete [] matchingPairs;
}
bool Q1::preffered(int a,int b,int * list,int sze){
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
Pair * Q1::makeStableMatching(){
	int * currentPlaneList;
	int * currentAirportNumber;
	currentPlaneList=new int[n];
	currentAirportNumber=new int[n];
	for(int i=0;i<n;i++){
		currentAirportNumber[i]=0;
		currentPlaneList[i]=-1;
	}

	for(int j=0;j<n;j++){
		for(int i=0;i<n;i++){ //for each plane
			int airportToGo=planePrefList[i][currentAirportNumber[i]];
			if(currentPlaneList[airportToGo]!=i){
				if(currentPlaneList[airportToGo]==-1){
					currentPlaneList[airportToGo]=i;
				}
				else if (preffered(i,currentPlaneList[airportToGo],airportPrefList[airportToGo],n)){ //more preffered
					currentAirportNumber[currentPlaneList[airportToGo]]++;
					currentPlaneList[airportToGo]=i;
				}
				else{
					currentAirportNumber[i]++;
				}
			}
		
		}
	}
	// for(int i=0;i<n;i++){
	// 	cout <<"P" <<i+1 << "-" << "A"<<currentPlaneList[i]+1 << "  ";
	// }
	// cout << endl;
	Pair * matchingPairs = new Pair[n];
	for(int i=0;i<n;i++){
		matchingPairs[i].plane=i;
		matchingPairs[i].airport=currentPlaneList[i];
	}
	delete [] currentPlaneList;
	delete [] currentAirportNumber;
	return matchingPairs;
}
Pair * Q1::removeSetXPairs(Pair * currentPairs){
	for(int i=0;i<n;i++){
		for (int j=0;j<initialSetX.size();j++){
			if(pairEqual(currentPairs[i],initialSetX[j])){
				int ap=findReplacement(currentPairs[i].plane,currentPairs);
				int index=findPlaneIndex(ap,currentPairs);
				currentPairs[index].airport=currentPairs[i].airport;
				currentPairs[i].airport=ap;
			}
		}
	}
	return currentPairs;
}
int Q1::findPlaneIndex(int ap,Pair* pairs){
	for(int i=0;i<n;i++){
		if(pairs[i].airport==ap){
			return i;
		}
	}
	cout << "Error" << endl;
	return -1;
}
	
int Q1::findReplacement(int pln,Pair * pairs){
	for(int i=0;i<n;i++){
		if(placementPossible(pln,i)){
			int altPlane=findPlane(i,pairs);
			int altAirport=findAirport(pln,pairs);
			if(placementPossible(altPlane,altAirport)){
				return i;
			}
		}
	}
	cout << "No Replacement Possible" << endl;
	return -1;
}
int Q1::findAirport(int pln,Pair * pairs){
	for(int i=0;i<n;i++){
		if(pln==pairs[i].plane){
			return pairs[i].airport;
		}
	}
	cout << "Unexpected Airport" << endl;
	return -1;
}
int  Q1::findPlane(int ap,Pair * pairs){
	for(int i=0;i<n;i++){
		if(ap==pairs[i].airport){
			return pairs[i].plane;
		}
	}
	cout << "Unexpected Plane" << endl;
	return -1;
}
bool Q1::placementPossible(int pln,int airport){
	Pair temp;
	temp.plane=pln;
	temp.airport=airport;
	for(int i=0;i<initialSetX.size();i++){
		if (pairEqual(initialSetX[i],temp)){
			return false;
		}
	}
	return true;
}

void Q1::print2DArray(int ** arr,int sze){
	for (int i=0;i<sze;i++){
		for(int j=0;j<sze;j++){
			cout << arr[i][j] << " ";
		}
		cout << endl;
	}
	cout << endl ;

}
void Q1::printPairs(Pair * pairs,int sze){
	for(int i=0;i<sze;i++){
		cout <<"A" <<pairs[i].plane+1 << "-"<< "P"<<pairs[i].airport+1<< "  " ;
	}
	cout <<endl;
}

int main(){
	Q1 obj("q1 test case.txt");
	obj.run();
	return 0;
}