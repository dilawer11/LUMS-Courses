#include <iostream>
#include <vector>
#include <fstream>

using namespace std;
ifstream fin;
ofstream fout;
class Q3{
private:
	int k;
	int n;
	vector<int> S1;
	vector<int> S2;
	vector<int> initialSet;
	vector<int> exclude(int i,vector<int> set);
	int printVector(vector<int> vec);
public:
	Q3(string fileName);
	bool setDifference(int sum,vector<int> set);

};
Q3::Q3(string fileName){
	fin.open(fileName);
	string temp;
	fin >> temp >> k;
	fin >> temp;
	int raw;
	
	while(fin>>raw){
		initialSet.push_back(raw);
	}
	if(setDifference(0,initialSet)){
		cout << "POSSIBLE" << endl;
		int sum1=printVector(S1);
		int sum2=printVector(S2);
		cout << "Difference: " << sum1 << "-" << sum2 << " = " << sum1-sum2 << endl;
	}
	else{
		cout << "NOT POSSIBLE" << endl;
	}



}
bool Q3::setDifference(int sum,vector<int> set){
	if(set.size()==1){
		if(sum+set[0]==k){
			S1.push_back(set[0]);
			return true;
		}
		else if(sum-set[0]==k){
			S2.push_back(set[0]);
			return true;
		}
		return false;
	}
	else{
		for(int i=0;i<set.size();i++){
			if(setDifference(sum+set[i],exclude(i,set))){
				S1.push_back(set[i]);
				return true;
			}
			else if(setDifference(sum-set[i],exclude(i,set))){
				S2.push_back(set[i]);
				return true;
			}
			else{
				return false;
			}
		}
	}

}
vector<int> Q3::exclude(int i,vector<int> set){
	set[i]=set[set.size()-1];
	set.pop_back();
	return set;
}

int Q3::printVector(vector<int> vec){
	int sum = 0;
	for(int i=0;i<vec.size();i++){
		cout << vec[i] << " ";
		sum+=vec[i];
	}
	cout << "sum=" << sum<<endl;
	return sum;
}
int main(){
	cout << "Question 3" << endl;
	Q3 obj("Q3TestCase.txt");
	return 0;
}