#include <opencv2\core.hpp>
#include <opencv2\imgproc.hpp>
#include <opencv2\highgui.hpp>
#include <iostream>
#include <string>

using namespace cv;
using namespace std;

const string helper = "lab0 --image <filename>  --mode <mode>,\n\
					  where <filename> image filename,\n\
					  <mode> processing mode (may be 'blur', 'houghLines', 'findContours', defauld = 'blur')\n";

bool processCommandLine(int argc, char *argv[], string &filename, string &mode);

int main(int argc, char *argv[]) {
	string filename, mode;
	mode = "blur";
	if (!processCommandLine(argc, argv, filename, mode)) {
		cout << helper << endl;	
		return 1;
	}

	Mat image = imread(filename, CV_LOAD_IMAGE_COLOR);

	Mat result;
	if (mode.compare("blur") == 0) {
		blur(image, result, Size(5, 5));
	} else if (mode.compare("houghLines") == 0) {
		cvtColor(image, image, CV_BGR2GRAY);
		threshold(image, image, 127, 255, THRESH_BINARY);

		vector<Vec2f> lines;
		HoughLines(image, lines, 1, CV_PI/180, 100, 0, 0 );

		result = Mat::zeros(image.size(), CV_8UC3);
		for(size_t i = 0; i < lines.size(); i++)
		{
			float rho = lines[i][0], theta = lines[i][1];
			Point pt1, pt2;
			double a = cos(theta), b = sin(theta);
			double x0 = a*rho, y0 = b*rho;
			pt1.x = cvRound(x0 + 1000*(-b));
			pt1.y = cvRound(y0 + 1000*(a));
			pt2.x = cvRound(x0 - 1000*(-b));
			pt2.y = cvRound(y0 - 1000*(a));
			line(result, pt1, pt2, Scalar(0, 0, 255), 3, CV_AA);
		}
	} else if (mode.compare("findContours") == 0) {
		cvtColor(image, image, CV_BGR2GRAY);
		threshold(image, image, 127, 255, THRESH_BINARY);
		
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		findContours(image, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );

		result = Mat::zeros( image.size(), CV_8UC3 );
		for(size_t i = 0; i < contours.size(); i++ )
		{
			drawContours(result, contours, i, Scalar(0, 0, 255), 2, 8, hierarchy, 0, Point() );
		}
	} else {
		cout << "Wrong mode." << endl;
	}

	if (!result.empty()) {
		imshow("Result", result);
	}
	waitKey(0);

	return 0;
}

bool processCommandLine(int argc, char *argv[], string &filename, string &mode) {
	if (argc < 3)
		return false;
	for (int i = 1; i < argc; i++)
	{
		string str = argv[i];
		if (str.compare("--image") == 0) {
			filename = argv[i + 1];
		} else if (str.compare("--mode") == 0)
		{
			mode = argv[i + 1];
		}
	}

	return true;
}