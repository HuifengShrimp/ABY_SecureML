/**
 \file 		matmulproduct.h
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

#ifndef __MATMULPRODUCT_H_
#define __MATMULPRODUCT_H_

#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/circuit/arithmeticcircuits.h"
#include "../../../abycore/circuit/circuit.h"
#include "../../../abycore/aby/abyparty.h"
#include <math.h>
#include <cassert>
#include <vector>
using namespace std;


/**
 \param		role 		role played by the program which can be server or client part.
 \param 	address 	IP Address
 \param 	seclvl 		Security level
 \param 	nvals		Number of values
 \param 	bitlen		Bit length of the inputs
 \param 	nthreads	Number of threads
 \param		mt_alg		The algorithm for generation of multiplication triples
 \param 	sharing		Sharing type object
 \param 	rownum		The row number of elements in the matmul product
 \param     colnum      The col number of elements in the matmul product
 \brief		This function is used for running a testing environment for solving the
 MATMUL Product.
 */
/*
uint16_t* getcol(uint16_t **matrix, uint32_t index,uint32_t rownum,uint32_t colnum);
uint16_t* getrow(uint16_t **matrix, uint32_t index,uint32_t rownum,uint32_t colnum);
*/
// uint16_t* get_col(vector<vector<uint16_t> > &X,uint32_t index);
// uint16_t* get_row(vector<vector<uint16_t> > &X,uint32_t index);

int32_t test_matmul_product_circuit(e_role role, const std::string& address, uint16_t port, seclvl seclvl,
		uint32_t rownum1, uint32_t colnum1,uint32_t rownum2, uint32_t colnum2, uint32_t bitlen, uint32_t nthreads, e_mt_gen_alg mt_alg,
		e_sharing sharing);

/**
 \param		s_x			share of X values
 \param		s_y 		share of Y values
 \param 	rownum	    the row number of elements in the matmul product
 \param     colnum      the col number of elements in the matmul product
 \param		ac	 		Arithmetic Circuit object.
 \brief		This function is used to build and solve the Matmul Product modulo 2^16. It computes the matmul product by
 	 	 	multiplying each row value in x and col velue in y, and adding those multiplied results to evaluate the matmul
 	 	 	product. The addition is performed in a tree, thus with logarithmic depth.
 */
share*** BuildMatmulProductCircuit(share *s_x, share *s_y, uint32_t rownum1, uint32_t colnum1, uint32_t rownum2, uint32_t colnum2, ArithmeticCircuit *ac) ;
share* BuildInnerProductCircuit(share *s_x, share *s_y, uint32_t numbers, ArithmeticCircuit *ac);

#endif