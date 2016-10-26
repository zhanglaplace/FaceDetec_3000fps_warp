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
		�������ƣ�Clear
		�������ܣ����������������ڴ��ͷź�����
		����������
		���������
		����ʱ��:2016��9��13�� 15:22
		������:�ŷ�
		��ע��
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
		�������ƣ�WriteToFile
		�������ܣ����������������ڴ��ͷź�����
		����������
		���������
		����ʱ��:2016��9��13�� 15:22
		������:�ŷ�
		��ע��
*************************************************************************/
void AdaBoostClassifier::WriteToFile(ofstream& f) const
{
	//һ�����������ļ���������д��
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
		�������ƣ�ReadFromFile
		�������ܣ���ȡÿ��ǿ��������������������
		����������
		���������
		����ʱ��:2016��9��13�� 10��58
		������:�ŷ�
		��ע��
*************************************************************************/
void AdaBoostClassifier::ReadFromFile(ifstream& f)
{
	Clear();
	//��������
	f>>count;
	scs = new SimpleClassifier[count]; 
	alphas = new double[count]; 
	//��ֵ
	f>>thresh;
	//alphasֵ
	for(int i=0;i<count;i++) 
		f>>alphas[i]; 
	f.ignore(256,'\n');
	for(int i=0;i<count;i++) 
		scs[i].ReadFromFile(f); 
	f.ignore(256,'\n');
}