#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ifstream fin;
ofstream fout;

struct retVal{
	string prevVals;
	int maxVal;
	retVal(){
		prevVals="";
		maxVal=-1;
	}
};
class Q1{
public:
	Q1(string fileName);
	~Q1();
private:
	retVal maxProduction(int i);
	int n;
	int* high;
	int* low;
	retVal* memo;

};
Q1::Q1(string fileName){
	fin.open(fileName);
	string temp;
	fin >> temp >> n;
	high = new int[n];
	low = new int[n];
	memo = new retVal[n];
	for(int i=0;i<n;i++){
		high[i]=0;
		low[i]=0;
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
	retVal val = maxProduction(n);
	cout << val.prevVals;
	cout << "Total Revenue: " << val.maxVal << endl;
}
Q1::~Q1(){
	if(high)delete [] high;
	if(low)delete [] low;
	if(memo)delete [] memo;
}
retVal Q1::maxProduction(int i){
	if(i==0){
		retVal ret;
		ret.prevVals="";
		ret.maxVal=0;
		return ret;
	}
	else if (memo[i-1].maxVal!=-1){
		return memo[i-1];
	}
	else if(i==1){
		retVal ret;
		if(high[0]>=low[0]){
			ret.prevVals = "Week 1: High " + to_string(high[0])+"\n";
			ret.maxVal=high[0];
		}
		else{
			ret.prevVals = "Week 1: Low " + to_string(low[0])+"\n";
			ret.maxVal=low[0];
		}
		memo[i-1]=ret;
		return ret;
	}
	else{
		retVal rh = maxProduction(i-2);
		retVal rl = maxProduction(i-1);
		if(rh.maxVal+high[i-1]>rl.maxVal+low[i-1]){
			rh.prevVals += "Week "+to_string(i-1)+": Priming\nWeek " +to_string(i)+": High "+to_string(high[i-1])+"\n";
			rh.maxVal+=high[i-1];
			memo[i-1]=rh;
			return rh;
		}
		else{
			rl.prevVals += "Week " +to_string(i)+": Low "+to_string(low[i-1])+"\n";
			rl.maxVal+=low[i-1];
			memo[i-1]=rl;
			return rl;
		}
	}
}
int main(){
	cout << "Question 1" << endl;
	Q1 obj("Q1TestCase.txt");
	return 0;
}