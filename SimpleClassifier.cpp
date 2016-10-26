
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
		函数名称：ReadFromFile
		函数功能：读取每个弱分类器参数
		函数参数：
		函数输出：
		创建时间:2016年9月13日 11：04
		创建人:张峰
		备注：
*************************************************************************/
void SimpleClassifier::ReadFromFile(ifstream& f)
{
	//阈值,方向,haar特征类型，对应特征坐标。
	f>>thresh>>parity>>type;
	f>>x1>>x2>>x3>>x4>>y1>>y2>>y3>>y4;
}


/*************************************************************************
		函数名称：WriteToFile
		函数功能：写入每个弱分类器参数到文件中
		函数参数：
		函数输出：
		创建时间:2016年9月13日 11：08
		创建人:张峰
		备注：
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
