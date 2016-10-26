#ifndef _INTIMAGE_H
#define _INTIMAGE_H
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
using namespace cv;
class ZSize
{
public:
	ZSize(int h,int w)
	{
		cx = h;
		cy = w;
	};
public:
	int cx;
	int cy;
};

class ZRect
{
public:
	ZRect()
	{
		top =0;
		left= 0;
		right= 0;
		bottom =0;
	};
	inline void IntersectRect(const ZRect rect1,const ZRect rect2)
	{
		top=max(rect1.top,rect2.top);
		left=max(rect1.left,rect2.left);
		bottom=min(rect1.bottom,rect2.bottom);
		right=min(rect1.right,rect2.right);
	}
	inline void UnionRect(const ZRect rect1,const ZRect rect2)
	{
		top=min(rect1.top,rect2.top);
		left=min(rect1.left,rect2.left);
		bottom=max(rect1.bottom,rect2.bottom);
		right=max(rect1.right,rect2.right);
	}
	
	int Height()
	{
		return bottom-top;
	}
	int Width()
	{
		return right-left;
	}
	
public:
	long top;
	long left;
	long right;
	long bottom;
};




class IntImage
{
public:
	IntImage(void);
	~IntImage(void);
	void Clear();
	void SetSize(const ZSize size);
	void CalculateVarianceAndIntegralImageInPlace();
	void CalcSquareAndIntegral(IntImage& square,IntImage& image) const;
	void Load(string filename);
	void Load(Mat& src);
	void IntImage::Copy(const IntImage& source);
	void Resize(IntImage &result, double ratio) const;

public:
	int height;
	int width;
	double** data;
	double* buf;
	double variance;
	int label;
};

void SwapIntImage(IntImage& i1,IntImage& i2);
#endif