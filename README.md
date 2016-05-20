# Basketrec
## Next-Basket Recommendation
* This project implemented a BPR-based recommendation. It extends the FPMC model by considering 2 previous baskets to predict next basket.
* Origin paper: Factorizing personalized Markov chains for next-basket recommendation (WWW, 2010)

## Implementation
* This project is modified from codes of Tag Recommender which is also proposed by Steffen Rendle.
* please see this website for details.
* http://www.informatik.uni-konstanz.de/rendle/software/tag-recommender/
* Reference: Steffen Rendle, Lars Schmidt-Thieme (2010): Pairwise Interaction Tensor Factorization for Personalized Tag Recommendation, in Proceedings of the Third ACM International Conference on Web Search and Data Mining (WSDM 2010), ACM.

## Usage
* cd trigramrec-1.2.src
* make
* ./run_cv.sh 1 ipad  # This will run the code that consider 2 previous basket to predict next basket.

## Dataset
* cross_validation/train contains train files and cross_validation/test contains test files
* Format: userId sequenceId sequenceLength app_0 app_1 app_2 .... app_n

## More Details of Implementation.
* Please see the following.
* src/basketrec/basketrec.cpp:
  1. this is the main function
  2. load data (use src/basketrec/src/Data.h)
  3. instantiate a NextBasketRecommender object. (use src/basketrec/src/NextBasketRecommender.h)
  4. instantiate a NextBasketRecommenderFPMC object.
  5. call "NextBasketRecommenderFPMC::train" to set up parameters and training.
  6. call "NextBasketRecommender::evaluate" for evaluation on test data.
  7. call "NextBasketRecommender::savePredict" to output for the prediction on test data.

* NextBasketRecommender
  * "NextBasketRecommender" is the base class. It contains basic function you will need.
  * You can extend this class when you want to implement other learning methods other than FPMC.
  * NextBasketRecommender::evaluate - This evaluates the results on the test data. We use MRR for evaluation metric.
  * NextBasketRecommender::predictTopItems - given user and basket, predict scores for every possible items.
  * NextBasketRecommender::testpredict - use for output the predict result.
  * NextBasketRecommender::savePrediction - call NextBasketRecommender::testpredict and output to file.

* basket_rec_fpmc.h: 
  * "NextBasketRecommenderFPMC" is a derived class from "NextBasketRecommender".
  * It contains important parameters and functions you will need during the learning process.
  * NextBasketRecommenderFPMC::train - This will instantiate a "BasketLearnerBPR" object.
  * NextBasketRecommenderFPMC::init - assign the parameters and set up the latent matrices we need to learn.
  * NextBasketRecommenderFPMC::predict - given user, basket and an item, predict the score.
  * NextBasketRecommenderFPMC::learn - the main learning process.

* BasketLearnerBPR:
  * "BasketLearnerBPR" is a class used only for BPR setting.
  * BasketLearnerBPR::train - calculate how many baskets from training data. Implement the outer iteration and sampling for BPR.
  * on line 111, it will call "NextBasketRecommenderFPMC::learn" for the learning process. 
  * on line 118, it will call "NextBasketRecommender::evaluate" for the evaluation.
  * BasketLearnerBPR::drawNextItemNeg - sample a negative item (used for BPR)
