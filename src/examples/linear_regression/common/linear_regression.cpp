


#include "linear_regression.h"
#include "../../../abycore/circuit/arithmeticcircuits.h"
#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/sharing/sharing.h"
#include "../../../examples/innerproduct/common/innerproduct.h"
#include "../../../abycore/sharing/arithsharing.h"
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

/*Transformation: from string to double*/
uint16_t stringToNum(const string& str){
	istringstream iss(str);
	uint16_t num;
	iss>>num;
	return num;
}

/*Read data. Data format:vector<vector<double>>*/
void read_data(string& filename,vector<vector<uint16_t> > &X){
	ifstream inFile(filename.c_str());
	if (inFile.fail()){
		cout<<"File not found"<<endl;
		return ;
	}
	//inFile.open(filename.c_str());
//	cout<<"file is open"<<endl;
	string lineStr;
//	int i=0;
	while(getline(inFile,lineStr)){
//		cout<<"handling every row"<<endl;
		stringstream ss(lineStr);
//		cout<<"stringstream"<<endl;
		string str;
//		cout<<"str"<<endl;
		vector<uint16_t> lineArray;
//		cout<<"a variable for storing a row"<<endl;
		while(getline(ss,str,',')){
			lineArray.push_back(stringToNum(str));
//			cout<<"a row has been put in"<<endl;			
		}
//		i++;
		X.push_back(lineArray);
//		cout<<"put"<<i<<endl;
	}
//	cout<<"already done!"<<endl;
	inFile.close();
}

/*Read label. Data format:vector<double>*/
void read_label(string &filename, vector<uint16_t> &X){
        ifstream inFile(filename.c_str());
	if(inFile.fail()){
		cout<<"File not found"<<endl;
		return;
	}
//	cout<<"Found the file"<<endl;
        string lineStr;
//	cout<<"lineStr"<<endl;
	int i=0;
        while(getline(inFile,lineStr)){
//		cout<<"can read"<<endl;
                stringstream ss(lineStr);
//		cout<<"stringstream"<<endl;
                string str;
                while(getline(ss,str,',')){
//			cout<<"str "<<str<<endl;
                        X.push_back(stringToNum(str));
//			cout<<"the"<<i<<"th fig:"<<X[i]<<endl;
			i++;
		}
        }
	inFile.close();
}


uint16_t* get_y_col(vector<uint16_t> &Y){
	uint16_t* col=new uint16_t[Y.size()];
	int i;
	for(i=0;i<Y.size();i++)
		col[i]=Y[i];
	return col;
}

uint16_t* get_y_row(uint16_t **Y,int index){

        int i;
        uint16_t* row=new uint16_t[10];
        for(i=0;i<10;i++){
                row[i]=Y[index][i];

        }
        return row;
}

uint16_t* get_col(vector<vector<uint16_t> > &X,uint32_t index){
	uint16_t* col=new uint16_t[X.size()];
	int i;
	for(i=0;i<X.size();i++)
		col[i]=X[i][index];
	return col;
}
uint16_t* get_row(vector<vector<uint16_t> > &X,uint32_t index){
	uint16_t* row=new uint16_t[X[0].size()];
	int i;
	for(i=0;i<X[0].size();i++){
		row[i]=X[index][i];	
	}
	return row;
}

/*
share* get_share_row(share **W,uint32_t index){
	uint32_t i,j;
	share* row=(share*)malloc(sizeof(share)*100);
	for(i=0;i<100;i++){
		row[i]=matrix[index][i];
	}
	return row;
}*/



int32_t test_linear_regression_circuit(e_role role,const std::string& address,uint16_t port,seclvl seclvl,uint32_t bitlen,uint32_t nthreads,e_mt_gen_alg mt_alg,e_sharing dstsharing,e_sharing minsharing,uint32_t n,bool only_yao){
	/**
		Step 1: Create the ABYParty object which defines the basis of all the
		 	 	operations which are happening.	Operations performed are on the
		 	 	basis of the role played by this object.
	*/
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,mt_alg);
	/**
		Step 2: Get to know all the sharing types available in the program.
	*/
	std::vector<Sharing*>& sharings = party->GetSharings();
	/**
		Step 3: Create the circuit object on the basis of the sharing type
				being inputed.
	*/

	ArithmeticCircuit* circ=(ArithmeticCircuit*) sharings[dstsharing]->GetCircuitBuildRoutine();


        /**
		Step 4:  Get the original valus of X,Y,W for respective parties.
				
	*/
	vector<vector<uint16_t> > X_train,X_test;
	vector<uint16_t> Y_train,Y_test;
	
	string filename1="/home/liujing/ABY/src/examples/datasets/mini_mnist/train_features.csv";
	string filename2="/home/liujing/ABY/src/examples/datasets/mini_mnist/test_features.csv";
	string filename3="/home/liujing/ABY/src/examples/datasets/mini_mnist/train_labels.csv";
	string filename4="/home/liujing/ABY/src/examples/datasets/mini_mnist/test_labels.csv";

	read_data(filename1,X_train);
	read_data(filename2,X_test);
	read_label(filename3,Y_train);
	read_label(filename4,Y_test);


	/**
		Step 5: Partition data for respective parties.Initialize W with zero.
	*/
	uint32_t i,j;
	uint32_t rownumX=100;
	uint32_t colnumX=392;
	uint32_t rownumY=100;
	uint32_t colnumY=10;
	uint32_t rownumW=392;
	uint32_t colnumW=10;

	
	vector<vector<uint16_t> > X_1,X_2;
	for(i=0;i<100;i++){
		vector<uint16_t> line1,line2;
		for(j=0;j<392;j++){
			line1.push_back(X_train[i][j]);
			line2.push_back(X_train[i][392+j]);
		}
		X_1.push_back(line1);
		X_2.push_back(line2);
	}

	vector<vector<uint16_t> > W(392,vector<uint16_t>(10,0));
	/**
	 * 	Step 6: Copy the original values into the respective share
	 * 		objects using the circuit object method.
	 * 		Call the build method for building the circuit
	 * 		for the problem by passing the shared objects 
	 * 		and circuit object.
	 * */

	/*one-hot编码*/
	uint16_t **trainY;
	uint16_t **testY;
	trainY=new uint16_t*[100];
	testY=new uint16_t*[10];
	for(i=0;i<100;i++)
		trainY[i]=new uint16_t[10];
	for(i=0;i<10;i++)
		testY[i]=new uint16_t[10];

	for(i=0;i<100;i++){
		for(j=0;j<10;j++){
			if((j+1)==Y_train[i]){
				trainY[i][j]=1;
			}else{
				trainY[i][j]=0;
			}
		}
	}

	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			if((j+1)==Y_test[i]){
				testY[i][j]=1;
			}else{
				testY[i][j]=0;
			}
		}
	}

	cout<<"Initialization done!\n";
	
	share ***XW,***XW1,***XW2;
	share *s_x1_vec,*s_x2_vec,*s_w_vec;
	share *s_w1_vec,*s_w2_vec;
	share *s_out1,*s_out2,*s_out;
	uint16_t output=0;

	XW=(share***)malloc(sizeof(share**)*rownumX);
	XW1=(share***)malloc(sizeof(share**)*rownumX);
	XW2=(share***)malloc(sizeof(share**)*rownumX);

	//Compute XW1 100*10
	cout<<"XW1\n";
	for(i=0;i<rownumX;i++){
		XW1[i]=(share**)malloc(sizeof(share*)*colnumW);
		s_x1_vec=circ->PutSIMDINGate(colnumX,get_row(X_1,i),16,SERVER);
		//cout<<"X["<<i<<"]\n";
		for(j=0;j<colnumW;j++){
			s_w1_vec=circ->PutSIMDINGate(rownumW,get_col(W,j),16,SERVER);
			s_out1=BuildMatmulProductCircuit(s_x1_vec,s_w1_vec,colnumX,rownumW,(ArithmeticCircuit*)circ);
			XW1[i][j]=s_out1;
		}
	}

	cout<<"XW2\n";
	//Compute XW2
	for(i=0;i<rownumX;i++){
		XW2[i]=(share**)malloc(sizeof(share*)*colnumW);
		s_x2_vec=circ->PutSIMDINGate(colnumX,get_row(X_2,i),16,CLIENT);
		//cout<<"X2["<<i<<"]\n";
		for(j=0;j<colnumW;j++){
			s_w2_vec=circ->PutSIMDINGate(rownumW,get_col(W,j),16,CLIENT);
			s_out2=BuildMatmulProductCircuit(s_x2_vec,s_w2_vec,colnumX,rownumW,(ArithmeticCircuit*)circ);
			XW2[i][j]=s_out2;
		}
	}

	cout<<"Y*\n";
	//Y*=XW1+XW2
	for(i=0;i<rownumX;i++){
		XW[i]=(share**)malloc(sizeof(share*)*colnumW);
		for(j=0;j<colnumW;j++){
			s_out=circ->PutADDGate(XW1[i][j],XW2[i][j]);
			//circ->PutPrintValueGate(s_out,"s_out");
			XW[i][j]=s_out;
		}
	}

	cout<<"delta\n";
	//DELTA=Y*-Y
	share *s_y_vec;
	share *s_minus;
	share ***delta;
	delta=(share***)malloc(sizeof(share**)*rownumY);
	for(i=0;i<rownumY;i++){
		delta[i]=(share**)malloc(sizeof(share*)*colnumY);
		for(j=0;j<colnumY;j++){
			s_y_vec=circ->PutINGate(trainY[i][j],16,SERVER);
			s_minus=circ->PutSUBGate(s_y_vec,XW[i][j]);
			delta[i][j]=s_minus; //100*10
		}
	}

	cout<<"deltaT\n";
	share ***deltaT;
	deltaT=(share***)malloc(sizeof(share**)*colnumY);
	for(i=0;i<colnumY;i++){  //10
		deltaT[i]=(share**)malloc(sizeof(share*)*rownumY);
		for(j=0;j<rownumY;j++){  //100
			deltaT[i][j]=delta[i][j];
			// deltaT[i][j]=circ->PutCombinerGate(deltaT[i][j]);
			// cout<<deltaT[i][j]->get_bitlength()<<"\n";
			// cout<<deltaT[i][j]->get_nvals()<<"\n";
		}
	}
	cout<<"done!\n";
	//gradient=X*DELTAT
	share ***gradT; //10*392
	share *s_xT_vec,*s_grad;  
	share *tmpD;
	uint32_t k;
	gradT=(share***)malloc(sizeof(share**)*10); //10
	for(i=0;i<10;i++){ //10
		gradT[i]=(share**)malloc(sizeof(share*)*392); //392
		// tmpD=circ->PutCombinerGate(deltaT[i][0],deltaT[i][1]);
		// for(k=2;k<100;k++){
		// 	tmpD=circ->PutCombinerGate(tmpD,deltaT[i][k]);
		// 	cout<<"tmpD "<<tmpD->get_bitlength()<<"\n";
		// }
		for(j=0;j<392;j++){ // 392
			s_xT_vec=circ->PutSIMDINGate(100,get_col(X_1,j),16,SERVER); //100
			std::cout << "TTTTTTTTTTTTTTT" << '\n';
			s_grad=BuildMatmulProductCircuit((share*)deltaT[i],s_xT_vec,100,100,(ArithmeticCircuit*)circ); // 100 ,100, 100,100
			gradT[i][j]=s_grad;
		}
	}

	cout<<"loop\n";
	party->ExecCircuit();
	cout<<"execute\n";
	delete party;
	cout<<"done!\n";
	return 0;
}

/*
 *	Constructs the linear regression circuit.num multiplications and num additions.
 * */
share* BuildMatmulProductCircuit(share *s_x, share *s_y, uint32_t colnum1, uint32_t rownum2, ArithmeticCircuit *ac) {
	assert(colnum1==rownum2);

	// for (int j = 0; j< colnum1; ++j) {
	// 	ac->PutPrintValueGate(s_x, "s_x");
	// }
	// std::cout << "Print end" << '\n';
	uint32_t i;
	cout<<"s_x bitlen: "<<s_x->get_bitlength()<<"\n";
	cout<<"s_y_bitlen: "<<s_y->get_bitlength()<<"\n";
	s_x=ac->PutCombinerGate(s_x);
	// pairwise multiplication of all input values
	s_x = ac->PutMULGate(s_x, s_y);

	// split SIMD gate to separate wires (size many)
	s_x = ac->PutSplitterGate(s_x);
	cout<<"s_x bitlen: "<<s_x->get_bitlength()<<"\n";
	

	// add up the individual multiplication results and store result on wire 0
	// in arithmetic sharing ADD is for free, and does not add circuit depth, thus simple sequential adding
	for (i = 1; i < colnum1; i++) {
		s_x->set_wire_id(0, ac->PutADDGate(s_x->get_wire_id(0), s_x->get_wire_id(i)));
	}
	cout<<"for\n";
	// discard all wires, except the addition result
	s_x->set_bitlength(1);

	return s_x;
}

