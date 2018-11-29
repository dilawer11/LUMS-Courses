#include <iostream>
#include <fstream>
#include <vector>
#include <limits>
#include <iterator>

using namespace std;
ifstream fin;
ofstream fout;

class Q2{
private:
	int optimalCut(vector<int> cuts,vector<int> set);
	void printVector(vector<int> lst);
	vector<int> ordering;
	vector<int> removeElem(vector<int> set,int i);
	vector<int> addSorted(vector<int> cuts, int val);
	int findPos(vector<int> cuts,int cutPos);
public:
	Q2(string fileName);
};
Q2::Q2(string fileName){
	fin.open(fileName);
	string waste;
	int n;
	int temp;
	vector<int> initialSet;
	fin >> waste >> n;
	fin >> waste;
	while(fin >> temp){
		initialSet.push_back(temp);
	}
	printVector(initialSet);
	vector<int> cuts;
	cuts.push_back(0);
	cuts.push_back(n);
	int val = optimalCut(cuts,initialSet);
	cout << "Here : " << val << endl;
	printVector(ordering);
	fin.close();
}
int Q2::findPos(vector<int> cuts,int cutPos){
	for(int i=0;i<cuts.size();i++){
		if(cutPos<cuts[i]){
			return i;
		}
	}

	return -1;
}
vector<int> Q2::addSorted(vector<int> cuts, int val){
	cuts.push_back(val);
	int counter = cuts.size()-1;
	while(cuts[counter-1]>cuts[counter]){
		int temp = cuts[counter];
		cuts[counter]=cuts[counter-1];
		cuts[counter-1]=temp;
		counter--;
	}
	// cout << "-------" << endl;
	// for(int i=0;i<cuts.size();i++){
	// 	cout << cuts[i] << " ";
	// }

	// cout <<endl<< "+++++++" << endl;
	return cuts;
}
vector<int> Q2::removeElem(vector<int> set,int i){
	set[i]=set[set.size()-1];
	set.pop_back();
	return set;
}
int Q2::optimalCut(vector<int> cuts,vector<int> set){
	// cout << "Called" << endl;
	// printVector(cuts);
	// printVector(set);
	if(set.size()==1){
		int pos = findPos(cuts,set[0]);
		return cuts[pos]-cuts[pos-1];

	}
	else{
		int min;
		int minIndex;
		int minCost;
		for(int i=0;i<set.size();i++){
			int pos = findPos(cuts,set[i]);
			int newCost=cuts[pos]-cuts[pos-1];
			int val = optimalCut(addSorted(cuts,set[i]),removeElem(set,i));
			if(i==0){
				min=val;
				minIndex=i;
				minCost=newCost;
			}
			else if(val<min){
				min=val;
				minIndex=i;
				minCost=newCost;
			}

		}
		return minCost+min;
		
	}
}
void Q2::printVector(vector<int> lst){
	vector<int> :: iterator it;
	for(it=lst.begin();it!=lst.end();++it){
		cout << *it << " " ;
	}
	cout << endl;
}
int main(){
	cout << "Question 2" << endl;
	Q2 obj("Q2TestCase.txt");
	return 0;
}