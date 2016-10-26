#ifndef _ADABOOSTCLASSIFIER_H
#define _ADABOOSTCLASSIFIER_H
#include "SimpleClassifier.h"
#include <fstream>
enum GOAL_METHODS {GOAL_METHOD_MIN_ERROR = 1, GOAL_METHOD_VALID_DR = 2, GOAL_METHOD_FIX_FP = 3};

struct AdaBoostClassifier
{
	int count;
	SimpleClassifier* scs;//单一弱分类器
	double* alphas;
	double thresh;

	AdaBoostClassifier();
	~AdaBoostClassifier();
	void Clear(void);

	
	void WriteToFile(ofstream& f) const;
	void ReadFromFile(ifstream& f);

	inline double GetValue(const IntImage& im) const;
	inline int ApplyImagePatch(const IntImage& im) const;

};

/*************************************************************************
		函数名称：GetValue
		函数功能：获取弱分类器级联的单一强分类器的预测值
		函数参数：const IntImage& im；输入图像
		函数输出：预测值
		创建时间:2016年9月13日 15:32
		创建人:张峰
		备注：
*************************************************************************/
double AdaBoostClassifier::GetValue(const IntImage& im) const
{
	int i;
	double value;

	value = 0.0;
	for(i=0;i<count;i++) 
		value += alphas[i]*scs[i].Apply(im);
	
	return value;
}

/*************************************************************************
		函数名称：ApplyImagePatch
		函数功能：获取弱分类器级联的单一强分类器的最终结果
		函数参数：const IntImage& im 输入图像
		函数输出：1 ==人脸，0==非人脸
		创建时间:2016年9月13日 15:34
		创建人:张峰
		备注：
*************************************************************************/
int AdaBoostClassifier::ApplyImagePatch(const IntImage& im) const
{
	return (GetValue(im)>=thresh)?1:0;
}

#endif