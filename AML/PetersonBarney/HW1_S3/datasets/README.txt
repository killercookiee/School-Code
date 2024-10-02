All necessary data for evaluation is available in this folder (in .csv format, the separator is ',').

	- Files with names ending in "1" contain a standard 80%-20% train-test split of all available data.

	- Files with names ending in "2" contain a train-test split where the test set includes only unstarred data, while the train set includes both starred (where the 'phoneme_in_ascii' value starts with '*') and unstarred data.

	- Files with names ending in "3" contain a train-test split where the test set includes all starred data, and the train set includes all unstarred data.

	- Files with names ending in "4" contain a train-test split where neither the train set nor the test set contains starred data.

In all files, the columns are as follows: 'gender', 'speaker', 'phoneme', 'phoneme_in_ascii', 'F0', 'F1', 'F2', 'F3'.