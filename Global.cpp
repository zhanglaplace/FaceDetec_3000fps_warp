#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <math.h>
#include <float.h>
#include <string>
#include <ctime>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"

string option_filename = "options.txt";
string cascade_filename;
string classifier_filename;
int sx; //height
int sy; //width
int max_nodes;
vector<int> nof;
int totalfeatures;
int total_fp;
NCascadeClassifier* cascade=NULL;
SimpleClassifier* classifiers=NULL;
double mean_min, mean_max, sq_min, sq_max, var_min, var_max;

/*************************************************************************
		函数名称：ReadInString
		函数功能：读取txt文件内部内容
		函数参数：ifstream& f,文件流
				  string& s 保存字符
		函数输出：
		创建时间:2016年9月13日 10:45
		创建人:张峰
		备注：
*************************************************************************/
void ReadInString(ifstream& f, string& s)
{
	char buf[256];
	f.getline(buf,255,'\n');
	s = buf;
}


/*************************************************************************
		函数名称：InitGlobalData
		函数功能：读取人脸分类器模型
		函数参数：
		函数输出：
		创建时间:2016年9月13日 10：40
		创建人:张峰
		备注：
*************************************************************************/
void InitGlobalData()
{
	LoadOptions();
	cascade=new NCascadeClassifier;
	cascade->LoadDefaultCascade();
	ReadRangeFile();
}

/*************************************************************************
		函数名称：LoadOptions
		函数功能：读取文件配置路径和图像基本信息
		函数参数：
		函数输出：
		创建时间:2016年9月13日 10：40
		创建人:张峰
		备注：
*************************************************************************/
void LoadOptions()
{
	ifstream f;
	int i;//循环遍历

	//option.txt
	f.open(option_filename);
	
	//classifiers.txt
	ReadInString(f,classifier_filename);

	//cascade.txt
	ReadInString(f,cascade_filename);
	
	//width;
	f>>sx;

	//height
	f>>sy;

	//特征总数
	f>>totalfeatures; 
	
	//最大节点数
	f>>max_nodes;
	nof.resize(max_nodes);

	for(i = 0;i < max_nodes;i++)
		f>>nof[i];//每个节点的特征数目
	f.close();
}

/*************************************************************************
		函数名称：ReadRangeFile
		函数功能：读取人脸图像的基本特性信息
		函数参数：
		函数输出：
		创建时间:2016年9月13日 10：40
		创建人:张峰
		备注：
*************************************************************************/
void ReadRangeFile(void)
{
	string filename;
	ifstream f;

	filename = cascade_filename + ".range";
	f.open(filename);
	f>>mean_min>>mean_max>>sq_min>>sq_max>>var_min>>var_max;
	f.close();
}


