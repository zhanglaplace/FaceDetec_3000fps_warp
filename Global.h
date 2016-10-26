#ifndef _GLOBAL_H
#define _GLOBAL_H
#include <string>
enum TRAIN_METHODS {TRAIN_ADA = 1, TRAIN_FFS= 2};
enum LINEAR_CLASSIFIER {LC_ORIGINAL = 1, LC_LAC = 2, LC_FDA = 3};

struct NCascadeClassifier;

extern string 	option_filename;
extern int total_fp;
extern string classifier_filename;
extern string cascade_filename;
extern int sx;
extern int sy;
extern int max_nodes;
extern vector<int> nof;
extern int totalfeatures;
extern NCascadeClassifier* cascade;
extern SimpleClassifier* classifiers;
extern double mean_min,mean_max,sq_min,sq_max,var_min,var_max;
void LoadOptions();
void ReadInstring(ifstream& f, string& s);
void InitGlobalData(void);
void ReadRangeFile(void);
#endif