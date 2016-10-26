

//得到某点的LBP特征，例如Pixel(i,j)的周围3*3领域，为10010101 --则pixel(i,j) = 149;则UniformPattern59[149]
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

double getDistance(double x1,double x2)
{
	return abs(x1-x2);
}

vector<Rect> splitBlock(Mat_<double>&landmark)
{//8个表情子区域

	double lefteye_center_y = 0.5*(landmark(39,1)+landmark(36,1));
	double leftbrow_center_y = 0.5*(landmark(18,1)+landmark(20,1));
	double nostril_y = landmark(33,1);
	double out_mouth_center_y = 0.5*(landmark(48,1)+landmark(54,1));
	double distance_d1 = getDistance(lefteye_center_y,leftbrow_center_y);//眉毛眼睛垂直距离
	double distance_d2 = getDistance(nostril_y,lefteye_center_y);//眼睛鼻孔距离
	double distance_d3 = getDistance(out_mouth_center_y,nostril_y);//鼻孔嘴巴距离
	double distance_left_brow = landmark(27,0)-landmark(17,0)-1;//左眉毛区域宽度
	double distance_right_brow = landmark(26,0)-landmark(27,0)-1;//右眉毛区域宽度
	double distance_nose_width = landmark(44,0)-4-landmark(38,0);//鼻子区域宽度
	double distance_nose_height = 0.5*landmark(33,1)+0.5*landmark(51,1)-0.5*(landmark(28,1)+landmark(29,1))+4;//鼻子高度
	double distance_mouth_width = landmark(45,0)-4-landmark(36,0);//嘴巴区域宽度
	double distance_mouth_height = 0.5*landmark(57,1)+0.5*landmark(8,1)-0.5*(landmark(33,1)+landmark(51,1));//嘴巴高度


	Point2d p[8];
	int dis[8][2];
	//左眉顶点
	p[0].x = landmark(17,0)-2;
	p[0].y = landmark(19,1)-1;
	dis[0][0]= distance_left_brow;
	dis[0][1]= 0.8*distance_d1;

	//右眉顶点
	p[1].x = landmark(27,0)+2;
	p[1].y = p[0].y;
	dis[1][0]= distance_right_brow;
	dis[1][1]= 0.8*distance_d1;
	//左眼顶点
	p[2].x = p[0].x;
	p[2].y = 0.5*(lefteye_center_y+leftbrow_center_y);
	dis[2][0]= distance_left_brow;
	dis[2][1]= 0.5*(distance_d1+distance_d2);
	//右眼顶点
	p[3].x = p[1].x;
	p[3].y = p[2].y;
	dis[3][0]= distance_right_brow;
	dis[3][1]= 0.5*(distance_d1+distance_d2);
	//鼻子距离
	p[4].x = 0.5*(landmark(36,0)+landmark(39,0));
	p[4].y = 0.5*(landmark(28,1)+landmark(29,1));
	dis[4][0]= distance_nose_width;
	dis[4][1]= distance_nose_height;
	
	//嘴角区域
	p[5].x = landmark(36,0)+2;
	p[5].y = 0.5*(landmark(33,1)+landmark(51,1));
	dis[5][0]= distance_mouth_width;
	dis[5][1]= distance_mouth_height;
	//左脸
	p[6].x = p[0].x;
	p[6].y = p[4].y;
	dis[6][0]= distance_left_brow;
	dis[6][1]= distance_nose_height;
	//右脸
	p[7].x =p[1].x ;
	p[7].y = p[4].y;
	dis[7][0]= distance_right_brow;
	dis[7][1]= distance_nose_height;
	vector<Rect>block;
	for (int i = 0; i < 8; i++)
	{
		Rect temp(p[i].x,p[i].y,dis[i][0],dis[i][1]);
		block.push_back(temp);
	}
	return block;
} 



int RoiHist[8][59]={0};
//p1 p2 p3
//p8 p0 p4
//p7 p6 p5
void GetLBPHist(Mat_<uchar>& img,int RoiHist[][59],vector<ZRect>&roi)
{
	//获取行和获取列
	for (int index = 0; index < roi.size(); index++)
	{
		int height = roi[index].height;
		int width = roi[index].width;
		uchar p[9];
		for (int i = roi[index].y+1; i < height-1; i++)
		{//循环遍历
			for (int j = roi[index].x+1; j < width-1; j++)
			{
				int code = 0;
				p[0] = img.at<uchar>(i,j);
				p[1] = img.at<uchar>(i-1,j-1);
				p[2] = img.at<uchar>(i-1,j);
				p[3] = img.at<uchar>(i-1,j+1);
				p[4] = img.at<uchar>(i,j+1);
				p[5] = img.at<uchar>(i+1,j+1);
				p[6] = img.at<uchar>(i+1,j);
				p[7] = img.at<uchar>(i+1,j-1);
				p[8] = img.at<uchar>(i,j-1);

				for (int k = 1; k < 8; k++)
				{
					code += (p[k] >= p[0]) << (8 - k);
				}
				RoiHist[index][UniformPattern59[code]]++;
			}
		}
	}
}


//读取连接三角都Mat文件，Mat 大小为(90,3,CV_32S);
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

//读取点的连接线到Mat文件中，Mat大小为(2,63,CV_32S);
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

//画图函数， 包含画连线三角，画连接线，画点
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
	//画点
	c = CV_RGB(255,0,0);
	for(i = 0; i < n; i++)
	{
		p1 = cv::Point(shape.at<double>(i,0),shape.at<double>(i,1));
		cv::circle(image,p1,2,c);
	}
}

Mat_<double> Wrap(Mat& src,Mat_<double>landmark,Mat& roi)
{
	Mat_<double>rotation_landmark(68,2,0.0);
	Mat temp;
	cvtColor(src,temp,CV_BGR2GRAY);

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
