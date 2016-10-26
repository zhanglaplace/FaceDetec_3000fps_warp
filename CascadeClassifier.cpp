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
		�������ƣ�Clear()
		�������ܣ��������������ͷ�
		����������
		���������
		����ʱ��:2016��9��13�� 15:37
		������:�ŷ�
		��ע��
*************************************************************************/
void NCascadeClassifier::Clear()
{
	count = 0;
	delete[] ac; 
	ac = NULL;
}

/*************************************************************************
		�������ƣ�SizeOfRect()
		�������ܣ����ο�����
		����������const ZRect& rect ����
		���������int ���
		����ʱ��:2016��9��13�� 15:40
		������:�ŷ�
		��ע��
*************************************************************************/
inline int SizeOfRect(ZRect& rect)
{
	return rect.Height()*rect.Width();
}

/*************************************************************************
		�������ƣ�operator=
		�������ܣ����ط���=
		����������const NCascadeClassifier& source ԭ������
		���������NCascadeClassifier& �·�����
		����ʱ��:2016��9��13�� 15:40
		������:�ŷ�
		��ע��
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
		�������ƣ�ReadFromFile
		�������ܣ���ȡ�����������ļ��ڲ�����
		����������
		���������
		����ʱ��:2016��9��13�� 10��58
		������:�ŷ�
		��ע��
*************************************************************************/
void NCascadeClassifier::ReadFromFile(ifstream& f)
{
	Clear();
	//����������
	f>>count; 
	ac = new AdaBoostClassifier[max_nodes];
	for(int i=0;i<count;i++) 
		ac[i].ReadFromFile(f);
}

/*************************************************************************
		�������ƣ�WriteToFile
		�������ܣ�������������д�뵽�ļ���
		����������ofstream& f ����ļ���
		���������
		����ʱ��:2016��9��13�� 15:40
		������:�ŷ�
		��ע��
*************************************************************************/

void NCascadeClassifier::WriteToFile(ofstream& f) const
{
	f<<count<<endl;
	for(int i=0;i<count;i++) 
		ac[i].WriteToFile(f);
}

/*************************************************************************
		�������ƣ�LoadDefaultCascade
		�������ܣ���ȡ��������������
		����������
		���������
		����ʱ��:2016��9��13�� 10��56
		������:�ŷ�
		��ע��
*************************************************************************/
void NCascadeClassifier::LoadDefaultCascade()
{
	ifstream f;
	f.open(cascade_filename);
	ReadFromFile(f);
	f.close();
}

/*************************************************************************
		�������ƣ�PostProcess
		�������ܣ��������εĺϲ�
		����������vector<ZRect>& result,��������
				  const int combine_min ���ν����С����
		���������
		����ʱ��:2016��9��13�� 19:49
		������:�ŷ�
		��ע��
*************************************************************************/
void PostProcess(vector<ZRect>& result,const int combine_min)
{
	vector<ZRect> res1;
	vector<ZRect> resmax;
	vector<int> res2;
	bool yet;
	ZRect rectInter;
	
	for(unsigned int i=0,size_i=result.size();i<size_i;i++)
	{//ѭ��������������С����
		yet = false;
		ZRect& result_i = result[i];//��¼��ǰ����
		for(unsigned int j=0,size_r=res1.size();j<size_r;j++)
		{//ѭ���������뽻��
			ZRect& resmax_j = resmax[j];
			rectInter.IntersectRect(result_i,resmax_j);//IntersectRect�������������ཻ�ľ���
			if(SizeOfRect(rectInter)>0.6*SizeOfRect(result_i) && SizeOfRect(rectInter)>0.6*SizeOfRect(resmax_j))
			{//����ཻ�������0.6���ľ���
				ZRect& res1_j = res1[j];
				resmax_j.UnionRect(resmax_j,result_i);//�������ε���С��Ӿ���
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
		{//����ཻ������ӵ���ѡ���о���
			res1.push_back(result_i);
			resmax.push_back(result_i);
			res2.push_back(1);
		}
	}

	for(unsigned int i=0,size=res1.size();i<size;i++)
	{//���ݼ����ľ��θ��������ƽ���ľ���λ��
		const int count = res2[i];
		ZRect& res1_i = res1[i];
		res1_i.top /= count;
		res1_i.bottom /= count;
		res1_i.left /= count;
		res1_i.right /= count;
	}

	result.clear();
	//�жϾ��εĽ�ϸ����Ƿ���������
	for(unsigned int i=0,size = res1.size();i<size;i++) 
		if(res2[i]>combine_min)
			result.push_back(res1[i]);
}


/*************************************************************************
		�������ƣ�Select
		�������ܣ����ϱ�׼���ε���ѡ
		����������CRect& Ra ����1
				  CRect& b ����2
				  vector<int>&index ������Ҫ����������
				  int id ����1��Ӧ���
				  int jd ����2��Ӧ���
		���������
		����ʱ��:2016��9��14�� 11:12
		������:�ŷ�
		��ע��
*************************************************************************/
void Select(ZRect& Ra,ZRect& Rb,vector<int>&index,int id,int jd)
{
	int size = 0;
	//�ж���������������λ�ã�
	int left = max(Ra.left,Rb.left);
	int top = max(Ra.top,Rb.top);
	int right= min(Ra.right,Rb.right);
	int bottom = min(Ra.bottom,Rb.bottom);
	if((left>right)||(top>bottom))//������뽻
		return ;
	else
	{
		int Sizea = Ra.Width()*Ra.Height();
		int Sizeb = Rb.Width()*Rb.Height();
		if(Sizea > 2*Sizeb)
			index.push_back(jd);//a����b������b
		else if(Sizeb> 2*Sizea)
			index.push_back(id);//b����a������a
		else 
			return ;
	}
}

/*************************************************************************
		�������ƣ�Select
		�������ܣ����ϱ�׼���ε���ѡ
		����������cv::Rect& Ra ����1
				  cv::Rect& b ����2
				  vector<int>&index ������Ҫ����������
				  int id ����1��Ӧ���
				  int jd ����2��Ӧ���
		���������
		����ʱ��:2016��9��14�� 11:12
		������:�ŷ�
		��ע��
*************************************************************************/
void Select(cv::Rect& Ra,cv::Rect& Rb,vector<int>&index,int id,int jd)
{
	int size = 0;
	//�ж���������������λ�ã�
	int left = max(Ra.x,Rb.x);
	int top = max(Ra.y,Rb.y);
	int right= min(Ra.x+Ra.width,Rb.x+Rb.width);
	int bottom = min(Ra.y+Ra.height,Rb.y+Rb.height);
	if((left>right)||(top>bottom))//������뽻
		return ;
	else
	{
		int Sizea = Ra.width*Ra.height;
		int Sizeb = Rb.width*Rb.height;
		if(Sizea > Sizeb)
			index.push_back(jd);//a����b������b
		else
			index.push_back(id);//b����a������a
	}
}

/*************************************************************************
		�������ƣ�ApplyOriginalSize
		�������ܣ��������
		����������IntImage& original,ԭʼͼ��
				  const string filename �����ļ�·��
		���������
		����ʱ��:2016��9��13�� 15:42
		������:�ŷ�
		��ע��
*************************************************************************/
void NCascadeClassifier::ApplyOriginalSize(IntImage& original,vector<cv::Rect>&box) 
{
	//�������ʱ���¼
	//clock_t start,finish;
	//double times;
	//start=clock();


	IntImage procface;
	IntImage image,square;//����ͼ��ƽ������ͼ
	double sq,ex,value;//���ƽ��ֵ��
	int result;
	ZRect rect;
	double ratio;
	vector<ZRect> results;

	procface.Copy(original);
	ratio = 1.0;
	results.clear();
	double paddedsize = double(1)/double((sx+1)*(sy+1));//1/ͼ���С
	while((procface.height>sx+1) && (procface.width>sy+1))
	{//ͼ��Ĵ��ڴ��ڴ��ڴ�С ѭ��
		procface.CalcSquareAndIntegral(square,image);//�������ͼ��ƽ������ͼ
		for(int i=0,size_x=image.height-sx;i<size_x;i+=1)
		{// ��0--ͼ���С��ȥ���ڸ�
			for(int j=0,size_y=image.width-sy;j<size_y;j+=1)
			{//�� 0 ---ͼ���С��ȥ���ڿ�
				//��ȡ����ͼ
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

				//��ʼ���м���ǿ�������жϣ�
				for(int k=0;k<count;k++)
				{// 0--����������
					value = 0.0;
					for(int t=0,size=ac[k].count;t<size;t++)
					{//ÿһ��ǿ��������������������
						double f1 = 0;
						double** p = image.data + i;

						//ÿһ��ǿ��������ÿһ����������
						SimpleClassifier& s = ac[k].scs[t];

						//������
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
						//�����жϣ����������valueֵ����alpha
						if(s.parity!=0)
							if(f1<sq*s.thresh)
								value += ac[k].alphas[t];
							else ;
						else
							if(f1>=sq*s.thresh)
								value += ac[k].alphas[t];
							else ;
					}
					//һ��ǿ���������жϽ��С����ֵ����������
					if(value<ac[k].thresh) 
					{
						result = 0;
						break;
					}
				}
				if(result!=0) 
				{//���з������������������¼��ǰ��������
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

	//����λ�ù�ϵ�������ϵ�ж��޳����������������
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
		{//�ӵ�һ��������ʼ����
			for(int jd = id+1;jd < nCount;jd++)
			{//��i+1��ʼ����
				Select(temp[id],temp[jd],index,id,jd);
			}
		}
	}
	sort(index.begin(),index.end());//����
	vector<int>::iterator iter_end;
	iter_end = unique(index.begin(), index.end());//Ψһֵ
	index.erase(iter_end, index.end());//ɾ�������ظ���
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
		�������ƣ�DrawResults
		�������ܣ��������
		����������IntImage& image,
				  const vector<ZRect>& results
		���������
		����ʱ��:2016��9��13�� 15:42
		������:�ŷ�
		��ע��
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

