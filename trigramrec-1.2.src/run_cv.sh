index=$1
datatype=$2
mkdir -p out_${datatype}
./bin/basketrec -test ../cross_validation/test/${datatype}_test_seq_${index}.txt -train ../cross_validation/train/${datatype}_train_seq_${index}.txt -method fpmc -out out_${datatype}/${datatype}_pred_${index}.txt -mrr_out out_${datatype}/${datatype}_mrr_${index}.txt -iter 100
