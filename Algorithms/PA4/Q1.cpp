#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ifstream fin;
ofstream fout;
class Q1{
public:
	Q1(string fileName);
	~Q1();
private:
	int maxProduction(int i);
	int n;
	int* high;
	int* low;
	string* memo;

};
Q1::Q1(string fileName){
	fin.open(fileName);
	string temp;
	fin >> temp >> n;
	high = new int[n];
	low = new int[n];
	memo = new string[n];
	for(int i=0;i<n;i++){
		high[i]=0;
		low[i]=0;
		string[i]="";
	}
	fin >> temp;
	for(int i=0;i<n;i++){
		fin >> high[i];
	}
	fin >> temp;
	for(int i=0;i<n;i++){
		fin >> low[i];
	}
	fin.close();
	cout << "Total Revenue: " << maxProduction(n) << endl;
}
Q1::~Q1(){
	if(high)delete [] high;
	if(low)delete [] low;
	if(memo)delete [] memo;
}
int maxProduction(int i){
	if(i==0){
		return 0;
	}
	else if (memo[i-1]!=""){
		cout << memo[i-1] << endl;
		int spacePos = memo[i-1].find(" ");
		int retVal = stoi(memo[i-1].substr(spacePos+1,memo[i-1].length()-1-spacePos));
		return retVal;
	}
	else if(i==1){
		if(high[0]>=low[0]){
			cout << "Week 1: High " << high[0] << endl;
			return high[0];
		}
		else{
			cout << "Week 1: Low " << low[0] << endl;
			return low[0];
		}
	}
	else{
		int rh = high[i-1]+maxProduction(i-2);
		int rl = low[i-1]+maxProduction(i-1);
		if(rh>rl){
			cout << "Week " << i << ": High " << high[i-1] << endl;
			memo[i-1] = "High "+to_string(high[i-1]);
			return rh;
		}
		else{
			cout << "Week " << i << ": Low " << low[i-1] << endl;
			memo[i-1] = "High "+to_string(low[i-1]);
			return rl;
		}

	}
}
int main(){
	cout << "Question 1" << endl
	Q1 obj("q1TestCase.txt");
	return 0;
}