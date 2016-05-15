/*
	Sparse Matrices and Tensors

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef SMATRIX_H_
#define SMATRIX_H_

#include <iostream>
#include <fstream>
#include <assert.h>
#include "token_reader.h"
#include <map>
#include <set>
#include <vector>
#include <algorithm>

template <typename T> class SparseVector : public std::map<int,T> {
	public:
		T get(int x) {
			typename SparseVector<T>::iterator iter = this->find(x);
			if (iter != this->end()) {
				return iter->second;
			} else {
				return 0;
			}			
		}
		void toStream(std::ostream &stream);
};

template <typename T>  class SparseMatrix : public std::map<int, SparseVector<T> > { 
	public:
		T get(int x, int y) {
			typename SparseMatrix<T>::iterator iter = this->find(x);
			if (iter != this->end()) {
				return iter->second.get(y);
			} else {
				return 0;
			}
		}
		void toStream(std::ostream &stream);		
		void fromFile(const std::string &filename);
};
template <typename T> class SparseTensor : public std::map<int, SparseMatrix<T> > {
	public:
		T get(int x, int y, int z) {
			typename SparseTensor<T>::iterator iter = this->find(x);
			if (iter != this->end()) {
				return iter->second.get(y, z);
			} else {
				return 0;
			}
		}	
		void toStream(std::ostream &stream);		
		void toFile(const std::string &filename);
		void fromFile(const std::string &filename);
};

class SparseVectorInt : public SparseVector<int> {};
class SparseMatrixInt : public SparseMatrix<int> {};
class SparseTensorInt : public SparseTensor<int> {};
class SparseVectorDouble : public SparseVector<double> {};
class SparseMatrixDouble : public SparseMatrix<double> {};
class SparseTensorDouble : public SparseTensor<double> {};

//<item_n-1, item_n-2, ...>
class SparseVectorBoolean : public std::vector<int> {
	public:
		bool get(int x) {
			if( std::find(this->begin(), this->end(), x) != this->end() ) {
				return true;
			}
			else {
				return false;
			}
				
		}
};
class SparseSetBoolean : public std::set<int> {
	public:
		bool get(int x) {
			SparseSetBoolean::iterator iter = this->find(x);
			if (iter != this->end()) {
				return true;
			} else {
				return false;
			}
		}
};
//<item_n, ...>
class SparseMatrixBoolean : public std::map< int, SparseVectorBoolean > {
	public:
		bool get(int x, int y) {
			SparseMatrixBoolean::iterator iter = this->find(x);
			if (iter != this->end()) {
				return iter->second.get(y);
			} else {
				return 0;
			}
		}
};
//<time,..>
class SparseTensorBoolean : public std::map<int, SparseMatrixBoolean> {
	public:
		bool get(int x, int y, int z) {
			SparseTensorBoolean::iterator iter = this->find(x);
			if (iter != this->end()) {
				return iter->second.get(y, z);
			} else {
				return 0;
			}
		}	
		void toStream(std::ostream &stream);
		void toFile(const std::string &filename);
		void fromFile(const std::string &filename);		
};
//<user,..>
class SparseFourDimBoolean : public std::map<int, SparseTensorBoolean> {
	public:
		bool get(int x, int y, int z, int w) {
			SparseFourDimBoolean::iterator iter = this->find(x);
			if (iter != this->end()) {
				return iter->second.get(y, z, w);
			} else {
				return 0;
			}
		}	
		void toStream(std::ostream &stream);
		void toFile(const std::string &filename);
		void fromFile(const std::string &filename);		
		
};
template <typename T> void SparseVector<T>::toStream(std::ostream &stream) {
	for(typename SparseVector<T>::const_iter it_cell = this->begin(); it_cell != this->end(); ++it_cell) {
		stream << it_cell->first << " " << it_cell->second << std::endl;
	}
}

template <typename T> void SparseMatrix<T>::toStream(std::ostream &stream) {
	for(typename SparseMatrix<T>::const_iter i = this->begin(); i != this->end(); ++i) {
		for(typename SparseVector<T>::const_iter j = i->second->begin(); j != i->second->end(); ++j) {
			stream << i->first << " " << j->first << " " << j->second << std::endl;
		}
	}
}

template <typename T> void SparseTensor<T>::toStream(std::ostream &stream) {
	for(typename SparseTensor<T>::const_iterator t = this->begin(); t != this->end(); ++t) {
		for(typename SparseMatrix<T>::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
			for(typename SparseVector<T>::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
				stream << t->first << " " << i->first << " " << j->first << " " << j->second << std::endl;
			}
		}
	}
}
	
template <typename T> void SparseTensor<T>::toFile(const std::string &filename) {
	std::ofstream out_file (filename.c_str());
	if (out_file.is_open())	{
		toStream(out_file);
		out_file.close();
	} else {
		throw "Unable to open file " + filename;
	}	
	
}

template <typename T> void SparseTensor<T>::fromFile(const std::string &filename) {
	std::ifstream fData (filename.c_str());
  	if (fData.is_open()) {
		token_reader fData2(&fData);
		do {
			int t = fData2.readInt();
			int m = fData2.readInt();
			//int v = fData2.readInt();
			T value;
			fData2.read(value);
			if (! fData2.is_missing) {
				(*this)[t][m] = value;
			}	
		} while (fData2.ch != 0);
		fData.close();
	} else {
		throw "Unable to open file " + filename;
	}		
}

template <typename T> void SparseMatrix<T>::fromFile(const std::string &filename) {
	std::ifstream fData (filename.c_str());
  	if (fData.is_open()) {
		token_reader fData2(&fData);
		do {
			int t = fData2.readInt();
			int m = fData2.readInt();
			T value;
			fData2.read(value);
			if (! fData2.is_missing) {
				(*this)[t][m] = value;
			}	
		} while (fData2.ch != 0);
		fData.close();
	} else {
		throw "Unable to open file " + filename;
	}		
}

void SparseFourDimBoolean::toStream(std::ostream &stream) {
	for(SparseFourDimBoolean::const_iterator u = this->begin(); u != this->end(); ++u){
		for(SparseTensorBoolean::const_iterator t = u->second.begin(); t != u->second.end(); ++t) {
			for(SparseMatrixBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
				for(SparseVectorBoolean::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
					stream << u->first << " " << t->first << " " << i->first << " " << (*j) << std::endl;
				}
			}
		}
	}
}
	
void SparseFourDimBoolean::toFile(const std::string &filename) {
	std::ofstream out_file (filename.c_str());
	if (out_file.is_open())	{
		toStream(out_file);
		out_file.close();
	} else {
		throw "Unable to open file " + filename;
	}	
	
}

void SparseFourDimBoolean::fromFile(const std::string &filename) {
	std::ifstream fData (filename.c_str());
  	if (fData.is_open()) {
		token_reader fData2(&fData);
		do {
			int userid = fData2.readInt();
			int timeid = fData2.readInt();
			int seqlength = fData2.readInt();
			std::vector<int> seq;
			for(int i = 0; i < seqlength-1; i++) {
				int itemid = fData2.readInt();
				seq.push_back(itemid);
			}
			int next_itemid = fData2.readInt();

			if (! fData2.is_missing) {
				// insert sequence into std::vector<int>
				SparseVectorBoolean::iterator it = (*this)[userid][timeid][next_itemid].end();
				std::reverse(seq.begin(), seq.end());
				(*this)[userid][timeid][next_itemid].insert(it, seq.begin(), seq.end());
			}
		} while (fData2.ch != 0);	
		fData.close();
	} else {
		throw "Unable to open file " + filename;
	}		
}


#endif /*SMATRIX_H_*/
