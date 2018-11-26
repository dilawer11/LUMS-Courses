#include <iostream>
#include <fstream>
#include <string>

using namespace std;

ifstream fin;
ofstream fout;
class Q4{
private:
	int lastCharTaken;
	string first;
	string second;
	string seq;
	string answer;
	bool check(int indexF,int indexS,int indexI);
public:
	Q4(string fileName);
};
Q4::Q4(string fileName){
	fin.open(fileName);
	answer="";
	lastCharTaken=0;
	fin >> first >> second >> seq;
	fin.close();
	bool temp = false;
	if(first.length()+second.length()==seq.length()){
		temp=check(0,0,0);
	}
	if(!temp){
		cout << "NOT VALID" << endl;
	}
	else{
		cout << "VALID" << endl;
		cout << answer << endl;
	}
	
}
bool Q4::check(int indexF,int indexS,int indexI){
	if(indexI==seq.length()){
		return true;
	}
	else if(first[indexF] != seq[indexI] && second[indexS] != seq[indexI]){
		return false;
	}
	else{
		if(first[indexF]==seq[indexI]){
			if(lastCharTaken==1){
				answer.append(1,first[indexF]);
			}
			else{
				lastCharTaken=1;
				answer+= " S1";
				answer.append(1,first[indexF]);
			}
			return check(indexF+1,indexS,indexI+1);
		}
		else{
			if(lastCharTaken==2){
				answer.append(1,second[indexS]);
			}
			else{
				lastCharTaken=2;
				answer+= " S2";
				answer.append(1,second[indexS]);
			}
			return check(indexF,indexS+1,indexI+1);
		}
	}
}
int main(){
	cout << "Question 4" << endl;
	Q4 obj("Q4TestCase.txt");
	return 0;
}