#include <algorithm>
#include <stdio.h>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace cv;
#include "IntImage.h"
IntImage::IntImage():height(0),width(0),data(NULL),buf(NULL),variance(0.0),label(-1)
{
}

IntImage::~IntImage()
{
	Clear();
}

void IntImage::Clear(void)
{
	if (data!=NULL)
	{
		if (buf != NULL)
		{
			for(int i=0;i<height;i++)	
				data[i] = NULL;
			delete[] data;	
			data = NULL;
			delete[] buf;
			buf = NULL;
			height = width = 0;
			variance = 0.0;
			label = -1;
		}
	}
}
// We pad a zero column and a zero row, so 24*24 image will be 25*25 in size
void IntImage::CalculateVarianceAndIntegralImageInPlace()
{
	double partialsum;
	double ex,ex2;
	double size,var;
	ex=0;
	ex2=0;
	for (int i = 0,size=height*width; i <size ; i++)
	{
		ex+=buf[i];
		ex2+=(buf[i]*buf[i]);
	}
	size=double(height*width);
	var=ex2/size-(ex/size)*(ex/size);
	if (var<0)
		return ;
	if (var>0)
		variance=sqrt(var);
	else
		variance=1.0;

		for(int i=1;i<width;i++)
	{
		data[0][i] = data[0][i-1] + data[0][i];
	}
	for (int i = 1; i < height; i++)
	{
		partialsum=0;
		for (int j = 0; j < width; j++)
		{
			partialsum+=data[i][j];
			data[i][j]=data[i-1][j]+partialsum;
		}
	}
}

void IntImage::CalcSquareAndIntegral(IntImage& square,IntImage& image) const
{
	double partialsum,partialsum2;

	square.SetSize(ZSize(height+1,width+1));
	image.SetSize(ZSize(height+1,width+1));

	for(int i=0;i<=width+1;i++) 
		square.buf[i]=image.buf[i]=0;
	for(int i=1;i<=height;i++)
	{
		partialsum = partialsum2 = 0;
		square.data[i][0] = 0;
		image.data[i][0] = 0;
		for(int j=1;j<=width;j++)
		{
			partialsum += (data[i-1][j-1]*data[i-1][j-1]);
			partialsum2 += data[i-1][j-1];
			square.data[i][j] = square.data[i-1][j] + partialsum;
			image.data[i][j] = image.data[i-1][j] + partialsum2;
		}
	}
}

void IntImage::SetSize(const ZSize size)
{
	if((size.cx==height)&&(size.cy==width)&&(buf!=NULL)&&(data!=NULL))
		return;
	Clear();
	height = size.cx;
	width = size.cy;
	buf = new double[height*width];
	if (buf==NULL)
		return;
	data=new double*[height];
	if (data==NULL)
		return;
	for(int i = 0;i < height;i++)	
		data[i] = &buf[i*width];
}

void IntImage::Copy(const IntImage& source)
{
	SetSize(ZSize(source.height,source.width));
	memcpy(buf,source.buf,sizeof(double)*height*width);
}

void IntImage::Load(string filename)
{
	IplImage* img;
	img = cvLoadImage(filename.c_str(),0);
	SetSize(ZSize(img->height,img->width));
	for(int i=0,ih=img->height,iw=img->width;i<ih;i++)
	{
		double* pdata=data[i];
		unsigned char* pimg = reinterpret_cast<unsigned char*>(img->imageData+img->widthStep*i);
		for(int j=0;j<iw;j++) 
			pdata[j] = pimg[j];
	}
	cvReleaseImage(&img);
}

void IntImage::Load(Mat& src)
{
	Mat temp  = src.clone();
	IplImage img = temp;
	SetSize(ZSize(img.height,img.width));
	for(int i=0,ih=img.height,iw=img.width;i<ih;i++)
	{
		double* pdata = data[i];
		unsigned char* pimg = reinterpret_cast<unsigned char*>(img.imageData+img.widthStep*i);
		for(int j=0;j<iw;j++) 
			pdata[j] = pimg[j];
	}
	//cvReleaseImage(&img);
}

void IntImage::Resize(IntImage &result, double ratio) const
{
	result.SetSize(ZSize(int(height*ratio),int(width*ratio)));
	ratio = 1/ratio;
	for(int i=0,rh=result.height,rw=result.width;i<rh;i++)
	{
		for(int j=0;j<rw;j++) 
		{
			int x0,y0;
			double x,y,fx0,fx1;
			x = j*ratio; 
			y = i*ratio;
			x0 = (int)(x);
			y0 = (int)(y);
			if (x0 == width-1) 
				x0--;
			if (y0 == height-1) 
				y0--;
			x = x - x0; 
			y = y - y0;

			fx0 = data[y0][x0] + x*(data[y0][x0+1]-data[y0][x0]);
			fx1 = data[y0+1][x0] + x*(data[y0+1][x0+1]-data[y0+1][x0]);
			result.data[i][j] = fx0 + y*(fx1-fx0);
		}
	}
}

void SwapIntImage(IntImage& i1,IntImage& i2)
{
	std::swap(i1.height,i2.height);
	std::swap(i1.width,i2.width);
	std::swap(i1.buf,i2.buf);
	std::swap(i1.data,i2.data);
	std::swap(i1.variance,i2.variance);
	std::swap(i1.label,i2.label);
}