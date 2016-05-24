/*
	Tag recommender tool

	Author:   Steffen Rendle, http://www.libfm.org/
	modified: 2011-07-14

	Copyright 2010-2011 Steffen Rendle, see license.txt for more information
*/
	
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <string>
#include <iterator>
#include <algorithm>
#include <iomanip>
#include "../util/util.h"
#include "../util/cmdline.h"

#include "src/Data.h"
#include "src/basket_rec_fpmc.h"


using namespace std;

int main(int argc, char **argv) { 
 	
 	srand ( time(NULL) );
	try {
		CMDLine cmdline(argc, argv);
		std::cout << "Facotorizing Personalized Markov Chains (FPMC)" << std::endl;
		std::cout << "  Version: 1.0" << std::endl;
		std::cout << "  Author:  Tanya Wang, r03922032@ntu.edu.tw" << std::endl;
		std::cout << "  License: Free for academic use. See license.txt." << std::endl;
		std::cout << "  Modified from Tag Recommender source codes by Steffen Rendle" << std::endl;
		std::cout << "----------------------------------------------------------------------------" << std::endl;

		const std::string param_train_file	= cmdline.registerParameter("train", "filename for training data [MANDATORY]");
		const std::string param_test_file	= cmdline.registerParameter("test", "filename for test data [MANDATORY]");
		const std::string param_out		= cmdline.registerParameter("out", "filename for output; default=''");
		const std::string param_mrr_out		= cmdline.registerParameter("mrr_out", "filename for bst mrr output; default=''");

		const std::string param_num_pred_out	= cmdline.registerParameter("num_out", "how many recommended items per (user,time,basket) should be written; default=10");

		const std::string param_method		= cmdline.registerParameter("method", "method: 'fpmc' [MANDATORY]");
		const std::string param_dim		= cmdline.registerParameter("dim", "dim of factorization; default=64");
		const std::string param_regular_UI		= cmdline.registerParameter("regular_UI", "regularization; default=0.01");
		const std::string param_regular_IU		= cmdline.registerParameter("regular_IU", "regularization; default=0.01");
		const std::string param_regular_IL		= cmdline.registerParameter("regular_IL", "regularization; default=0.01");
		const std::string param_regular_LI		= cmdline.registerParameter("regular_LI", "regularization; default=0.01");
		
		const std::string param_regular_MI		= cmdline.registerParameter("regular_MI", "regularization; default=0.01");
		const std::string param_regular_IM		= cmdline.registerParameter("regular_IM", "regularization; default=0.01");

		const std::string param_init_stdev	= cmdline.registerParameter("init_stdev", "stdev for initialization of 2-way factors; default=0.01");			
		const std::string param_num_iter	= cmdline.registerParameter("iter", "number of iterations for SGD; default=100");
		const std::string param_learn_rate	= cmdline.registerParameter("learn_rate", "learn_rate for SGD; default=0.01");
		const std::string param_num_sample      = cmdline.registerParameter("num_sample", "number of the pair samples drawn for each training tuple, default 100");

		const std::string param_help            = cmdline.registerParameter("help", "this screen");

		if (cmdline.hasParameter(param_help) || (argc == 1)) {
			cmdline.print_help();
			return 0;
		}
		cmdline.checkParameters();

		// (1) Load the data
		std::cout << "Loading train...\t";
		Dataset dataset = Dataset(cmdline.getValue(param_train_file));
		std::cout << "Loading test... \t";
	  	dataset.loadTestSplit(cmdline.getValue(param_test_file));
		
		// (2) Setup the learning method:
		NextBasketRecommender* rec;
		if (! cmdline.getValue(param_method).compare("fpmc")) {
			std::cout << "Method: FPMC (BPR)" << std::endl;
	 		NextBasketRecommenderFPMC *fpmc = new NextBasketRecommenderFPMC();
 			
			fpmc->loss_function = LOSS_FUNCTION_LN_SIGMOID;
			fpmc->learn_rate = cmdline.getValue(param_learn_rate, 0.01);
			fpmc->num_neg_samples = cmdline.getValue(param_num_sample, 100);
	 		fpmc->num_iterations = cmdline.getValue(param_num_iter, 100);
				
			fpmc->num_user = dataset.max_user_id+1;
			fpmc->num_item = dataset.max_item_id+1;
		
			fpmc->init_mean = 0;
			fpmc->init_stdev = cmdline.getValue(param_init_stdev, 0.01);
			
			fpmc->num_feature = cmdline.getValue(param_dim, 64);
			fpmc->regular_UI = cmdline.getValue(param_regular_UI, 0.01);
			fpmc->regular_IU = cmdline.getValue(param_regular_IU, 0.01);
			fpmc->regular_IL = cmdline.getValue(param_regular_IL, 0.01);
			fpmc->regular_LI = cmdline.getValue(param_regular_LI, 0.01);
			
			fpmc->regular_MI = cmdline.getValue(param_regular_MI, 0.01);
			fpmc->regular_IM = cmdline.getValue(param_regular_IM, 0.01);
			
			fpmc->init();
			rec = fpmc;

		} else {
			throw "unknown method";
		}
		rec->N = 10;

		// (3) learning
		double best_mrr = 0.0;
		best_mrr = rec->train(dataset);
		std::cout << "model trained" << std::endl;std::cout.flush();
	 	//double avg_mrr = rec->evaluate(&dataset);
	 	//std::cout << "MRR on test data: " << avg_mrr << std::endl;std::cout.flush();
	 	
		// (4) Save prediction
		if (cmdline.hasParameter(param_out)) {
			rec->savePrediction(dataset.test_baskets, cmdline.getValue(param_out), dataset.max_item_id+1, cmdline.getValue(param_num_pred_out, 10));	 	
		}
		// (5) Save best MRR
		if (cmdline.hasParameter(param_mrr_out)) {
			std::ofstream out_file (cmdline.getValue(param_mrr_out).c_str());
			if (out_file.is_open())	{
				out_file << best_mrr << std::endl;
				out_file.close();
			} else {
				throw "Unable to open file " + cmdline.getValue(param_mrr_out);
			}	
		}

	} catch (std::string &e) {
		std::cerr << e << std::endl;
	}

}
