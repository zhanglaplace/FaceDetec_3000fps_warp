
#include <fstream>
#include <vector>
#include <math.h>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"

/*************************************************************************
		�������ƣ�ReadFromFile
		�������ܣ���ȡÿ��������������
		����������
		���������
		����ʱ��:2016��9��13�� 11��04
		������:�ŷ�
		��ע��
*************************************************************************/
void SimpleClassifier::ReadFromFile(ifstream& f)
{
	//��ֵ,����,haar�������ͣ���Ӧ�������ꡣ
	f>>thresh>>parity>>type;
	f>>x1>>x2>>x3>>x4>>y1>>y2>>y3>>y4;
}


/*************************************************************************
		�������ƣ�WriteToFile
		�������ܣ�д��ÿ�����������������ļ���
		����������
		���������
		����ʱ��:2016��9��13�� 11��08
		������:�ŷ�
		��ע��
*************************************************************************/
void SimpleClassifier::WriteToFile(ofstream& f) const
{
	f<<thresh<<" ";
	f<<parity<<" ";
	f<<type<<" ";
	f<<x1<<" ";
	f<<x2<<" ";
	f<<x3<<" ";
	f<<x4<<" ";
	f<<y1<<" ";
	f<<y2<<" ";
	f<<y3<<" ";
	f<<y4<<" ";
	f<<endl;
}
