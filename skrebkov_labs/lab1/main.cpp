#include <opencv2\core.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

const string helper = "lab1 --image <filename> --s <maxWindowSize>,\n\
					  <filename> is image filename,\n\
					  <maxWindowSize> is maximum window size for filter (default - 100)\n";

bool processCommandLine(int argc, char *argv[], string &filename, int &maxWindowSize);
Mat filter(const Mat &image, int maxWindowSize = 101);

int main(int argc, char *argv[]) {
	string filename;
	int maxWindowSize = 100;
	if (!processCommandLine(argc, argv, filename, maxWindowSize)) {
		cout << helper << endl;	
		return 1;
	}
	Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);

	Mat result = filter(image, maxWindowSize);
	
	if (!result.empty()) {
		imshow("Source", image);
		imshow("Result", result);
	}
	waitKey(0);
	
	return 0;
}

Mat filter(const Mat &image, int maxWindowSize)
{
	const float CANNY_LOW_THRESHOLD = 100.f;
	const float CANNY_HIGH_THRESHOLD = 300.f;

	Mat grayImage;
	cvtColor(image, grayImage, CV_BGR2GRAY);

	Mat cannyImage;
	Canny(grayImage, cannyImage, CANNY_LOW_THRESHOLD, CANNY_HIGH_THRESHOLD);
	Mat inverseCannyImage = ~cannyImage;

	Mat distImage;
	distanceTransform(inverseCannyImage, distImage, CV_DIST_L2, 3);

	Mat *imgChannels = new Mat[3]; 
	split(image, imgChannels);

	double minDist, maxDist;
	minMaxLoc(distImage, &minDist, &maxDist);

	float ratio = maxWindowSize / maxDist;
	int maxShift = maxWindowSize / 2;
	Mat *results = new Mat[3];
	for (int k = 0; k < 3; k++) {
		copyMakeBorder(imgChannels[k],  imgChannels[k], 
					   maxShift, maxShift, 
					   maxShift, maxShift, BORDER_REPLICATE);
		Mat integralImage;
		integral(imgChannels[k], integralImage);
		results[k].create(image.size(), CV_8U);
		for (int i = 0; i < image.rows; i++) {
			for (int j = 0; j < image.cols; j++) {
				int windowSize = (int)(distImage.at<float>(i, j) * ratio);
				if (windowSize > 1) {
					int shift = windowSize / 2;
					if (windowSize % 2 == 0) windowSize += 1;
					int mean = integralImage.at<int>(maxShift + i - shift, maxShift + j - shift) 
							 - integralImage.at<int>(maxShift + i - shift, maxShift + j + shift + 1)
							 - integralImage.at<int>(maxShift + i + shift + 1, maxShift + j - shift)
							 + integralImage.at<int>(maxShift + i + shift + 1, maxShift + j + shift + 1);
					mean = mean / (windowSize * windowSize);
					results[k].at<uchar>(i, j) = (uchar)mean;
				} else {
					results[k].at<uchar>(i, j) = imgChannels[k].at<uchar>(maxShift + i, maxShift + j);
				}
			}
		}
	}
	
	Mat result;
	merge(results, 3, result);

	delete[] imgChannels;
	delete[] results;

	return result;
}

bool processCommandLine(int argc, char *argv[], string &filename, int &maxWindowSize) {
	if (argc < 3)
		return false;
	for (int i = 1; i < argc; i++)
	{
		string str = argv[i];
		if (str.compare("--image") == 0) {
			filename = argv[i + 1];
		}
		if (str.compare("--s") == 0) {
			maxWindowSize = atoi(argv[i + 1]);
		}
	}

	return true;
}