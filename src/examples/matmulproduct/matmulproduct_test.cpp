/**
 \file 		matmulproduct_test.cpp
 \author	sreeram.sadasivam@cased.de
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
 \brief		Inner Product Test class implementation.
 */

//Utility libs
#include <ENCRYPTO_utils/crypto/crypto.h>
#include <ENCRYPTO_utils/parse_options.h>
//ABY Party class
#include "../../abycore/aby/abyparty.h"

#include "common/matmulproduct.h"

int32_t read_test_options(int32_t* argcp, char*** argvp, e_role* role,
		uint32_t* bitlen, uint32_t* rownum1,uint32_t* colnum1, uint32_t* rownum2,uint32_t* colnum2,uint32_t* secparam, std::string* address,
		uint16_t* port, int32_t* test_op) {
	
	//std::cout << *rownum1 << '\n';
	uint32_t int_role = 0, int_port = 0;
	uint32_t int_r1 = 0, int_r2=0, int_c1=0, int_c2=0;

	parsing_ctx options[] =
			{ { (void*) &int_role, T_NUM, "r", "Role: 0/1", true, false },
			  { (void*) &int_r1, T_NUM, "c",	"Row number of elements for matmul product, default: 128", false, false },
              { (void*) &int_c1, T_NUM, "d",	"Col number of elements for matmul product, default: 128", false, false },
              { (void*) &int_r2, T_NUM, "e",	"Row number of elements for matmul product, default: 128", false, false },
              { (void*) &int_c2, T_NUM, "f",	"Col number of elements for matmul product, default: 128", false, false },
    		  {	(void*) bitlen, T_NUM, "b", "Bit-length, default 16", false, false },
			  { (void*) secparam, T_NUM, "s", "Symmetric Security Bits, default: 128", false, false },
			  {	(void*) address, T_STR, "a", "IP-address, default: localhost", false, false },
			  {	(void*) &int_port, T_NUM, "p", "Port, default: 7766", false, false },
			  { (void*) test_op, T_NUM, "t", "Single test (leave out for all operations), default: off",
					false, false } };

	if (!parse_options(argcp, argvp, options,
			sizeof(options) / sizeof(parsing_ctx))) {
		print_usage(*argvp[0], options, sizeof(options) / sizeof(parsing_ctx));
		std::cout << "Exiting" << std::endl;
		exit(0);
	}
	assert(int_role < 2);
	*role = (e_role) int_role;
	assert(int_c1==int_r2);
	*rownum1 = (uint32_t) int_r1;
	*colnum1 = (uint32_t) int_c1;
	*rownum2 = (uint32_t) int_r2;
	*colnum2 = (uint32_t) int_c2;

	if (int_port != 0) {
		assert(int_port < 1 << (sizeof(uint16_t) * 8));
		*port = (uint16_t) int_port;
	}
	//std::cout<<"in test:"<<*rownum1<<"\n";

	return 1;
}

int main(int argc, char** argv) {

	e_role role;
	uint32_t bitlen = 16, rownum1 = 128, colnum1=128, rownum2=128, colnum2=128, secparam = 128, nthreads = 1;
	uint16_t port = 7766;
	std::string address = "127.0.0.1";
	int32_t test_op = -1;
	e_mt_gen_alg mt_alg = MT_OT;

	read_test_options(&argc, &argv, &role, &bitlen, &rownum1, &colnum1, &rownum2, &colnum2, &secparam, &address, &port, &test_op);

	seclvl seclvl = get_sec_lvl(secparam);

	//std::cout<<"read rownum1 "<<rownum1<<"\n";

	// call matmul product routine. set size with cmd-parameter -r1 - <size>
	test_matmul_product_circuit(role, address, port, seclvl, rownum1, colnum1, rownum2, colnum2, bitlen, nthreads, mt_alg, S_ARITH);

	return 0;
}

