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
		�������ƣ�ReadInString
		�������ܣ���ȡtxt�ļ��ڲ�����
		����������ifstream& f,�ļ���
				  string& s �����ַ�
		���������
		����ʱ��:2016��9��13�� 10:45
		������:�ŷ�
		��ע��
*************************************************************************/
void ReadInString(ifstream& f, string& s)
{
	char buf[256];
	f.getline(buf,255,'\n');
	s = buf;
}


/*************************************************************************
		�������ƣ�InitGlobalData
		�������ܣ���ȡ����������ģ��
		����������
		���������
		����ʱ��:2016��9��13�� 10��40
		������:�ŷ�
		��ע��
*************************************************************************/
void InitGlobalData()
{
	LoadOptions();
	cascade=new NCascadeClassifier;
	cascade->LoadDefaultCascade();
	ReadRangeFile();
}

/*************************************************************************
		�������ƣ�LoadOptions
		�������ܣ���ȡ�ļ�����·����ͼ�������Ϣ
		����������
		���������
		����ʱ��:2016��9��13�� 10��40
		������:�ŷ�
		��ע��
*************************************************************************/
void LoadOptions()
{
	ifstream f;
	int i;//ѭ������

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

	//��������
	f>>totalfeatures; 
	
	//���ڵ���
	f>>max_nodes;
	nof.resize(max_nodes);

	for(i = 0;i < max_nodes;i++)
		f>>nof[i];//ÿ���ڵ��������Ŀ
	f.close();
}

/*************************************************************************
		�������ƣ�ReadRangeFile
		�������ܣ���ȡ����ͼ��Ļ���������Ϣ
		����������
		���������
		����ʱ��:2016��9��13�� 10��40
		������:�ŷ�
		��ע��
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


