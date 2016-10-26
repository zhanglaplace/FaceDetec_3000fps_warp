#ifndef _CASCADECLASSIFIER_H
#define _CASCADECLASSIFIER_H
#include "AdaBoostClassifier.h"
#include "IntImage.h"
#include <vector>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/core/core.hpp"
using namespace std;
struct NCascadeClassifier
{
	int count;
	AdaBoostClassifier* ac;

	NCascadeClassifier();
	~NCascadeClassifier();
	void Clear(void);
	NCascadeClassifier& operator=(const NCascadeClassifier& source);

	void WriteToFile(ofstream& f) const;
	void ReadFromFile(ifstream& f);
	void LoadDefaultCascade(void);

	virtual void ApplyOriginalSize(IntImage& original,vector<cv::Rect>&box);

	void DrawResults(IntImage& image, const vector<ZRect>& results) const;

};

#endif