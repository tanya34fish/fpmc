/*
	Dataset for tag recommender

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef __DATA_H__
#define __DATA_H__

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include <math.h>
#include <assert.h>

#include "../../util/token_reader.h"
#include "../../util/util.h"
#include "../../util/matrix.h"
#include "../../util/smatrix.h"


class Dataset {
	private:
		void loadData(std::string filename);
		void loadTest(std::string filename);
		
	public:
		// all data is stored in the order (user,time, item sequence)
		SparseFourDimBoolean data;
		SparseFourDimBoolean test_data;
		SparseTensorBoolean test_baskets;
		
		int max_user_id, max_time_id, max_item_id;
		
		Dataset(std::string filename) {
  			max_user_id = -1;
  			max_time_id = -1;
  			max_item_id = -1;
  			std::cout << "read data file " << filename << "..."; std::cout.flush();
			loadData(filename); 		
		}	
		void loadTestSplit(std::string filename) {
			std::cout << "read test file " << filename << "..."; std::cout.flush();
			loadTest(filename); 	
		}
};


void Dataset::loadData(std::string filename) {
	data.fromFile(filename);
	int num_baskets = 0;
	//user
	for(SparseFourDimBoolean::const_iterator t = data.begin(); t != data.end(); ++t) {
		max_user_id = std::max(t->first, max_user_id);
		//time	
		for(SparseTensorBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
			max_time_id = std::max(i->first, max_time_id);
			//item_n
			for(SparseMatrixBoolean::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
				max_item_id = std::max(j->first, max_item_id);
				//item_n-1, item_n-2, ...
				for(SparseVectorBoolean::const_iterator k = j->second.begin(); k != j->second.end(); ++k) {
					max_item_id = std::max(*k, max_item_id);
				}
				num_baskets++;
			}
		}
	}
	
	std::cout << std::endl;
  	std::cout << "number of train users             " << max_user_id+1 << std::endl;
	std::cout << "number of train time             " << max_time_id+1 << std::endl;
	std::cout << "number of train item              " << max_item_id+1 << std::endl;
	std::cout << "number of train baskets             " << num_baskets << std::endl;
	
}
		
//not sure how to store these testing information yet
void Dataset::loadTest(std::string filename) {
	test_data.fromFile(filename);
	
	SparseSetBoolean test_users;
	SparseSetBoolean test_times;
	SparseSetBoolean test_items;
	
	for(SparseFourDimBoolean::const_iterator t = test_data.begin(); t != test_data.end(); ++t) {
		for(SparseTensorBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
			for(SparseMatrixBoolean::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
				test_baskets[t->first][i->first] = j->second;
			}
		}
	}
	int num_baskets = 0;
	//user	
	for(SparseFourDimBoolean::const_iterator t = test_data.begin(); t != test_data.end(); ++t) {
		test_users.insert(t->first);
		max_user_id = std::max(t->first, max_user_id);	
		//time	
		for(SparseTensorBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
			test_times.insert(i->first);
			max_time_id = std::max(i->first, max_time_id);
			//item_n
			for(SparseMatrixBoolean::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
				test_items.insert(j->first);
				test_items.insert(j->second.begin(), j->second.end());
				max_item_id = std::max(j->first, max_item_id);
				//item_n-1, ...
				for(SparseVectorBoolean::const_iterator k = j->second.begin(); k != j->second.end(); ++k) {
					max_item_id = std::max((*k), max_item_id);
				}
				num_baskets++;
			}
			// remove Assertion here
		}
	}
	std::cout << std::endl;
  	std::cout << "number of test users        " << test_users.size() << std::endl;
	std::cout << "number of test times        " << test_times.size() << std::endl;
	std::cout << "number of test items         " << test_items.size() << std::endl;
	std::cout << "number of test baskets        " << num_baskets << std::endl;
	
	std::cout << std::endl;
  	std::cout << "number of total users             " << max_user_id+1 << std::endl;
	std::cout << "number of total time             " << max_time_id+1 << std::endl;
	std::cout << "number of total item              " << max_item_id+1 << std::endl;
}		

			
#endif /*DATA_H_*/
