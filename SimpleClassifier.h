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
		函数名称：Apply
		函数功能：返回预测值
		函数参数：const IntImage& im //待求haar特征的指定图像
		函数输出：int  = 1人脸 否则非人脸
		创建时间:2016年9月13日 15:25
		创建人:张峰
		备注：
*************************************************************************/
const int SimpleClassifier::Apply(const IntImage& im) const
{
	//parity表示方向性
	if(parity == 1)
		return (GetOneFeature(im)<thresh)?1:0;//求Haar特征并与阈值比较
	else
		return (GetOneFeature(im)>=thresh)?1:0;
}



/*************************************************************************
		函数名称：Apply
		函数功能：返回预测值
		函数参数：const double value //指定类型的haar特征像素差/平均像素
		函数输出：int  = 1人脸 否则非人脸
		创建时间:2016年9月13日 15:25
		创建人:张峰
		备注：
*************************************************************************/
const int SimpleClassifier::Apply(const double value) const
{
	//parity表明方向
	if(parity == 1)
		return (value<thresh)?1:0;
	else
		return (value>=thresh)?1:0;
}


/*************************************************************************
		函数名称：GetOneFeature
		函数功能：获取指定类型的haar特征值
		函数参数：const IntImage& im 输入图像
		函数输出：haar特征值
		创建时间:2016年9月13日 15:28
		创建人:张峰
		备注：
*************************************************************************/
const double SimpleClassifier::GetOneFeature(const IntImage& im) const
{
	double f1;
	double** data = im.data;

	switch(type)
	{//haar特征类型
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
		函数名称：GetOneFeatureTranslation
		函数功能：获取指定类型的，指定区域的haar特征值
		函数参数：couble** const data 输入图像的地址
				const int y  坐标偏移值
		函数输出：haar特征值
		创建时间:2016年9月13日 15:35
		创建人:张峰
		备注：
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