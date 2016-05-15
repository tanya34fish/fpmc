/*
	Base class for tag recommenders

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef NEXTBASKETRECOMMENDER_H_
#define NEXTBASKETRECOMMENDER_H_

#include <vector>
#include <assert.h>
#include <math.h>

struct WeightedItem {
	int item_id;
	double weight;	
};

bool operator<(const WeightedItem& a, const WeightedItem& b) {
    return a.weight < b.weight;
}


class NextBasketRecommender {
	public:
		NextBasketRecommender() { N = 10; }

		int N;
		
		// abstract methods to be implemented in base class
		virtual double train(Dataset& dataset) = 0;
		virtual double predict(int user_id, int time_id, int nextitem_id, const SparseVectorBoolean* basket) = 0;

		// implemented methods by NextBasketRecommender
		double evaluate(Dataset* dataset);
		virtual void predictTopItems(int user_id, int time_id, WeightedItem* items, int num_items, const SparseVectorBoolean* basket);
		virtual void saveModel(std::string filename) {};	
		virtual void loadModel(std::string filename) {};	
		virtual SparseTensorDouble testpredict(SparseTensorBoolean& baskets, int num_items, int max_items_per_basket_out);
		void savePrediction(SparseTensorBoolean& baskets, const std::string& filename, int num_items, int max_items_per_basket_out);
		inline virtual void learn(int user_id, int time_id, int nextitem_p, int nextitem_n, const SparseVectorBoolean* basket) {};
		virtual void auto_save(int iteration) {};
};

double NextBasketRecommender::evaluate(Dataset* dataset) {

	int num_baskets = 0;
	int num_items = dataset->max_item_id+1;
	double avg_mrr = 0;	
	
	WeightedItem* weighted_item = new WeightedItem[num_items];
	//user
	for(SparseFourDimBoolean::const_iterator u = dataset->test_data.begin(); u != dataset->test_data.end(); ++u) {
		//time
		for(SparseTensorBoolean::const_iterator t = u->second.begin(); t != u->second.end(); ++t) {
			//seq
			for(SparseMatrixBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
				int user_id = u->first;
				int time_id = t->first;
				int answer_item_id = i->first;
				const SparseVectorBoolean* basket = & (i->second);
				// evaluate on (user_id, time_id, basket)
				for (int j = 0; j < num_items; j++) {
					weighted_item[j].item_id = j;
				}
				predictTopItems(user_id, time_id, weighted_item, num_items, basket);

				// sort
				std::sort(weighted_item, weighted_item+num_items);
				// evaluate F1-Measure
				for (int t = 0; t < N; t++) {
					// look if this item is in the users tag list
					if (weighted_item[num_items-t-1].item_id == answer_item_id) {
						avg_mrr += 1.0/(double)(t+1);
						break;
					}
				}
				
				num_baskets++;
			}
		}
	}
	
	avg_mrr /= (double)num_baskets;
  	std::cout << std::endl;
	
	delete [] weighted_item;
	
	return avg_mrr;
}


void NextBasketRecommender::predictTopItems(int user_id, int time_id, WeightedItem* items, int num_items, const SparseVectorBoolean* basket) {
	for (int t = 0; t < num_items; t++) {
		items[t].weight = predict(user_id, time_id, items[t].item_id, basket);
	}
}


SparseTensorDouble NextBasketRecommender::testpredict(SparseTensorBoolean& baskets, int num_items, int max_items_per_basket_out) {
	WeightedItem* weighted_item = new WeightedItem[num_items];
	SparseTensorDouble prediction;
	
	for (SparseTensorBoolean::const_iterator u = baskets.begin(); u != baskets.end(); ++u) {
		for (SparseMatrixBoolean::const_iterator t = u->second.begin(); t != u->second.end(); ++t) {
			int user_id = u->first;
			int time_id = t->first;

			for (int i = 0; i < num_items; i++) {
				weighted_item[i].item_id = i;
				weighted_item[i].weight = 0;
			}				
			predictTopItems(user_id, time_id, weighted_item, num_items, &(t->second));
			std::sort(weighted_item, weighted_item+num_items);
	
			for (int i = 0; i < std::min(num_items, max_items_per_basket_out); i++) {
				prediction[user_id][time_id][weighted_item[num_items-i-1].item_id] = weighted_item[num_items-i-1].weight;
			}
		}
	}
	delete[] weighted_item;
	return prediction;
}


void NextBasketRecommender::savePrediction(SparseTensorBoolean& baskets, const std::string& filename, int num_items, int max_items_per_basket_out) {
	SparseTensorDouble prediction = testpredict(baskets, num_items, max_items_per_basket_out);		
	prediction.toFile(filename);	
}		
		


#endif /*NEXTBASKETRECOMMENDER_H_*/
