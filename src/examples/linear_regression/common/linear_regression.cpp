#include "linear_regression.h"
#include "../../../abycore/sharing/sharing.h"
#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
using namespace std;

/*
 *	Constructs the linear regression circuit.num multiplications and num additions.
 * */
share*** BuildMatmulProductCircuit(share ***s_x, share ***s_y, uint32_t rownum1, uint32_t colnum1, uint32_t rownum2, uint32_t colnum2, ArithmeticCircuit *ac) {
	
	assert(colnum1==rownum2);
	cout<<"n_vals "<<s_x[1][1]->get_nvals()<<"\n";
	cout<<"n_bitlen "<<s_x[1][1]->get_bitlength()<<"\n";
	uint32_t i,j,k;

	share *s_out;
	share ***res;
	share *temp;
	uint16_t init=0;
	// cout<<"res\n";
	res=(share***)malloc(sizeof(share**)*rownum1);
	for(i=0;i<rownum1;i++){
		res[i]=(share**)malloc(sizeof(share*)*colnum2);
		for(j=0;j<colnum2;j++){
			// cout<<"init zero\n";
			res[i][j]=ac->PutSharedINGate(init,16);
			for(k=0;k<colnum1;k++){
				// ac->PutPrintValueGate(s_x[i][k],"s_x");
				// ac->PutPrintValueGate(s_y[k][j],"s_y");
				// cout<<"mul x "<<i<<" "<<k<<" y "<<k<<" "<<j<<" \n";
				temp=ac->PutMULGate(s_x[i][k],s_y[k][j]);
				// cout<<"add\n";
				res[i][j]=ac->PutADDGate(res[i][j],temp);
				// ac->PutPrintValueGate(res[i][j],"tmp");
			}
			// ac->PutPrintValueGate(res[i][j],"res");
		}
	}
	cout<<"matmul is over\n";
	return res;
}

/*Transformation: from string to double*/
uint16_t stringToNum(const string& str){
	istringstream iss(str);
	uint16_t num;
	iss>>num;
	return num;
}

/*Read data. Data format:vector<vector<double>>*/
void read_data(string& filename,vector<vector<uint16_t> > &X){
	// cout<<filename.c_str()<<"\n";
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
	// cout<<filename.c_str()<<"\n";
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


int32_t test_linear_regression_circuit(e_role role,const std::string& address,uint16_t port,seclvl seclvl,uint32_t bitlen,uint32_t nthreads,e_mt_gen_alg mt_alg,e_sharing sharing){
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

	ArithmeticCircuit* circ=(ArithmeticCircuit*) sharings[sharing]->GetCircuitBuildRoutine();


        /**
		Step 4:  Get the original valus of X,Y,W for respective parties.
				
	*/
	vector<vector<uint16_t> > X_train,X_test;
	vector<uint16_t> Y_train,Y_test;
	
	string filename1="/home/jean/datasets/mini_mnist/train_features.csv";
	string filename2="/home/jean/datasets/mini_mnist/test_features.csv";
	string filename3="/home/jean/datasets/mini_mnist/train_labels.csv";
	string filename4="/home/jean/datasets/mini_mnist/test_labels.csv";

	read_data(filename1,X_train);
	read_data(filename2,X_test);
	read_label(filename3,Y_train);
	read_label(filename4,Y_test);

	/**
		Step 5: Partition data for respective parties.Initialize W with zero.
	*/
	uint32_t i,j,k;
	uint32_t rownumX=100;
	uint32_t colnumX=392;
	uint32_t rownumY=100;
	uint32_t colnumY=10;
	uint32_t rownumW=392;
	uint32_t colnumW=10;

	/*Original Input X_1,X_2*/
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

	/*Initialize W_vec with zero */
	vector<vector<uint16_t> > W_vec(392,vector<uint16_t>(10,0));
	/**
	 * 	Step 6: Copy the original values into the respective share
	 * 		objects using the circuit object method.
	 * 		Call the build method for building the circuit
	 * 		for the problem by passing the shared objects 
	 * 		and circuit object.
	 * */

	/*trainY testY one-hot编码*/
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

	/*cout<<"Original Input:\n";
	cout<<"X1:\n";
	for(i=0;i<100;i++){
		for(j=0;j<392;j++){
			cout<<X_1[i][j]<<" ";
		}
		cout<<"\n";
	}
	cout<<"X2\n";
	for(i=0;i<100;i++){
		for(j=0;j<392;j++){
			cout<<X_2[i][j]<<" ";
		}
		cout<<"\n";
	}
	cout<<"W\n";
	for(i=0;i<392;i++){
		for(j=0;j<10;j++){
			cout<<W_vec[i][j]<<" ";
		}
		cout<<"\n";
	}
	cout<<"trainY\n";
	for(i=0;i<100;i++){
		for(j=0;j<10;j++){
			cout<<trainY[i][j]<<" ";
		}
		cout<<"\n";
	}
	cout<<"testY\n";
	for(i=0;i<10;i++){
		for(j=0;j<10;j++){
			cout<<testY[i][j]<<" ";
		}
		cout<<"\n";
	}*/

	cout<<"Initialization done!\n";

	/*Put Original Input into Gate : X1,X2,W*/
	share ***X1,***X2,***W;
	X1=(share***)malloc(sizeof(share**)*100);
	X2=(share***)malloc(sizeof(share**)*100);
	W=(share***)malloc(sizeof(share**)*392);

	for(i=0;i<100;i++){
		X1[i]=(share**)malloc(sizeof(share*)*392);
		X2[i]=(share**)malloc(sizeof(share*)*392);
		for(j=0;j<392;j++){
			X1[i][j]=circ->PutINGate(X_1[i][j],16,SERVER);
			X2[i][j]=circ->PutINGate(X_2[i][j],16,CLIENT);
		}
	}
	for(i=0;i<392;i++){
		W[i]=(share**)malloc(sizeof(share*)*10);
		for(j=0;j<10;j++){
			W[i][j]=circ->PutINGate(W_vec[i][j],16,ALL);
		}
	}
	cout<<"Already put in!\n ";

	share ***XW,***XW1,***XW2,***Y;
	share *s_x1_vec,*s_x2_vec,*s_w_vec,*s_y_vec;
	share *s_w1_vec,*s_w2_vec;
	share *s_out1,*s_out2,*s_out;
	uint16_t output=0;

	XW=(share***)malloc(sizeof(share**)*rownumX);
	XW1=(share***)malloc(sizeof(share**)*rownumX);
	XW2=(share***)malloc(sizeof(share**)*rownumX);
	Y=(share***)malloc(sizeof(share**)*rownumY);

	for(i=0;i<rownumY;i++){
		XW1[i]=(share**)malloc(sizeof(share*)*colnumY);
		XW2[i]=(share**)malloc(sizeof(share*)*colnumY);
	}

	uint32_t epoch=10;
	cout<<"XW1\n";
	//Compute XW1 100*10
	XW1=BuildMatmulProductCircuit(X1,W,rownumX,colnumX,rownumW,colnumW,(ArithmeticCircuit*)circ);
	
	cout<<"XW2\n";
	//Compute XW2 100*10
	XW2=BuildMatmulProductCircuit(X2,W,rownumX,colnumX,rownumW,colnumW,(ArithmeticCircuit*)circ);
	
	// cout<<"Y*\n";
	// //Y*=XW1+XW2
	// for(i=0;i<rownumY;i++){
	// 	XW[i]=(share**)malloc(sizeof(share*)*rownumY);
	// 	Y[i]=(share**)malloc(sizeof(share*)*rownumY);
	// 	for(j=0;j<colnumY;j++){
	// 		s_out=circ->PutADDGate(XW1[i][j],XW2[i][j]);
	// 		XW[i][j]=s_out;
	// 		Y[i][j]=circ->PutINGate(trainY[i][j],16,CLIENT);
	// 	}
	// }

	// cout<<"DELTA\n";
	// //DELTA=Y*-Y
	// share *s_minus;
	// share ***delta;
	// delta=(share***)malloc(sizeof(share**)*rownumY);
	// for(i=0;i<rownumY;i++){
	// 	delta[i]=(share**)malloc(sizeof(share*)*colnumY);
	// 	for(j=0;j<colnumY;j++){
	// 		s_minus=circ->PutSUBGate(Y[i][j],XW[i][j]);
	// 		delta[i][j]=s_minus; //100*10
	// 	}
	// }

	// cout<<"DELTAT\n";
	// /*deltaT*/
	// share ***deltaT;
	// deltaT=(share***)malloc(sizeof(share**)*colnumY);
	// for(i=0;i<colnumY;i++){  //10
	// 	deltaT[i]=(share**)malloc(sizeof(share*)*rownumY);
	// 	for(j=0;j<rownumY;j++){  //100
	// 		deltaT[i][j]=delta[j][i];
	// 	}
	// }

	// //grad^T1=DELTAT*X1
	// //grad^T2=DELTAT*X2
	// cout<<"gradT1,2\n";
	// share ***gradT1,***gradT2; //10*392
	// gradT1=(share***)malloc(sizeof(share**)*10); //10
	// gradT2=(share***)malloc(sizeof(share**)*10);
	// for(i=0;i<10;i++){
	// 	gradT1[i]=(share**)malloc(sizeof(share*)*392);
	// 	gradT2[i]=(share**)malloc(sizeof(share*)*392);
	// }
	// gradT1=BuildMatmulProductCircuit(deltaT,X1,colnumY,rownumY,rownumX,colnumX,(ArithmeticCircuit*)circ);
	// gradT2=BuildMatmulProductCircuit(deltaT,X2,colnumY,rownumY,rownumX,colnumX,(ArithmeticCircuit*)circ);
	
	// //grad
	// cout<<"grad\n";
	// share ***grad;
	// grad=(share***)malloc(sizeof(share**)*392);
	// for(i=0;i<392;i++){
	// 	grad[i]=(share**)malloc(sizeof(share*)*10);
	// 	for(j=0;j<10;j++){
	// 		grad[i][j]=circ->PutADDGate(gradT1[j][i],gradT2[j][i]);
	// 	}
	// }

	// // update w
	// cout<<"w\n";
	// for(i=0;i<392;i++){
	// 	for(j=0;j<10;j++){
	// 		W[i][j]=circ->PutSUBGate(W[i][j],grad[i][j]);
	// 		W[i][j]=circ->PutOUTGate(W[i][j],ALL);
	// 	}
	// }

	for (i=0;i<rownumY;i++){
		for(j=0;j<colnumY;j++){
			XW1[i][j]=circ->PutOUTGate(XW1[i][j],SERVER);
			XW2[i][j]=circ->PutOUTGate(XW2[i][j],CLIENT);
		}
	}
	

	cout<<"loop\n";
	party->ExecCircuit();
	cout<<"execute\n";
	uint16_t res1,res2;
	for(i=0;i<rownumY;i++){
		for(j=0;j<colnumY;j++){
			res1=XW1[i][j]->get_clear_value<uint16_t>();
			cout<<res1<<" ";
		}
		cout<<"\n";
	}

	for(i=0;i<rownumY;i++){
		for(j=0;j<colnumY;j++){
			res2=XW2[i][j]->get_clear_value<uint16_t>();
			cout<<res2<<" ";
		}
		cout<<"\n";
	}
	delete party;
	cout<<"done!\n";
	return 0;
}
