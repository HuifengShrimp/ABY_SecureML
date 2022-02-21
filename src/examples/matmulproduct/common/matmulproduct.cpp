/**
 \file 		matmulproduct.cpp
 \author 	sreeram.sadasivam@cased.de
 \copyright	ABY - A Framework for Efficient Mixed-protocol Secure Two-party Computation
 Copyright (C) 2019 Engineering Cryptographic Protocols Group, TU Darmstadt
			This program is free software: you can redistribute it and/or modify
            it under the terms of the GNU Lesser General Public License as published
            by the Free Software Foundation, either version 3 of the License, or
            (at your option) any later version.
            ABY is distributed in the hope that it will be useful,
            but WITHOUT ANY WARRANTY; without even the implied warranty of
            MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
            GNU Lesser General Public License for more details.
            You should have received a copy of the GNU Lesser General Public License
            along with this program. If not, see <http://www.gnu.org/licenses/>.
 \brief		Implementation of the Inner Product using ABY Framework.
 */

#include "matmulproduct.h"
#include "../../../abycore/sharing/sharing.h"
#include <vector>
#include <iostream>
// #include <innerproduct.h>
using namespace std;

/*uint16_t* get_col(vector<vector<uint16_t> > &X,uint32_t index){
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
}*/

int32_t test_matmul_product_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t rownum1, uint32_t colnum1, uint32_t rownum2,uint32_t colnum2,uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing) {

	/**
	 Step 1: Create the ABYParty object which defines the basis of all the
	 operations which are happening.	Operations performed are on the
	 basis of the role played by this object.
	 */
	ABYParty* party = new ABYParty(role, address, port, seclvl, bitlen, nthreads,
			mt_alg);

	/**
	 Step 2: Get to know all the sharing types available in the program.
	 */
	std::vector<Sharing*>& sharings = party->GetSharings();

	/**
	 Step 3: Create the circuit object on the basis of the sharing type
	 being inputed.
	 */
	ArithmeticCircuit* circ =
			(ArithmeticCircuit*) sharings[sharing]->GetCircuitBuildRoutine();

	/**
	 Step 4: Creating the share objects - s_x_vec, s_y_vec which
	 are used as inputs to the computation. Also, s_out which stores the output.
	 */

	share *s_x_vec, *s_y_vec;

	/**
	 Step 5: Allocate the xvals and yvals that will hold the plaintext values.
	 */
	uint16_t x, y ;

	std::vector< vector<uint16_t> > xvals(rownum1);
	std::vector< vector<uint16_t> > yvals(rownum2);
	std::vector< vector<uint16_t> > vals(rownum1);

	uint32_t i,j,k;
	for(i=0;i<xvals.size();i++){
		xvals[i].resize(colnum1);
	}
	for(i=0;i<yvals.size();i++){
		yvals[i].resize(colnum2);
	}
	for(i=0;i<vals.size();i++){
		vals[i].resize(colnum2);
	}

	srand(time(NULL));

	/**
	 Step 6: Fill the arrays xvals and yvals with the generated random values.
	 Both parties use the same seed, to be able to verify the
	 result. In a real example each party would only supply
	 one input value. Copy the randomly generated vector values into the respective
	 share objects using the circuit object method PutINGate().
	 Also mention who is sharing the object.
	 The values for the party different from role is ignored,
	 but PutINGate() must always be called for both roles.
	 */
	k=1;
	for (i = 0; i < rownum1; i++) {
        for (j=0; j < colnum1; j++){
        	xvals[i][j]=k+1;
			k++;
        }
	}

    for(i=0;i<rownum2;i++){
        for(j=0;j<colnum2;j++){
			yvals[i][j]=1;
        }
    }

	uint16_t tmp;
	std::cout<<"\nVerification Result: "<<"\n";
	 for(i=0;i<rownum1;i++){
        for(j=0;j<colnum2;j++){
			tmp=0;
            for(k=0;k<colnum1;k++){
                tmp+=xvals[i][k]*yvals[k][j];
            }
			vals[i][j]=tmp;
        }
    }
	for(i=0;i<rownum1;i++){
		for(j=0;j<colnum2;j++){
			std::cout<<vals[i][j]<<" ";
		}
		std::cout<<"\n";
	}

	//store matrix into a vector
	uint16_t* x_vec=new uint16_t[rownum1*colnum1];
	uint16_t* y_vec=new uint16_t[rownum2*colnum2];
	for(i=0;i<rownum1;i++){
		for(j=0;j<colnum1;j++){
			x_vec[i*colnum1+j]=xvals[i][j];
		}
	}
	for(i=0;i<rownum2;i++){
		for(j=0;j<colnum2;j++){
			y_vec[i*colnum2+j]=yvals[i][j];
		}
	}

	//Put the vector into PutINGate
	// share *s_x_vec, *s_y_vec, *s_out;
	share ***s_out;
	s_x_vec = circ->PutSIMDINGate(rownum1*colnum1, x_vec, 16, SERVER);
	s_y_vec = circ->PutSIMDINGate(rownum2*colnum2, y_vec, 16, CLIENT);
	s_out=BuildMatmulProductCircuit(s_x_vec,s_y_vec,rownum1,colnum1,rownum2,colnum2,circ);
	// circ->PutPrintValueGate(s_out[0][0],"reveal");
	// circ->PutPrintValueGate(s_out[0][1],"reveal");
	// circ->PutPrintValueGate(s_out[1][0],"reveal");
	// circ->PutPrintValueGate(s_out[1][1],"reveal");
	/**/
	share **output;
	share *result;
	k=0;
	output=(share**)malloc(sizeof(share*)*(rownum1*colnum2));
	for(i=0;i<rownum1;i++){
		for(j=0;j<colnum2;j++){
			result=s_out[i][j];
			output[k]=circ->PutOUTGate(result,ALL);
			// circ->PutPrintValueGate(output[k],"output");
			k++;
		}
	}

	party->ExecCircuit();

	uint16_t matmul;
	k=0;
	cout<<"Circuit Result:\n";
	for(i=0;i<rownum1;i++){
		for(j=0;j<colnum2;j++){
			matmul=output[k]->get_clear_value<uint16_t>();
			cout<<matmul<<" ";	
			k++;
		}
		cout<<"\n";
	}	

	delete party;

	return 0;
}

/*
 Constructs the matmul product circuit. num multiplications and num additions.
 */

share*** BuildMatmulProductCircuit(share *s_x, share *s_y, uint32_t rownum1, uint32_t colnum1, uint32_t rownum2, uint32_t colnum2, ArithmeticCircuit *ac) {
	assert(colnum1==rownum2);
	
	s_x=ac->PutSplitterGate(s_x);
	// cout<<"x's bitlen "<<s_x->get_bitlength()<<"\n";
	s_y=ac->PutSplitterGate(s_y);
    // cout<<"y's bitlen "<<s_y->get_bitlength()<<"\n";
	uint32_t i,j,k;
	share ***shX;
	share ***shY;
	shX=(share***)malloc(sizeof(share**)*rownum1);
	shY=(share***)malloc(sizeof(share**)*rownum2);
	// cout<<"clarification\n";
	for(i=0;i<rownum1;i++){
		shX[i]=(share**)malloc(sizeof(share*)*colnum1);
		for(j=0;j<colnum1;j++){
			shX[i][j]=s_x->get_wire_ids_as_share(i*colnum1+j);
			// cout<<"shareX\n";
		    // ac->PutPrintValueGate(shX[i][j],"x");
		}
	}
	//y
	for(i=0;i<colnum2;i++){
		shY[i]=(share**)malloc(sizeof(share*)*rownum2);
		for(j=0;j<rownum2;j++){
			shY[i][j]=s_y->get_wire_ids_as_share(j*colnum2+i);
			// cout<<"shareY\n";
			// ac->PutPrintValueGate(shY[i][j],"y");
		}
	}
	cout<<"n_vals "<<shX[1][1]->get_nvals()<<"\n";
	cout<<"n_bitlen "<<shY[1][1]->get_bitlength()<<"\n";
	share *s_out;
	share ***res;
	share *temp;
	uint32_t init=0;
	// ac->PutPrintValueGate(shX[0][0],"shX00");
	// ac->PutPrintValueGate(shX[1][0],"shX10");
	// ac->PutPrintValueGate(shY[0][0],"shY00");
	// ac->PutPrintValueGate(shY[1][0],"shY10");
	res=(share***)malloc(sizeof(share**)*rownum1);
	for(i=0;i<rownum1;i++){
		res[i]=(share**)malloc(sizeof(share*)*colnum2);
		for(j=0;j<colnum2;j++){
			cout<<"init zero\n";
			res[i][j]=ac->PutSharedINGate(init,16);
			for(k=0;k<colnum1;k++){
				// cout<<"mul x "<<i<<" "<<k<<" y "<<k<<" "<<j<<" \n";
				temp=ac->PutMULGate(shX[i][k],shY[k][j]);
				// cout<<"add\n";
				res[i][j]=ac->PutADDGate(res[i][j],temp);
				// ac->PutPrintValueGate(res[i][j],"tmp");
			}
			// ac->PutPrintValueGate(res[i][j],"res");
		}
	}
/*
	share *out;
	for(i=0;i<rownum1;i++){
		for(j=0;j<colnum2;j++){
			out[i*colnum2+j]=res[i][j];
			// ac->PutPrintValueGate(out[i*colnum2+j],"out");
			cout<<"out\n";
		}
	}*/

	return res;
}

share* BuildInnerProductCircuit(share *s_x, share *s_y, uint32_t numbers, ArithmeticCircuit *ac) {
	uint32_t i;

	// pairwise multiplication of all input values
	s_x = ac->PutMULGate(s_x, s_y);

	// split SIMD gate to separate wires (size many)
	s_x = ac->PutSplitterGate(s_x);

	// add up the individual multiplication results and store result on wire 0
	// in arithmetic sharing ADD is for free, and does not add circuit depth, thus simple sequential adding
	for (i = 1; i < numbers; i++) {
		s_x->set_wire_id(0, ac->PutADDGate(s_x->get_wire_id(0), s_x->get_wire_id(i)));
	}

	// discard all wires, except the addition result
	s_x->set_bitlength(1);

	return s_x;
}