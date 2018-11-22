/*
Data Structures: A binary tree along with 2 arrays
How it works: Everynode except the leaf node returns the current max pair it has in its subchildren and current max single value and adds it own value to it.
Running Time : O(n)
Space Complexity : O(n)
Recurrence Relation : T(1) = weight
T(n) = max{T(LC),T(RC)} + weight   
LC = 2n+1 RC = 2n+2
*/
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;
ofstream fout;
ifstream fin;

struct treeNode{
	int weight;
	string name;
	treeNode * LC; //leftChild
	treeNode * RC;	//rightChild
};
struct cSet{
	string val1;
	string val2;
	int currentWeight;
};
class Q4{
private:
	treeNode * root;
	treeNode* makeTreeFromArray(int * weights,string * names,int size,int index);
	vector<cSet> maxPair(treeNode * node);
	void vecPrint(vector<cSet> temp);
	void delNode(treeNode * node);
	int n;
public:
	Q4(string fileName);
	~Q4();
	void printTree(treeNode * node);
	void run();

};
Q4::Q4(string fileName){
	//read file make two arrays and pass it to make tree function
	fin.open(fileName);
	string temp;
	fin >> temp >> n;
	int * initWeights= new int [n];
	string * nameTags=new string[n];
	int posSem,posColon;
	string strName,strWeight;
	getline(fin,temp);
	for(int i=0;i<n;i++){
		getline(fin,temp);
		posSem=temp.find(";");
		posColon=temp.find(":");
		strName=temp.substr(0,posSem);
		strWeight=temp.substr(posSem+1,posColon-posSem-1);
		nameTags[i]=strName;
		initWeights[i]=stoi(strWeight);
	}
	fin.close();




	
	root = makeTreeFromArray(initWeights,nameTags,n,0);
	vector<cSet> final = maxPair(root);

	delete[] initWeights;
	delete[] nameTags; 

}
Q4::~Q4(){
	// cout << "Des Called" <<endl;
	delNode(root);
}
void Q4::delNode(treeNode * node){
	if(node){
		delNode(node->LC);
		delNode(node->RC);
		delete node;
	}
}
treeNode * Q4::makeTreeFromArray(int * weights,string * names,int size,int index){
	if(index>=size){
		return 0;
	}
	else{
		treeNode * temp = new treeNode;
		temp->weight=weights[index];
		temp->name=names[index];
		int twoN = 2*index;
		temp->LC=makeTreeFromArray(weights,names,size,twoN+1);
		temp->RC=makeTreeFromArray(weights,names,size,twoN+2);
		return temp;
	}
}

void Q4::printTree(treeNode * node){
	if(node){
		printTree(node->LC);
		cout << node->name << "->"<<node->weight <<endl;
		printTree(node->RC);
	}
	else{
		return;
	}
}
vector<cSet> Q4::maxPair(treeNode * node){
	if(node->LC==0 && node->RC==0){
		cSet temp;
		temp.val1=node->name;
		temp.val2="";
		temp.currentWeight=node->weight;
		vector<cSet> ret;
		ret.push_back(temp);
		// vecPrint(ret);
		return ret;
	}
	else{
		vector<cSet> vecL=maxPair(node->LC);
		vector<cSet> vecR=maxPair(node->RC);
		vector<cSet> pairSet;
		if(vecL.size()>1){
			pairSet.push_back(vecL[1]);
		}
		if(vecR.size()>1){
			pairSet.push_back(vecR[1]);
		}
		cSet temp ;
		temp.val1=vecL[0].val1;
		temp.val2=vecR[0].val1;
		temp.currentWeight=vecR[0].currentWeight+vecL[0].currentWeight;
		pairSet.push_back(temp);
		cSet maxPair =pairSet[0];
		cSet maxSingle;
		for(int i=0;i<pairSet.size();i++){
			if(pairSet[i].currentWeight>maxPair.currentWeight){
				maxPair=pairSet[i];
			}
		}
		if(vecL[0].currentWeight>vecR[0].currentWeight){
			maxSingle=vecL[0];
		}
		else{
			maxSingle=vecR[0];
		}
		vector<cSet> ret;
		maxPair.currentWeight+=node->weight;
		maxSingle.currentWeight+=node->weight;
		ret.push_back(maxSingle);
		ret.push_back(maxPair);
		// vecPrint(ret);
		return ret;
	}
}
void Q4::vecPrint(vector<cSet> temp){
	cout << "--" << endl;
	for(int i=0;i<temp.size();i++){
		
		cout <<temp[i].val1 << " " <<temp[i].val2 << " "<<temp[i].currentWeight << endl;
	}
}
void Q4::run(){
	vector<cSet> final = maxPair(root);
	cout << "[ "<<final[1].val1 << ", " << final[1].val2 << " ]" << " weight = " << final[1].currentWeight <<endl;
}
int main(){
	Q4 obj("q4 test case 2.txt");
	obj.run();

	return 0;
}
