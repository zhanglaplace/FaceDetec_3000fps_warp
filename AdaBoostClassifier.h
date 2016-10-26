#ifndef _ADABOOSTCLASSIFIER_H
#define _ADABOOSTCLASSIFIER_H
#include "SimpleClassifier.h"
#include <fstream>
enum GOAL_METHODS {GOAL_METHOD_MIN_ERROR = 1, GOAL_METHOD_VALID_DR = 2, GOAL_METHOD_FIX_FP = 3};

struct AdaBoostClassifier
{
	int count;
	SimpleClassifier* scs;//��һ��������
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
		�������ƣ�GetValue
		�������ܣ���ȡ�������������ĵ�һǿ��������Ԥ��ֵ
		����������const IntImage& im������ͼ��
		���������Ԥ��ֵ
		����ʱ��:2016��9��13�� 15:32
		������:�ŷ�
		��ע��
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
		�������ƣ�ApplyImagePatch
		�������ܣ���ȡ�������������ĵ�һǿ�����������ս��
		����������const IntImage& im ����ͼ��
		���������1 ==������0==������
		����ʱ��:2016��9��13�� 15:34
		������:�ŷ�
		��ע��
*************************************************************************/
int AdaBoostClassifier::ApplyImagePatch(const IntImage& im) const
{
	return (GetValue(im)>=thresh)?1:0;
}

#endif