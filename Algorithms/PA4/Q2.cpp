#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
ifstream fin;
ofstream fout;

class Q2{
private:
	vector<int> optimalCutOrdering(int cost,vector<int> set);
	void printVector(vector<int> lst);
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
	fin.close();
}
vector<int> Q2::optimalCutOrdering(int cost,vector<int> set){
	int max=0;
	for(int i=0;i<set.size();i++){
		optimalCutOrdering()
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