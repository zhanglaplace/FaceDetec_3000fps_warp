#include <iostream>
#include <fstream>
#include <vector>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/core.hpp>
using namespace cv;
using namespace std;
#include "headers.h"
#include "IntImage.h"
#include "SimpleClassifier.h"
#include "AdaBoostClassifier.h"
#include "CascadeClassifier.h"
#include "Global.h"
#include <omp.h>

vector<vector <string>>EmotionName;
const int numCategories = 7; 
const string train_file_path = "L:\\Backup\\picture\\windows\\CK+(expression)\\data\\train\\";//训练文件路径
int EmotionNum[7]={132,154,63,276,100,80,94};
const string TriName = "./data/face.tri";
const string  ConName= "./data/face.con";
CvScalar color = CV_RGB(0,255,0);
int  UniformPattern59[256]={    
			1,   2,   3,   4,   5,   0,   6,   7,   8,   0,   0,   0,   9,   0,  10,  11,
		12,   0,   0,   0,   0,   0,   0,   0,  13,   0,   0,   0,  14,   0,  15,  16,
		17,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		18,   0,   0,   0,   0,   0,   0,   0,  19,   0,   0,   0,  20,   0,  21,  22,
		23,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		24,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		25,   0,   0,   0,   0,   0,   0,   0,  26,   0,   0,   0,  27,   0,  28,  29,
		30,  31,   0,  32,   0,   0,   0,  33,   0,   0,   0,   0,   0,   0,   0,  34,
		0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  35,
		0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,
		0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  36,
		37,  38,   0,  39,   0,   0,   0,  40,   0,   0,   0,   0,   0,   0,   0,  41,
		0,    0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  42,
		43,  44,   0,  45,   0,   0,   0,  46,   0,   0,   0,   0,   0,   0,   0,  47,
		48,  49,   0,  50,   0,   0,   0,  51,  52,  53,   0,  54,  55,  56,  57,  58
};


void ReadString(ifstream& f,vector<string>&temp,int num,string pre)
{
	string s;
	string tmp;
	for (int i = 0; i < num; i++)
	{
		f>>s;
		tmp = pre+s;
		temp.push_back(tmp);
	}
}

void LoadTrainImage(string filename)
{
	ifstream f;
	ifstream sf;
	int i ;
	string temp;
	f.open(filename);
	for (int i = 0; i < 7; i++)
	{
		f>>temp;
		vector<string>EmotionTemp;;
		string predictory = train_file_path+temp+"\\";
		string EmotionTrainClassify = predictory+temp+".txt";
		sf.open(EmotionTrainClassify);
		ReadString(sf,EmotionTemp,EmotionNum[i],predictory);
		EmotionName.push_back(EmotionTemp);
		sf.close();
	}
	f.close();
}

void LoadTriPosition(Mat& Tri,string filename)
{
	fstream file(filename,fstream::in);
	if (!file.is_open())
	{
		cout<<"Failed to open file for reading\n";
		return ;
	}
	char str[256];
	char c;
	while(1)
	{
		file>>str;
		if (strncmp(str,"n_tri:",6) == 0)
			break;
	}
	int n ;
	file >> n;
	while(1)
	{
		file >> c; 
		if(c == '{')
			break;
	}
	for (int i = 0; i < n; i++)
	{
		file >> Tri.at<int>(i,0) >> Tri.at<int>(i,1) >> Tri.at<int>(i,2);
	}
	file.close();
}

void LoadCon(Mat&Con,string filename)
{
	int i,n;
	char str[256];
	char c; 
	fstream file(filename,fstream::in);
	if(!file.is_open())
	{
	cout<<"Failed to open file for reading\n";
		return ;
	}
	while(1)
	{
		file >> str; 
		if(strncmp(str,"n_connections:",14) == 0)
			break;
	}
	file >> n; 
	while(1)
	{
		file >> c; 
		if(c == '{')
			break;
	}
	for(i = 0; i < n; i++)
		file >> Con.at<int>(0,i) >> Con.at<int>(1,i);
	file.close();

}

void Draw(Mat image,Mat_<double>& shape,Mat Tri,Mat Con)
{
	int i,n = 68;
	cv::Point p1,p2;
	cv::Scalar c;

	//画出三角链接
	c = CV_RGB(0,0,0);
	for(i = 0; i < Tri.rows; i++)
	{
		p1 = cv::Point(shape.at<double>(Tri.at<int>(i,0),0),
				shape.at<double>(Tri.at<int>(i,0),1));
		p2 = cv::Point(shape.at<double>(Tri.at<int>(i,1),0),
				shape.at<double>(Tri.at<int>(i,1),1));
		cv::line(image,p1,p2,c);
		p1 = cv::Point(shape.at<double>(Tri.at<int>(i,0),0),
				shape.at<double>(Tri.at<int>(i,0),1));
		p2 = cv::Point(shape.at<double>(Tri.at<int>(i,2),0),
				shape.at<double>(Tri.at<int>(i,2),1));
		cv::line(image,p1,p2,c);
		p1 = cv::Point(shape.at<double>(Tri.at<int>(i,2),0),
				shape.at<double>(Tri.at<int>(i,2),1));
		p2 = cv::Point(shape.at<double>(Tri.at<int>(i,1),0),
				shape.at<double>(Tri.at<int>(i,1),1));
		cv::line(image,p1,p2,c);
	}
	//画出点链接区域
	c = CV_RGB(0,0,255);
	for(i = 0; i < Con.cols; i++)
	{
		p1 = cv::Point(shape.at<double>(Con.at<int>(0,i),0),
			shape.at<double>(Con.at<int>(0,i),1));
		p2 = cv::Point(shape.at<double>(Con.at<int>(1,i),0),
			shape.at<double>(Con.at<int>(1,i),1));
		cv::line(image,p1,p2,c,1);
	}

	CvFont font;
	cvInitFont(&font,CV_FONT_VECTOR0,1.5,0.8,0.0,1);
	//draw points
	c = CV_RGB(255,0,0);
	for(i = 0; i < n; i++)
	{
		p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i,1));
		cv::circle(image,p1,2,c);
	}
}

void facesDetect(Mat& img,cv::vector<Rect>&Crect)
{
	IntImage Intimg;
	Intimg.Load(img);
	cascade->ApplyOriginalSize(Intimg,Crect);
}

void getPartLBP(Mat& src,int roiHist[][59],vector<BoundingBox>&roi)
{
	for (int index = 0; index < roi.size(); index++)
	{
		int height = roi[index].height;
		int width = roi[index].width;
		uchar p[9];
		for (int i = roi[index].start_y+1; i < height-1; i++)
		{//循环遍历
			for (int j = roi[index].start_x+1; j < width-1; j++)
			{
				int code = 0;
				p[0] = src.at<uchar>(i,j);
				p[1] = src.at<uchar>(i-1,j-1);
				p[2] = src.at<uchar>(i-1,j);
				p[3] = src.at<uchar>(i-1,j+1);
				p[4] = src.at<uchar>(i,j+1);
				p[5] = src.at<uchar>(i+1,j+1);
				p[6] = src.at<uchar>(i+1,j);
				p[7] = src.at<uchar>(i+1,j-1);
				p[8] = src.at<uchar>(i,j-1);

				for (int k = 1; k < 8; k++)
				{
					code += (p[k] >= p[0]) << (8 - k);
				}
				roiHist[index][UniformPattern59[code]]++;
			}
		}
	}
}

Mat_<double> warpImage(Mat& src,Mat_<double>landmark,Mat& roi)
{
	Mat_<double>rotation_landmark(68,2,0.0);
	Mat temp;
	if (src.channels()==3)
	{
		cvtColor(src,temp,CV_BGR2GRAY);
	}
	else
		temp = src.clone();
	//人眼定位
	CvPoint leftEye = cvPoint(0,0);
	CvPoint rightEye = leftEye;
	CvPoint centerEye = leftEye;
	for (int i = 36; i < 42; i++)
	{
		leftEye.x+= landmark(i,0);
		leftEye.y+= landmark(i,1);
	}
	leftEye.x = cvRound(leftEye.x/6.0);
	leftEye.y =cvRound(leftEye.y/6.0);
	for (int i = 42; i <48; i++)
	{
		rightEye.x+=landmark(i,0);
		rightEye.y+=landmark(i,1);
	}
	rightEye.x = cvRound(rightEye.x/6.0);
	rightEye.y = cvRound(rightEye.y/6.0);
	
	centerEye.x = 0.5*(leftEye.x+rightEye.x);
	centerEye.y = 0.5*(leftEye.y+rightEye.y);

	//dy为正时头像左偏，顺时针旋转angle调整图像
	double dy = rightEye.y - leftEye.y;
	double dx = rightEye.x - leftEye.x;
	double len = sqrtf(dx*dx+dy*dy);
	double angle = atan2f(dy,dx)*180/CV_PI;

	//确定旋转后的图像大小
	Mat roi_mat = getRotationMatrix2D(centerEye,angle,1.0);

	Mat s = temp;
	warpAffine(s,roi,roi_mat,cvSize(temp.cols,temp.rows));

	//vector<Point2f> marks;
	//按照仿射变换矩阵，计算变换后各关键点在新图中所对应的位置坐标。
	for (int n = 0; n < landmark.rows; n++)
	{
		rotation_landmark(n,0) = roi_mat.ptr<double>(0)[0] *landmark(n,0) + roi_mat.ptr<double>(0)[1] * landmark(n,1) + roi_mat.ptr<double>(0)[2];
		rotation_landmark(n,1) = roi_mat.ptr<double>(1)[0] *landmark(n,0) + roi_mat.ptr<double>(1)[1] * landmark(n,1) + roi_mat.ptr<double>(1)[2];
	}
	return rotation_landmark;
}





void getFaceLandmark(const char* Cas_file_name)
{
	//读取特征连线点坐标
	Mat Tri(90,3,CV_32S);
	LoadTriPosition(Tri,TriName);

	//读取三角形连线
	Mat Con(2,63,CV_32S);
	LoadCon(Con,ConName);

	CvFont font;
	cvInitFont(&font,CV_FONT_HERSHEY_DUPLEX,1.0f,1.0f,0,1,CV_AA);

	string optionsfilename = train_file_path+"Directory.txt";//训练
	LoadTrainImage(optionsfilename);

	CascadeRegressor cas_load;
	cas_load.LoadCascadeRegressor(Cas_file_name);
	
	string image_file_name;
	for (int i = 0; i < EmotionName.size(); i++)
	{//循环类别
		for (int j = 0; j < EmotionName[i].size(); j++)
		{//循环单个类别自训练数据
			image_file_name = "warp_test.jpg";//EmotionName[i][j];//"7.jpg";
			Mat img,gray;
			img = imread(image_file_name);
			cvtColor(img,gray,CV_BGR2GRAY);
			Mat small_gray(gray.rows*0.5,gray.cols*0.5,gray.type());
			resize(gray, small_gray, small_gray.size(), 0, 0, INTER_AREA);
			vector<Rect>faces;
			facesDetect(small_gray,faces);
			Mat_<uchar>pridict_input_temp = gray.clone();
			for (int i = 0; i < faces.size(); i++)
			{
				BoundingBox bbox;
				bbox.start_x = 2*faces[i].x;
				bbox.start_y = 2*faces[i].y;
				bbox.width = 2*faces[i].width;
				bbox.height = 2*faces[i].height;
				bbox.center_x = bbox.start_x + bbox.width / 2.0;
				bbox.center_y = bbox.start_y + bbox.height / 2.0;
				cv::Mat_<double> current_shape = ReProjection(cas_load.params_.mean_shape_, bbox);
				Mat_<double>landmark = cas_load.Predict(pridict_input_temp,current_shape,bbox);
				Mat rotation_image;//旋转后的图像
				//rotation_landmark 为旋转后的68个特征点坐标
				Mat_<double>rotation_landmark = warpImage(img,landmark,rotation_image);
				

				
				rectangle(img,Point(bbox.start_x,bbox.start_y),Point(bbox.start_x+bbox.width,bbox.start_y+bbox.height),CV_RGB(255,0,0));
				Draw(img,landmark,Tri,Con);
				imshow("test",img);
				rectangle(rotation_image,Point(bbox.start_x,bbox.start_y),Point(bbox.start_x+bbox.width,bbox.start_y+bbox.height),CV_RGB(255,0,0));
				Draw(rotation_image,rotation_landmark,Tri,Con);
				imshow("test2",rotation_image);
				cvWaitKey(0);
			}
		}
	}
}
int main()
{
	InitGlobalData();
	string cas_file_path = "./helenModel";
	getFaceLandmark(cas_file_path.c_str());
	return 0;
}