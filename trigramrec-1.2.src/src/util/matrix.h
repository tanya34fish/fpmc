/*
	Dense Matrix and Vectors

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef MATRIX_H_
#define MATRIX_H_

#include <vector>
#include <assert.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "../util/random.h"


template <typename T> class DMatrix {
	public:
		std::vector<std::string> col_names;
		uint dim1, dim2;
		T** value;
		
		T get(uint x, uint y) {
   			assert((x < dim1) && (y < dim2));
			return value[x][y];
		}

		DMatrix(uint p_dim1, uint p_dim2) {
			dim1 = 0;
			dim2 = 0;	
			value = NULL;
			setSize(p_dim1, p_dim2);
		}
		
		DMatrix() {
			dim1 = 0;
			dim2 = 0;	
			value = NULL;
		}
		
		~DMatrix() {
			if (value != NULL) {
				delete [] value[0];
				delete [] value;
			}	
		}
		
		void setSize(uint p_dim1, uint p_dim2) {
			if (value != NULL) {
				delete [] value[0];
				delete [] value;
			}
			dim1 = p_dim1;
			dim2 = p_dim2;
			value = new T*[dim1];
			value[0] = new T[dim1 * dim2];
			for (unsigned i = 1; i < dim1; i++) {
				value[i] = value[0] + i * dim2;
			}			
			col_names.resize(dim2);
			for (unsigned i = 1; i < dim2; i++) {
				col_names[i] = "";
			}						
		}
		
		T& operator() (unsigned x, unsigned y) {
   			assert((x < dim1) && (y < dim2));
			return value[x][y];	
		}
   		T operator() (unsigned x, unsigned y) const {
   			assert((x < dim1) && (y < dim2));
   			return value[x][y];
   		}
   		
   		T* operator() (unsigned x) const {
   			assert((x < dim1));
   			return value[x];
   		}
   		
   		void save(std::string filename, bool has_header = false) {
		   	std::ofstream out_file (filename.c_str());
			if (out_file.is_open())	{
				if (has_header) {
					for (uint i_2 = 0; i_2 < dim2; i_2++) {
						if (i_2 > 0) {
							out_file << "\t";
						}
						out_file << col_names[i_2];					
					}	
					out_file << std::endl;
				}
				for (uint i_1 = 0; i_1 < dim1; i_1++) {
					for (uint i_2 = 0; i_2 < dim2; i_2++) {
						if (i_2 > 0) {
							out_file << "\t";
						}
						out_file << value[i_1][i_2];
					}
					out_file << std::endl;
				}
				out_file.close();
			} else {
				std::cout << "Unable to open file " << filename;
			}   			
   		}
   		
   		
};

template <typename T> class DVector {
	public:
		uint dim;
		T* value;
		DVector() {
			dim = 0;
			value = NULL;
		}
		DVector(uint p_dim) {
			dim = 0;
			value = NULL;
			setSize(p_dim);
		}
		~DVector() {
			if (value != NULL) {
				delete [] value;
			}	
		}
		T get(uint x) {
			return value[x];
		}
		void setSize(uint p_dim) {
			if (value != NULL) {
				delete [] value;
			}
			dim = p_dim;
			value = new T[dim];			
		}
		T& operator() (unsigned x) {
			return value[x];	
		}
   		T operator() (unsigned x) const {
   			return value[x];
   		}
   		void init(T v) {
   			for (uint i = 0; i < dim; i++) {
   				value[i] = v;
   			}	
   		}
   		void assign(T* v) {
			if (v->dim != dim) { setSize(v->dim); }
   			for (uint i = 0; i < dim; i++) {
   				value[i] = v[i];
   			}
   		}
   		void save(std::string filename) {
		   	std::ofstream out_file (filename.c_str());
			if (out_file.is_open())	{
				for (uint i = 0; i < dim; i++) {
					out_file << value[i] << std::endl;
				}
				out_file.close();
			} else {
				std::cout << "Unable to open file " << filename;
			}   			
   		}
};


class DMatrixDouble : public DMatrix<double> {
	public:
		void init(double mean, double stdev) {	
			for (uint i_1 = 0; i_1 < dim1; i_1++) {
				for (uint i_2 = 0; i_2 < dim2; i_2++) {
					do{
						value[i_1][i_2] = ran_gaussian(mean, stdev);
					}while(isnan(value[i_1][i_2]));
					
				}
			}
		}
		void init_column(double mean, double stdev, int column) {	
			for (uint i_1 = 0; i_1 < dim1; i_1++) {
				value[i_1][column] = ran_gaussian(mean, stdev);
			}
		}
};


#endif /*MATRIX_H_*/
