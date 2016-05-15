/*
	Generic BPR learning algorithm for tag recommendation.

	Based on the publication(s):
	Steffen Rendle, Christoph Freudenthaler, Zeno Gantner, Lars Schmidt-Thieme (2009): BPR: Bayesian Personalized Ranking from Implicit Feedback, in Proceedings of the 25th Conference on Uncertainty in Artificial Intelligence (UAI 2009)
	Steffen Rendle (2010): Context-aware Ranking with Factorization Models, in Series on Studies in Computational Intelligence, Springer, ISBN: 3642168973.

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef BPRLEARNER_H_
#define BPRLEARNER_H_

#include "Data.h"
#include "NextBasketRecommender.h"

int LOSS_FUNCTION_SIGMOID = 0;
int LOSS_FUNCTION_LN_SIGMOID = 1;
using namespace std;

class BasketLearner {
	public:
		static inline double partial_loss(int loss_function, double x) {
			if (loss_function == LOSS_FUNCTION_SIGMOID) {
            			double sigmoid_tp_tn = (double) 1/(1+exp(-x));
              			return sigmoid_tp_tn*(1-sigmoid_tp_tn);
			} else if (loss_function == LOSS_FUNCTION_LN_SIGMOID) {
				double exp_x = exp(-x);
     				return exp_x / (1 + exp_x);
     			} else {
				assert((loss_function == LOSS_FUNCTION_LN_SIGMOID) || (loss_function == LOSS_FUNCTION_SIGMOID));	
			}             			
			return 0;
		}
		virtual double train(Dataset& dataset, NextBasketRecommender& rec) = 0;	
			
};

class BasketLearnerBPR : BasketLearner{
	private:
		struct BasketCase {
			int user_id;
			int time_id;
			int nextitem_id;
			const SparseVectorBoolean* basket;
		};	
		int num_item;	
		inline int drawNextItemNeg(Dataset& dataset, int nextitem_positive);
	public:
		int num_iterations;
		int num_neg_samples;
		virtual double train(Dataset& dataset, NextBasketRecommender& rec);	
};

double BasketLearnerBPR::train(Dataset& dataset, NextBasketRecommender& rec) {
	double total_time = getusertime();

	num_item = dataset.max_item_id + 1;
	
	std::cout << "Training BPR (Case-Update):"
			<< " num_iter=" << num_iterations
			<< " neg_samples=" << num_neg_samples
			<< std::endl;
			
	double f_best_mrr_measure = -1;
	int f_best_mrr_iteridx = -1;	

	// build basket case db: {user, time, {next_item, itemset}}
	int num_basket_case = 0; 
	//user
	for(SparseFourDimBoolean::const_iterator t = dataset.data.begin(); t != dataset.data.end(); ++t) {
		//time
		for(SparseTensorBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
			num_basket_case += i->second.size();
		}
	}
	std::cout << "num_basket_case:" << num_basket_case << endl;
	BasketCase* basket_case = new BasketCase[num_basket_case];
	{
		int cntr = 0;
		//user
		for(SparseFourDimBoolean::const_iterator t = dataset.data.begin(); t != dataset.data.end(); ++t) {
			//time
			for(SparseTensorBoolean::const_iterator i = t->second.begin(); i != t->second.end(); ++i) {
				//next_item
				for(SparseMatrixBoolean::const_iterator j = i->second.begin(); j != i->second.end(); ++j) {
					basket_case[cntr].user_id = t->first;
					basket_case[cntr].time_id = i->first;
					basket_case[cntr].basket = & (j->second);
					basket_case[cntr].nextitem_id = j->first;
					cntr++;
				}
			}
		}
	}


	long long num_draws_per_iteration = num_basket_case * num_neg_samples;
		
	for (int iteration = 0; iteration < num_iterations; iteration++) {
		double iteration_time = getusertime();
		for (int draw = 0; draw < num_draws_per_iteration; draw++) {
			int p  = rand() % num_basket_case;
			int u  = basket_case[p].user_id;
			int t  = basket_case[p].time_id;
			int ni_p = basket_case[p].nextitem_id;
			int ni_n = drawNextItemNeg(dataset, ni_p);
			rec.learn(u, t, ni_p, ni_n, basket_case[p].basket);
		}
		
		iteration_time = (getusertime() - iteration_time);
		std::cout << "Time: " << iteration_time << " / ";

		std::cout << "Iteration(" << iteration << "/" << num_iterations << ")  ";
		double this_mrr_measure = rec.evaluate(&dataset);
		f_best_mrr_measure = std::max(this_mrr_measure, f_best_mrr_measure);
		
		std::cout << "MRR:  " << this_mrr_measure << std::endl;
		std::cout << "best MRR:  " << f_best_mrr_measure << std::endl;

		//rec.auto_save();
	}
	delete [] basket_case;
	
	total_time = (getusertime() - total_time);
	std::cout << "training time: " << total_time << " s" << std::endl;
	
	return f_best_mrr_measure;
}


inline int BasketLearnerBPR::drawNextItemNeg(Dataset& dataset, int nextitem_positive) {
	int nextitem_negative;
	do {
		nextitem_negative = rand() % num_item;		
	} while (nextitem_negative == nextitem_positive);
	return nextitem_negative;
}


#endif /*BPRLEARNER_H_*/
