#ifndef _SIMPLECLASSIFIER_H
#define _SIMPLECLASSIFIER_H
#include <fstream>
using namespace std;
struct SimpleClassifier
{
	double thresh;
	double error;
	int parity;
	int type; // which type of feature?
	int x1,x2,x3,x4,y1,y2,y3,y4;

	inline const double GetOneFeature(const IntImage& im) const;
	inline const double GetOneFeatureTranslation(double** const data,const int y) const;
	inline const int Apply(const double value) const;
	inline const int Apply(const IntImage& im) const;
	void WriteToFile(ofstream& f) const;
	void ReadFromFile(ifstream& f);
};


/*************************************************************************
		�������ƣ�Apply
		�������ܣ�����Ԥ��ֵ
		����������const IntImage& im //����haar������ָ��ͼ��
		���������int  = 1���� ���������
		����ʱ��:2016��9��13�� 15:25
		������:�ŷ�
		��ע��
*************************************************************************/
const int SimpleClassifier::Apply(const IntImage& im) const
{
	//parity��ʾ������
	if(parity == 1)
		return (GetOneFeature(im)<thresh)?1:0;//��Haar����������ֵ�Ƚ�
	else
		return (GetOneFeature(im)>=thresh)?1:0;
}



/*************************************************************************
		�������ƣ�Apply
		�������ܣ�����Ԥ��ֵ
		����������const double value //ָ�����͵�haar�������ز�/ƽ������
		���������int  = 1���� ���������
		����ʱ��:2016��9��13�� 15:25
		������:�ŷ�
		��ע��
*************************************************************************/
const int SimpleClassifier::Apply(const double value) const
{
	//parity��������
	if(parity == 1)
		return (value<thresh)?1:0;
	else
		return (value>=thresh)?1:0;
}


/*************************************************************************
		�������ƣ�GetOneFeature
		�������ܣ���ȡָ�����͵�haar����ֵ
		����������const IntImage& im ����ͼ��
		���������haar����ֵ
		����ʱ��:2016��9��13�� 15:28
		������:�ŷ�
		��ע��
*************************************************************************/
const double SimpleClassifier::GetOneFeature(const IntImage& im) const
{
	double f1;
	double** data = im.data;

	switch(type)
	{//haar��������
	case 0:
		f1 =   data[x1][y3] - data[x1][y1] + data[x3][y3] - data[x3][y1]
			 + 2*(data[x2][y1] - data[x2][y3]);
		break;
	case 1:
		f1 =   data[x3][y1] + data[x3][y3] - data[x1][y1] - data[x1][y3]
			 + 2*(data[x1][y2] - data[x3][y2]);
		break;
	case 2:
		f1 =   data[x1][y1] -data[x1][y3] + data[x4][y3] - data[x4][y1]
			 + 3*(data[x2][y3] - data[x2][y1] + data[x3][y1] - data[x3][y3]);
		break;
	case 3:
		f1 =   data[x1][y1] - data[x1][y4] + data[x3][y4] - data[x3][y1]
			 + 3*(data[x3][y2] - data[x3][y3] + data[x1][y3] - data[x1][y2] );
		break;
	case 4:
		f1 =   data[x1][y1] + data[x1][y3] + data[x3][y1] + data[x3][y3]
			 - 2*(data[x2][y1] + data[x2][y3] + data[x1][y2] + data[x3][y2])
			 + 4*data[x2][y2];
		break;
	}
	return f1/im.variance;
}


/*************************************************************************
		�������ƣ�GetOneFeatureTranslation
		�������ܣ���ȡָ�����͵ģ�ָ�������haar����ֵ
		����������couble** const data ����ͼ��ĵ�ַ
				const int y  ����ƫ��ֵ
		���������haar����ֵ
		����ʱ��:2016��9��13�� 15:35
		������:�ŷ�
		��ע��
*************************************************************************/
const double SimpleClassifier::GetOneFeatureTranslation(double** const data,const int y) const
{
	double f1=0;

	switch(type)
	{
	case 0:
		f1 =   data[x1][y+y3] - data[x1][y+y1] + data[x3][y+y3] - data[x3][y+y1]
			 + 2*(data[x2][y+y1] - data[x2][y+y3]);
		break;
	case 1:
		f1 =   data[x3][y+y1] + data[x3][y+y3] - data[x1][y+y1] - data[x1][y+y3]
			 + 2*(data[x1][y+y2] - data[x3][y+y2]);
		break;
	case 2:
		f1 =   data[x1][y+y1] - data[x1][y+y3] + data[x4][y+y3] - data[x4][y+y1]
			 + 3*(data[x2][y+y3] - data[x2][y+y1] + data[x3][y+y1]  - data[x3][y+y3]);
		break;
	case 3:
		f1 =   data[x1][y+y1] - data[x1][y+y4] + data[x3][y+y4] - data[x3][y+y1]
			 + 3*(data[x3][y+y2] - data[x3][y+y3] + data[x1][y+y3] - data[x1][y+y2]);
		break;
	case 4:
		f1 =   data[x1][y+y1] + data[x1][y+y3] + data[x3][y+y1] + data[x3][y+y3]
			 - 2*(data[x2][y+y1] + data[x2][y+y3] + data[x1][y+y2] + data[x3][y+y2])
			 + 4*data[x2][y+y2];
		break;
	default:
		break;
	}
	return f1;
}

#endif