#ifndef __LINEARREGRESSION_H_
#define __LINEARREGRESSION_H_

#include "../../../abycore/circuit/booleancircuits.h"
#include "../../../abycore/circuit/arithmeticcircuits.h"
#include "../../../abycore/circuit/circuit.h"
#include "../../../abycore/aby/abyparty.h"
#include <math.h>
#include <cassert>
#include <vector>
#include <string>
using namespace std;

/*This is a transforming function used in the process of reading data from files.*/
uint16_t stringToNum(const string& str);


/*A function for reading features from files.*/
void read_data(string &filename,vector<vector<uint16_t> > &X);

/*A function for reading labels from files.*/
void read_label(string &filename,uint16_t* &X);

//void read_test_label(string &filename,uint64_t* &X);
/*A function for getting rows from matrix.*/
uint16_t* get_col(vector<vector<uint16_t> > &X,uint32_t index);

/*A function for getting cols from matrix.*/
uint16_t* get_row(vector<vector<uint16_t> >&X,uint32_t index);

uint16_t* get_y_col(vector<uint16_t> &Y);

uint16_t* get_y_row(uint16_t **Y,int index);
share* BuildMatmulProductCircuit(share *s_x, share *s_y, uint32_t colnum1, uint32_t rownum2, ArithmeticCircuit *ac);

//share* get_share_row(share **W,uint32_t index);

/*A function for linear regression training. */
int32_t test_linear_regression_circuit(e_role role,const std::string& address,uint16_t port,seclvl seclvl,uint32_t bitlen,uint32_t nthreads,e_mt_gen_alg mt_alg,e_sharing dstsharing,e_sharing minsharing,uint32_t n,bool only_yao);

#endif /*__LINEARREGRESSION_H_*/

