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
		memo[i]="";
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
	int val = maxProduction(n);
	cout << "Total Revenue: " << val << endl;
}
Q1::~Q1(){
	if(high)delete [] high;
	if(low)delete [] low;
	if(memo)delete [] memo;
}
int Q1::maxProduction(int i){
	if(i==0){
		return 0;
	}
	else if (memo[i-1]!=""){
		int spacePos = memo[i-1].find(" ");
		int retVal = stoi(memo[i-1].substr(spacePos+1,memo[i-1].length()-1-spacePos));

		return retVal;
	}
	else if(i==1){
		if(high[0]>=low[0]){
			memo[0]="High "+to_string(high[0]);
			cout << "Week 1: High " << high[0] << endl;

			return high[0];
		}
		else{
			memo[0]="Low "+to_string(low[0]);
			cout << "Week 1: Low " << low[0] << endl;
			return low[0];
		}
	}
	else{
		int rh = high[i-1]+maxProduction(i-2);
		int rl = low[i-1]+maxProduction(i-1);
		int max = rl;
		if(rh>rl){
			memo[i-1] = "High "+to_string(rh);
			max=rh;
		}
		else{
			memo[i-1] = "Low "+to_string(rl);
		}
	
		return max;

	}
}
int main(){
	cout << "Question 1" << endl;
	Q1 obj("Q1TestCase.txt");
	return 0;
}