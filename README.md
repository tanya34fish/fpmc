# Basketrec
## Next-Basket Recommendation
* This project implemented a BPR-based recommendation.
* Origin paper: Factorizing personalized Markov chains for next-basket recommendation (WWW, 2010)

## Implementation
* This project is modified from codes of Tag Recommender which is also proposed by Steffen Rendle
* please see this website for details.
* http://www.informatik.uni-konstanz.de/rendle/software/tag-recommender/
* Reference: Steffen Rendle, Lars Schmidt-Thieme (2010): Pairwise Interaction Tensor Factorization for Personalized Tag Recommendation, in Proceedings of the Third ACM International Conference on Web Search and Data Mining (WSDM 2010), ACM.

## Usage
* cd trigramrec-1.2.src
* make
* ./run_cv.sh 2 ipad  # This will run the code

## Dataset
* cross_validation/train contains train files and cross_validation/test contains test files
* Format: userId sequenceId sequenceLength app_0 app_1 app_2 .... app_n
