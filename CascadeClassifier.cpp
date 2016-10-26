#include <iostream>
#include <time.h>
#include <fstream>
#include <vector>
#include <math.h>
#include <algorithm>
using namespace std;
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"

NCascadeClassifier::NCascadeClassifier():count(0),ac(NULL)
{
}

NCascadeClassifier::~NCascadeClassifier()
{
	Clear();	
}



/*************************************************************************
		函数名称：Clear()
		函数功能：级联分类器的释放
		函数参数：
		函数输出：
		创建时间:2016年9月13日 15:37
		创建人:张峰
		备注：
*************************************************************************/
void NCascadeClassifier::Clear()
{
	count = 0;
	delete[] ac; 
	ac = NULL;
}

/*************************************************************************
		函数名称：SizeOfRect()
		函数功能：矩形框的面积
		函数参数：const ZRect& rect 矩形
		函数输出：int 面积
		创建时间:2016年9月13日 15:40
		创建人:张峰
		备注：
*************************************************************************/
inline int SizeOfRect(ZRect& rect)
{
	return rect.Height()*rect.Width();
}

/*************************************************************************
		函数名称：operator=
		函数功能：重载符号=
		函数参数：const NCascadeClassifier& source 原分类器
		函数输出：NCascadeClassifier& 新分类器
		创建时间:2016年9月13日 15:40
		创建人:张峰
		备注：
*************************************************************************/
NCascadeClassifier& NCascadeClassifier::operator=(const NCascadeClassifier& source)
{
	Clear();
	count = source.count;
	ac = new AdaBoostClassifier[max_nodes];
	for(int i=0;i<count;i++)
		ac[i] = source.ac[i];
	return *this;
}

/*************************************************************************
		函数名称：ReadFromFile
		函数功能：读取级联分类器文件内部参数
		函数参数：
		函数输出：
		创建时间:2016年9月13日 10：58
		创建人:张峰
		备注：
*************************************************************************/
void NCascadeClassifier::ReadFromFile(ifstream& f)
{
	Clear();
	//分类器个数
	f>>count; 
	ac = new AdaBoostClassifier[max_nodes];
	for(int i=0;i<count;i++) 
		ac[i].ReadFromFile(f);
}

/*************************************************************************
		函数名称：WriteToFile
		函数功能：将级联分类器写入到文件中
		函数参数：ofstream& f 输出文件流
		函数输出：
		创建时间:2016年9月13日 15:40
		创建人:张峰
		备注：
*************************************************************************/

void NCascadeClassifier::WriteToFile(ofstream& f) const
{
	f<<count<<endl;
	for(int i=0;i<count;i++) 
		ac[i].WriteToFile(f);
}

/*************************************************************************
		函数名称：LoadDefaultCascade
		函数功能：读取级联分类器参数
		函数参数：
		函数输出：
		创建时间:2016年9月13日 10：56
		创建人:张峰
		备注：
*************************************************************************/
void NCascadeClassifier::LoadDefaultCascade()
{
	ifstream f;
	f.open(cascade_filename);
	ReadFromFile(f);
	f.close();
}

/*************************************************************************
		函数名称：PostProcess
		函数功能：人脸矩形的合并
		函数参数：vector<ZRect>& result,人脸容器
				  const int combine_min 矩形结合最小次数
		函数输出：
		创建时间:2016年9月13日 19:49
		创建人:张峰
		备注：
*************************************************************************/
void PostProcess(vector<ZRect>& result,const int combine_min)
{
	vector<ZRect> res1;
	vector<ZRect> resmax;
	vector<int> res2;
	bool yet;
	ZRect rectInter;
	
	for(unsigned int i=0,size_i=result.size();i<size_i;i++)
	{//循环遍历所有人脸小矩形
		yet = false;
		ZRect& result_i = result[i];//记录当前矩形
		for(unsigned int j=0,size_r=res1.size();j<size_r;j++)
		{//循环遍历不想交的
			ZRect& resmax_j = resmax[j];
			rectInter.IntersectRect(result_i,resmax_j);//IntersectRect返回两个矩形相交的矩形
			if(SizeOfRect(rectInter)>0.6*SizeOfRect(result_i) && SizeOfRect(rectInter)>0.6*SizeOfRect(resmax_j))
			{//如果相交面积大于0.6倍的矩形
				ZRect& res1_j = res1[j];
				resmax_j.UnionRect(resmax_j,result_i);//两个矩形的最小外接矩形
				res1_j.bottom += result_i.bottom;
				res1_j.top += result_i.top;
				res1_j.left += result_i.left;
				res1_j.right += result_i.right;
				res2[j]++;
				yet = true;
				break;
			}
		}
		if(yet==false)
		{//如果相交，则添加到候选序列矩形
			res1.push_back(result_i);
			resmax.push_back(result_i);
			res2.push_back(1);
		}
	}

	for(unsigned int i=0,size=res1.size();i<size;i++)
	{//根据级联的矩形个数，求出平均的矩形位置
		const int count = res2[i];
		ZRect& res1_i = res1[i];
		res1_i.top /= count;
		res1_i.bottom /= count;
		res1_i.left /= count;
		res1_i.right /= count;
	}

	result.clear();
	//判断矩形的结合个数是否满足条件
	for(unsigned int i=0,size = res1.size();i<size;i++) 
		if(res2[i]>combine_min)
			result.push_back(res1[i]);
}


/*************************************************************************
		函数名称：Select
		函数功能：不合标准矩形的挑选
		函数参数：CRect& Ra 人脸1
				  CRect& b 人脸2
				  vector<int>&index 不符合要求人脸容器
				  int id 人脸1对应序号
				  int jd 人脸2对应序号
		函数输出：
		创建时间:2016年9月14日 11:12
		创建人:张峰
		备注：
*************************************************************************/
void Select(ZRect& Ra,ZRect& Rb,vector<int>&index,int id,int jd)
{
	int size = 0;
	//判断两个矩形人脸的位置，
	int left = max(Ra.left,Rb.left);
	int top = max(Ra.top,Rb.top);
	int right= min(Ra.right,Rb.right);
	int bottom = min(Ra.bottom,Rb.bottom);
	if((left>right)||(top>bottom))//如果不想交
		return ;
	else
	{
		int Sizea = Ra.Width()*Ra.Height();
		int Sizeb = Rb.Width()*Rb.Height();
		if(Sizea > 2*Sizeb)
			index.push_back(jd);//a包含b，丢弃b
		else if(Sizeb> 2*Sizea)
			index.push_back(id);//b包含a，丢弃a
		else 
			return ;
	}
}

/*************************************************************************
		函数名称：Select
		函数功能：不合标准矩形的挑选
		函数参数：cv::Rect& Ra 人脸1
				  cv::Rect& b 人脸2
				  vector<int>&index 不符合要求人脸容器
				  int id 人脸1对应序号
				  int jd 人脸2对应序号
		函数输出：
		创建时间:2016年9月14日 11:12
		创建人:张峰
		备注：
*************************************************************************/
void Select(cv::Rect& Ra,cv::Rect& Rb,vector<int>&index,int id,int jd)
{
	int size = 0;
	//判断两个矩形人脸的位置，
	int left = max(Ra.x,Rb.x);
	int top = max(Ra.y,Rb.y);
	int right= min(Ra.x+Ra.width,Rb.x+Rb.width);
	int bottom = min(Ra.y+Ra.height,Rb.y+Rb.height);
	if((left>right)||(top>bottom))//如果不想交
		return ;
	else
	{
		int Sizea = Ra.width*Ra.height;
		int Sizeb = Rb.width*Rb.height;
		if(Sizea > Sizeb)
			index.push_back(jd);//a包含b，丢弃b
		else
			index.push_back(id);//b包含a，丢弃a
	}
}

/*************************************************************************
		函数名称：ApplyOriginalSize
		函数功能：人脸检测
		函数参数：IntImage& original,原始图像
				  const string filename 保存文件路径
		函数输出：
		创建时间:2016年9月13日 15:42
		创建人:张峰
		备注：
*************************************************************************/
void NCascadeClassifier::ApplyOriginalSize(IntImage& original,vector<cv::Rect>&box) 
{
	//检测人脸时间记录
	//clock_t start,finish;
	//double times;
	//start=clock();


	IntImage procface;
	IntImage image,square;//积分图和平方积分图
	double sq,ex,value;//方差，平均值，
	int result;
	ZRect rect;
	double ratio;
	vector<ZRect> results;

	procface.Copy(original);
	ratio = 1.0;
	results.clear();
	double paddedsize = double(1)/double((sx+1)*(sy+1));//1/图像大小
	while((procface.height>sx+1) && (procface.width>sy+1))
	{//图像的窗口大于窗口大小 循环
		procface.CalcSquareAndIntegral(square,image);//计算积分图和平方积分图
		for(int i=0,size_x=image.height-sx;i<size_x;i+=1)
		{// 从0--图像大小减去窗口高
			for(int j=0,size_y=image.width-sy;j<size_y;j+=1)
			{//从 0 ---图像大小减去窗口宽
				//获取积分图
				ex = image.data[i+sx][j+sy]+image.data[i][j]-image.data[i+sx][j]-image.data[i][j+sy];
				if(ex<mean_min || ex>mean_max) 
					continue;
				sq = square.data[i+sx][j+sy]+square.data[i][j]-square.data[i+sx][j]-square.data[i][j+sy];
				if(sq<sq_min)
					continue;
				ex *= paddedsize;
				ex = ex * ex;
				sq *= paddedsize;
				sq = sq - ex;
				if(sq>0) 
					sq = sqrt(sq);
				else 
					sq = 1.0;
				if(sq<var_min) 
					continue;
				result = 1;

				//开始进行级联强分类器判断，
				for(int k=0;k<count;k++)
				{// 0--分类器个数
					value = 0.0;
					for(int t=0,size=ac[k].count;t<size;t++)
					{//每一层强分类器的弱分类器个数
						double f1 = 0;
						double** p = image.data + i;

						//每一个强分类器的每一个弱分类器
						SimpleClassifier& s = ac[k].scs[t];

						//求特征
						switch(s.type)
						{
							case 0:
								f1 =   p[s.x1][j+s.y3] - p[s.x1][j+s.y1] + p[s.x3][j+s.y3] - p[s.x3][j+s.y1]
									+ double(2)*(p[s.x2][j+s.y1] - p[s.x2][j+s.y3]);
								break;
							case 1:
								f1 =   p[s.x3][j+s.y1] + p[s.x3][j+s.y3] - p[s.x1][j+s.y1] - p[s.x1][j+s.y3]
									+ double(2)*(p[s.x1][j+s.y2] - p[s.x3][j+s.y2]);
								break;
							case 2:
								f1 =   p[s.x1][j+s.y1] - p[s.x1][j+s.y3] + p[s.x4][j+s.y3] - p[s.x4][j+s.y1]
									+ double(3)*(p[s.x2][j+s.y3] - p[s.x2][j+s.y1] + p[s.x3][j+s.y1]  - p[s.x3][j+s.y3]);
								break;
							case 3:
								f1 =   p[s.x1][j+s.y1] - p[s.x1][j+s.y4] + p[s.x3][j+s.y4] - p[s.x3][j+s.y1]
									+ double(3)*(p[s.x3][j+s.y2] - p[s.x3][j+s.y3] + p[s.x1][j+s.y3] - p[s.x1][j+s.y2]);
								break;
							case 4:
								f1 =   p[s.x1][j+s.y1] + p[s.x1][j+s.y3] + p[s.x3][j+s.y1] + p[s.x3][j+s.y3]
									- double(2)*(p[s.x2][j+s.y1] + p[s.x2][j+s.y3] + p[s.x1][j+s.y2] + p[s.x3][j+s.y2])
									+ double(4)*p[s.x2][j+s.y2];
								break;
							default:
#ifndef DEBUG 
								__assume(0);
#else
								;
#endif
						}
						//进行判断，如果是人脸value值加上alpha
						if(s.parity!=0)
							if(f1<sq*s.thresh)
								value += ac[k].alphas[t];
							else ;
						else
							if(f1>=sq*s.thresh)
								value += ac[k].alphas[t];
							else ;
					}
					//一个强分类器的判断结果小于阈值，则不是人脸
					if(value<ac[k].thresh) 
					{
						result = 0;
						break;
					}
				}
				if(result!=0) 
				{//所有分类器均输出人脸。记录当前矩形区域
					const double r = 1.0/ratio;
					rect.left = (long)(j*r);rect.top = (long)(i*r);
					rect.right = (long)((j+sy)*r);rect.bottom = (long)((i+sx)*r);
					results.push_back(rect);
				}
			}
		}
		ratio = ratio * double(0.8);
		procface.Resize(image,double(0.8));
		SwapIntImage(procface,image);
	}

	total_fp += results.size();

	PostProcess(results,2);
	PostProcess(results,0);

	//finish = clock();
	//times = finish-start;
	//cout<<times<<endl;

	//根据位置关系和面积关系判断剔除掉多余的人脸部分
	int nCount = results.size();
	std::vector<int>index ;
	Rect tempbox;
	int width  =original.width;
	int height = original.height;
	vector<Rect>temp;
	for (int i = 0; i < nCount; i++)
	{
		tempbox.x = results[i].left;
		tempbox.y=results[i].top;
		tempbox.width=(results[i].right-results[i].left);
		tempbox.height=(results[i].bottom-results[i].top);
		if (tempbox.width>0.15*width&&tempbox.height>0.15*height)
		{
			temp.push_back(tempbox);
		}
	}

	nCount = temp.size();
	if (nCount > 1)
	{
		for(int id = 0;id < nCount-1;id++)
		{//从第一张人脸开始遍历
			for(int jd = id+1;jd < nCount;jd++)
			{//从i+1开始遍历
				Select(temp[id],temp[jd],index,id,jd);
			}
		}
	}
	sort(index.begin(),index.end());//排序
	vector<int>::iterator iter_end;
	iter_end = unique(index.begin(), index.end());//唯一值
	index.erase(iter_end, index.end());//删除相邻重复的
 	int k = 0;
	for(int t = 0;t < index.size();t++)
	{
		vector<Rect>::iterator iter = temp.begin()+(index[t]-k);
		temp.erase(iter);
		k++;
	}
	for (int i = 0; i < temp.size(); i++)
	{
		tempbox.x=temp[i].x;
		tempbox.y=temp[i].y;
		tempbox.width= temp[i].width;
		tempbox.height= temp[i].height;
		box.push_back(tempbox);
	}
}



/*************************************************************************
		函数名称：DrawResults
		函数功能：人脸检测
		函数参数：IntImage& image,
				  const vector<ZRect>& results
		函数输出：
		创建时间:2016年9月13日 15:42
		创建人:张峰
		备注：
*************************************************************************/
void NCascadeClassifier::DrawResults(IntImage& image,const vector<ZRect>& results) const
{
	int i;
	unsigned int k;
	int x1,x2,y1,y2;

	for(k=0;k<results.size();k++)
	{
		y1 = (results[k].top>=0)?results[k].top:0; 
		y1 = (results[k].top<image.height)?results[k].top:(image.height-1);
		y2 = (results[k].bottom>=0)?results[k].bottom:0;
		y2 = (results[k].bottom<image.height)?results[k].bottom:(image.height-1);
		x1 = (results[k].left>=0)?results[k].left:0;
		x1 = (results[k].left<image.width)?results[k].left:(image.width-1);
		x2 = (results[k].right>=0)?results[k].right:0;
		x2 = (results[k].right<image.width)?results[k].right:(image.width-1);	
		for(i=y1;i<=y2;i++) 
		{
			image.data[i][x1] = 255;
			image.data[i][x2] = 255;
		}
		for(i=x1;i<=x2;i++)
		{
			image.data[y1][i] = 255;
			image.data[y2][i] = 255;
		}
	}
}

