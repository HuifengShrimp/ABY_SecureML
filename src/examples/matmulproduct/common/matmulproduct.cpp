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
using namespace std;

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

	share *s_x_vec, *s_y_vec,*s_out;

	/**
	 Step 5: Allocate the xvals and yvals that will hold the plaintext values.
	 */
	uint16_t x, y ;

	std::vector< vector<uint16_t> > xvals;
	std::vector< vector<uint16_t> > yvals;
	std::vector< vector<uint16_t> > vals;
	std::vector<uint16_t> d;

	uint32_t i,j,k;
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
	for (i = 0; i < rownum1; i++) {
        for (j=0; j < colnum1; j++){
            x = rand();
            d.push_back(x);
        }
		xvals.push_back(d);
		d.resize(0);
	}

    for(i=0;i<rownum2;i++){
        for(j=0;j<colnum2;j++){
            y=rand();
            d.push_back(y);
        }
		yvals.push_back(d);
		d.resize(0);
    }
/*
	std::cout<<"\nVerification Result: "<<"\n";
	 for(i=0;i<rownum1;i++){
        for(j=0;j<colnum2;j++){
			vals[i][j]=0;
			std::cout<<vals[i][j]<<"\n";
            for(k=0;k<colnum1;k++){
                vals[i][j]+=xvals[i][k]*yvals[k][j];
            }
        }
    }
	for(i=0;i<rownum1;i++){
		for(j=0;j<colnum2;j++){
			std::cout<<vals[i][j]<<" ";
		}
		std::cout<<"\n";
	}*/
	share ***res;
	res=(share***)malloc(sizeof(share**)*rownum1);
	for(i=0;i<rownum1;i++){
		res[i]=(share**)malloc(sizeof(share*)*colnum2);
		s_x_vec=circ->PutSIMDINGate(colnum1,get_row(xvals,i),16,SERVER);
		for(j=0;j<colnum2;j++){
			s_y_vec=circ->PutSIMDINGate(rownum2,get_col(yvals,j),16,CLIENT);
			s_out=BuildMatmulProductCircuit(s_x_vec,s_y_vec,colnum1,rownum2,(ArithmeticCircuit*)circ);
			res[i][j]=s_out;
			circ->PutPrintValueGate(s_out,"s_out");
		}	
	}

	party->ExecCircuit();

	delete party;

	return 0;
}

/*
 Constructs the matmul product circuit. num multiplications and num additions.
 */
/*
share* BuildMatmulProductCircuit(){
	
}*/

share* BuildMatmulProductCircuit(share *s_x, share *s_y, uint32_t colnum1, uint32_t rownum2, ArithmeticCircuit *ac) {
	assert(colnum1==rownum2);
	uint32_t i;

	// pairwise multiplication of all input values
	s_x = ac->PutMULGate(s_x, s_y);

	// split SIMD gate to separate wires (size many)
	s_x = ac->PutSplitterGate(s_x);

	// add up the individual multiplication results and store result on wire 0
	// in arithmetic sharing ADD is for free, and does not add circuit depth, thus simple sequential adding
	for (i = 1; i < colnum1; i++) {
		s_x->set_wire_id(0, ac->PutADDGate(s_x->get_wire_id(0), s_x->get_wire_id(i)));
	}

	// discard all wires, except the addition result
	s_x->set_bitlength(1);

	return s_x;
}
