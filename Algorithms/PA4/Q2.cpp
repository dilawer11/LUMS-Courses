#include <iostream>

using namespace std;
ifstream fin;
ofstream fout;
class Q2{
private:
	int k
	int n;
	vector<int> S1;
	vector<int> S2;
	vector<int> initialSet;
public:
	Q2(string fileName);
	~Q2();
	int setDifference(int n);


};
Q2::Q2(string fileName){
	fin.open(fileName);
	string temp;
	fin >> temp >> k;
	fin >> temp;
	int raw;
	
	while(fin>>raw){
		initialSet.push_back(raw);
	}



}
Q2::~Q2(){

}
int Q2::setDifference(int n){
	if(n==0){
		return 0;
	}
	else{
		
	}
}
int main(){
	return 0;
}