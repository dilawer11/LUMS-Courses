/*
Data Structures : Arrays  (Vectors) of Custom type (SetVal) Pair
How it works : It works sort of like merge sort except there is no creteria to sort images so what it does is it divides till base case then 
if two images are same it merges to form one with increased value in size
Running Time : O(nlogn)
Space Complexity : O(n)
Recurrence Relation :  T(2) = b 
T(n) = 2T(n/2)+n;
*/
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
ifstream fin;
ofstream fout;

struct SetVal{
	int size;
	int type;
};


class Q5{
private:
	int n;
	int m;
	vector<SetVal> recur(vector<SetVal> set);
	vector<SetVal> merge(vector<SetVal> set1,vector<SetVal> set2);
	bool decode(int type1,int type2);
	void printDataSet(vector<SetVal> set,string label);
	

public:
	Q5(int photos,int species);
	~Q5();
};
Q5::Q5(int photos,int species){
	n=photos;
	m=species;
	srand(time(0));
	vector<SetVal>initialSet;
	for(int i=0;i<n;i++){
		SetVal temp;
		temp.size=1;
		temp.type=rand()%m;
		initialSet.push_back(temp);
	}
	printDataSet(initialSet,"Initial Data Set");
	vector<SetVal> final = recur(initialSet);
	bool found=false;

	for(int i=0;i<final.size();i++){
		if(final[i].size>(n/2)){
			cout << "SUCCESS T:"<<final[i].type << endl;
			found=true;
			break;
		}
	}
	if(!found){
		cout << "FAILURE" << endl;
	}
	printDataSet(final,"Final Data Set");
}
Q5::~Q5(){

}
vector<SetVal> Q5::recur(vector<SetVal> set){
	if(set.size()==1){
		return set;
	}
	else{
		vector<SetVal> set1;
		vector<SetVal> set2;
		int newSize=set.size()/2;
		for(int i=0;i<newSize;i++){
			set1.push_back(set[i]);
		}
		for(int i=newSize;i<set.size();i++){
			set2.push_back(set[i]);
		}
		return merge(recur(set1),recur(set2));
	}
}
vector<SetVal> Q5::merge(vector<SetVal> set1,vector<SetVal> set2){
	
	for(int i=0;i<set1.size();i++){
		bool found=false;
		for(int j=0;j<set2.size();j++){
			if(decode(set1[i].type,set2[j].type)){
				set2[j].size+=set1[i].size;
				found=true;
			}
		}
		if(!found){
			set2.push_back(set1[i]);
		}
	}
	return set2;
}
bool Q5::decode(int type1,int type2){
	if (type1==type2){
		return true;
	}
	else{
		return false;
	}
}
void Q5::printDataSet(vector<SetVal> set,string label){
	cout << label<< ": [ ";
	for(int i=0;i<set.size()-1;i++){
		cout <<set[i].type << ", ";
	}
	cout << set[set.size()-1].type << " ]"<< endl;
}
int main(){
	Q5 obj(10,2);
	return 0;
}


