/*
	Tagrecommender based on the PITF method.

	Based on the publication(s):
	Steffen Rendle, Lars Schmidt-Thieme (2010): Pairwise Interaction Tensor Factorization for Personalized Tag Recommendation, in Proceedings of the Third ACM International Conference on Web Search and Data Mining (WSDM 2010), ACM.

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2010-12-10

	Copyright 2010 Steffen Rendle, see license.txt for more information
*/

#ifndef BASKET_REC_FPMC_H_
#define BASKET_REC_FPMC_H_

#include "BPRLearner.h"
#include "../../util/util.h"
using namespace std;

class NextBasketRecommenderFPMC : public NextBasketRecommender {
	protected:	
		DMatrixDouble V_UI, V_IU, V_IL, V_LI, V_MI, V_IM;	
	public:	
		int loss_function;
		int num_neg_samples;
		int num_iterations;
		double learn_rate;

		int num_feature;
		double regular_UI, regular_IU, regular_IL, regular_LI, regular_MI, regular_IM;
		int num_user;
		int num_item;
	
		double init_stdev;
		double init_mean;
				
		virtual double train(Dataset& dataset) {
			BasketLearnerBPR learner;
			learner.num_iterations = this->num_iterations;
			learner.num_neg_samples = this->num_neg_samples;
			double best_mrr = learner.train(dataset, *this);
			return best_mrr;
		}
				
		virtual void init() {
			this->V_UI.setSize(num_user, num_feature);
			this->V_IU.setSize(num_item, num_feature);			
			this->V_IL.setSize(num_item,  num_feature);
			this->V_LI.setSize(num_item,  num_feature);
			this->V_MI.setSize(num_item,  num_feature);
			this->V_IM.setSize(num_item,  num_feature);

			this->V_UI.init(init_mean, init_stdev);
			this->V_IU.init(init_mean, init_stdev);			
			this->V_IL.init(init_mean, init_stdev);
			this->V_LI.init(init_mean, init_stdev);			
			this->V_MI.init(init_mean, init_stdev);
			this->V_IM.init(init_mean, init_stdev);
		}
		
		//TODO
		virtual void predictTopItems(int user_id, int time_id, WeightedItem* items, int num_items, const SparseVectorBoolean* basket) {
			for (int t_x = 0; t_x < num_items; t_x++) {
      				items[t_x].weight = predict(user_id, time_id, items[t_x].item_id, basket);
			}
		}

		virtual double predict(int user_id, int time_id, int nextitem_id, const SparseVectorBoolean* basket) {
			double result = 0;
			double mf_dot = 0;
			double fmc_dot = 0;
			for (int f = 0; f < num_feature; f++) {
				mf_dot += this->V_UI(user_id,f) * this->V_IU(nextitem_id,f);
			}
			//item_n-1
			SparseVectorBoolean::const_iterator iter = basket->begin();
			
			for (int f = 0; f < num_feature; f++) {
				fmc_dot += this->V_IL(nextitem_id,f) * this->V_LI((*iter),f);
				if(basket->size() > 1) {
					fmc_dot += this->V_IM(nextitem_id,f) * this->V_MI(*(iter+1),f);
				}
			}

			result += mf_dot + fmc_dot;
			
			if (isnan(result)) {
				throw "Prediction is NAN";
			}
			return result;
		}
		
		inline virtual void learn(int user_id, int time_id, int nextitem_p, int nextitem_n, const SparseVectorBoolean* basket) {
			
			double x_utnip = predict(user_id, time_id, nextitem_p, basket);
     		double x_utnin = predict(user_id, time_id, nextitem_n, basket);
     		double normalizer = BasketLearner::partial_loss(loss_function, x_utnip - x_utnin);
     		// update the features
     		for (int f = 0; f < num_feature; f++) {
     			double UI_u_f = this->V_UI(user_id,f);
     			double IU_p_f = this->V_IU(nextitem_p,f);
     			double IU_n_f = this->V_IU(nextitem_n,f);
				
     			this->V_UI(user_id, f) += learn_rate * (normalizer * (IU_p_f - IU_n_f) - regular_UI * V_UI(user_id, f));
     			this->V_IU(nextitem_p, f) += learn_rate * (normalizer * UI_u_f - regular_IU * V_IU(nextitem_p, f));
     			this->V_IU(nextitem_n, f) += learn_rate * (normalizer * (-UI_u_f) - regular_IU * V_IU(nextitem_n, f));
     		}
     		

     		for (int f = 0; f < num_feature; f++) {
	     		double eta = 0.0;
	     		SparseVectorBoolean::const_iterator iter = basket->begin();

				eta = this->V_LI((*iter), f);
				double IL_p_f = this->V_IL(nextitem_p,f);
     			double IL_n_f = this->V_IL(nextitem_n,f);
     			double tmp = (this->V_IL(nextitem_p,f) - this->V_IL(nextitem_n,f)) / 1.0;
     			
				double LI_item_f = this->V_LI((*iter),f);

				this->V_IL(nextitem_p,f) += learn_rate * (normalizer * eta - regular_IL * IL_p_f);
     			this->V_IL(nextitem_n,f) += learn_rate * (normalizer * (-eta) - regular_IL * IL_n_f);
     			
				this->V_LI((*iter), f) += learn_rate * (normalizer * tmp - regular_LI * LI_item_f);
     			
				if(basket->size() > 1) {
					double tmp_im = (this->V_IM(nextitem_p,f) - this->V_IM(nextitem_n,f)) / 1.0;
					double MI_item_f = this->V_MI(*(iter + 1),f);
					double IM_p_f = this->V_IM(nextitem_p,f);
					double IM_n_f = this->V_IM(nextitem_n,f);
					this->V_MI(*(iter + 1), f) += learn_rate * (normalizer * tmp_im - regular_MI * MI_item_f);
					this->V_IM(nextitem_p,f) += learn_rate * (normalizer * MI_item_f - regular_IM * IM_p_f);
					this->V_IM(nextitem_n,f) += learn_rate * (normalizer * (-MI_item_f) - regular_IM * IM_n_f);
				}
     		}
     		
     	}

};

#endif /*BASKET_REC_FPMC_H_*/
