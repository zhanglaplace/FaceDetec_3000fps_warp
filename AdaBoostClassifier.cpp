#include <fstream>
#include <vector>
#include <math.h>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"
AdaBoostClassifier::AdaBoostClassifier():count(0),thresh(0.0),alphas(NULL),scs(NULL)
{
}

AdaBoostClassifier::~AdaBoostClassifier()
{
	Clear();
}

/*************************************************************************
		函数名称：Clear
		函数功能：单个弱分类器的内存释放和清零
		函数参数：
		函数输出：
		创建时间:2016年9月13日 15:22
		创建人:张峰
		备注：
*************************************************************************/
void AdaBoostClassifier::Clear()
{
	delete[] scs; 
	scs=NULL;
	delete[] alphas; 
	alphas=NULL;
	count = 0;
	thresh = 0.0;
}

/*************************************************************************
		函数名称：WriteToFile
		函数功能：单个弱分类器的内存释放和清零
		函数参数：
		函数输出：
		创建时间:2016年9月13日 15:22
		创建人:张峰
		备注：
*************************************************************************/
void AdaBoostClassifier::WriteToFile(ofstream& f) const
{
	//一个抢分类器的级联参数的写入
	f<<count<<endl;
	f<<thresh<<endl;
	for(int i=0;i<count;i++) 
		f<<alphas[i]<<" ";
	f<<endl;
	for(int i=0;i<count;i++) 
		scs[i].WriteToFile(f);
	f<<endl;
}
/*************************************************************************
		函数名称：ReadFromFile
		函数功能：读取每个强分类器的弱分类器参数
		函数参数：
		函数输出：
		创建时间:2016年9月13日 10：58
		创建人:张峰
		备注：
*************************************************************************/
void AdaBoostClassifier::ReadFromFile(ifstream& f)
{
	Clear();
	//特征个数
	f>>count;
	scs = new SimpleClassifier[count]; 
	alphas = new double[count]; 
	//阈值
	f>>thresh;
	//alphas值
	for(int i=0;i<count;i++) 
		f>>alphas[i]; 
	f.ignore(256,'\n');
	for(int i=0;i<count;i++) 
		scs[i].ReadFromFile(f); 
	f.ignore(256,'\n');
}