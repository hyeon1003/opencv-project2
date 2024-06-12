#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
void menu(Mat& img);
void Savefile(Mat& src);
void Roadfile(Mat& src);
void Clear(Mat& src);
void Run(Mat& src);
void Exit();
Mat bounding_src(Mat& src);
Point CenterPt(Mat& src);
double aspect_ratio(Mat& src);
int Contour(Mat& src);
int left(Mat src);
int top(Mat src);
void on_mouse(int event, int x, int y, int flags, void* userdata);
int main(void)
{
	Mat src(500, 900, CV_8UC3, Scalar(255, 255, 255));
	menu(src);
	namedWindow("src");
	setMouseCallback("src", on_mouse, &src);
	while (true)
	{
		imshow("src", src);
		waitKey(1);
	}
	return 0;
}
void menu(Mat& img)
{
	Mat src = img;
	rectangle(src, Rect(500, 0, 400, 500), Scalar::all(0), 1);
	line(src, Point(700, 0), Point(700, 499), Scalar(0, 0, 0), 1);
	for (int i = 1; i <= 4; ++i) {
		line(src, Point(500, i * 100 - 1), Point(699, i * 100 - 1), Scalar(0, 0, 0), 1);
	}
	for (int i = 0; i < 400;) {
		line(src, Point(700, i), Point(900, i), Scalar::all(0), 1);
		line(src, Point(900, i), Point(900, i + 100), Scalar::all(0), 1);
		if (i == 0)i += 99;
		else i += 100;
	}
	String massage[] = { "Save", "Load", "Clear", "Run", "Exit", "contour", "CenterPt", "CfXcontour", "CfYcontour","Aspect_ratio" };
	int y = 0;
	Mat dst = src(Rect(500, 0, 200, 500));
	Size dstsize = dst.size();
	for (int i = 0; i < 5; i++) {
		Size sizeText = getTextSize(massage[i], FONT_HERSHEY_TRIPLEX, 0.8, 1, 0);
		Point org((dstsize.width - sizeText.width) / 2, (dstsize.height / 5 + sizeText.height) / 2 + y);
		putText(dst, massage[i], org, FONT_HERSHEY_TRIPLEX, 0.8, 1);
		y += 100;
	}
	y = 0;
	Mat dst2 = src(Rect(700, 0, 200, 500));
	Size dst2size = dst2.size();
	for (int i = 5; i < 10; i++) {
		Size sizeText = getTextSize(massage[i], FONT_HERSHEY_TRIPLEX, 0.8, 1.0, 0);
		Point org((dst2size.width - sizeText.width) / 2, (dst2size.height / 5 + sizeText.height) / 2 + y);
		putText(dst2, massage[i], org, FONT_HERSHEY_TRIPLEX, 0.8, 1, 0);
		y += 100;
	}
}
void on_mouse(int event, int x, int y, int flags, void* userdata)
{
	Mat src = *(Mat*)userdata;
	static Point ptPrev(-1, -1);
	switch (event)
	{
	case EVENT_LBUTTONDOWN:
	{
		ptPrev = Point(x, y);
		if (ptPrev.inside(Rect(500, 0, 200, 100))) {
			left(src);
			Savefile(src);
		}
		else if (ptPrev.inside(Rect(500, 100, 200, 100))) {
			Roadfile(src);
		}
		else if (ptPrev.inside(Rect(500, 200, 200, 100))) {
			Clear(src);
		}
		else if (ptPrev.inside(Rect(500, 300, 200, 100))) {
			Run(src);
		}
		else if (ptPrev.inside(Rect(500, 400, 200, 100))) {
			Exit();
		}
		else if (ptPrev.inside(Rect(700, 0, 200, 100))) {
			Contour(src);
		}
		else if (ptPrev.inside(Rect(700, 100, 200, 100))) {
			Point cf = CenterPt(src);
			cout << "X:" << cf.x << "%" << endl;
			cout << "Y:" << cf.y << "%" << endl;
		}
		else if (ptPrev.inside(Rect(700, 200, 200, 100))) {
			top(src);
		}
		else if (ptPrev.inside(Rect(700, 300, 200, 100))) {
			left(src);
		}
		else if (ptPrev.inside(Rect(700, 400, 200, 100))) {

			double ratio = aspect_ratio(src);
			cout << "가로세로비율:" << ratio * 100 << "%" << endl;
		}
		break;
	}
	case EVENT_MOUSEMOVE:
	{
		if (flags & EVENT_FLAG_LBUTTON) {
			if (Rect(0, 0, 500, 500).contains(ptPrev) && Rect(0, 0, 500, 500).contains(Point(x, y))) {
				line(src, ptPrev, Point(x, y), Scalar::all(0), 5, LINE_AA, 0);
				ptPrev = Point(x, y);
			}
		}
		break;
	}
	}
}
void Savefile(Mat& src)
{
	String file;
	cout << "파일명입력:";
	cin >> file;
	Mat dst = src(Rect(3, 3, 496, 496));
	resize(dst, dst, Size(500, 500));
	imwrite(file, dst);
	cout << file << "파일이 저장됨" << endl;
}
void Roadfile(Mat& src)
{
	String file;
	cout << "파일명을 입력:";
	cin >> file;
	Mat dst = imread(file, IMREAD_COLOR);
	if (dst.empty()) {
		cerr << "파일이 존재하지않습니다." << endl;
		return;
	}
	dst(Rect(3, 3, 496, 496)).copyTo(src(Rect(3, 3, 496, 496)));
	cout << file << "파일을 불러옴" << endl;
}
void Clear(Mat& src)
{
	Mat dst = src(Rect(3, 3, 496, 496));
	dst = Scalar::all(255);
	cout << "입력창 삭제됨" << endl;
}
void Run(Mat& src) {
	int count_contour = Contour(src);
	int count_x = top(src);
	int count_y = left(src);
	Point cf = CenterPt(src);
	double ar = aspect_ratio(src);
	if (count_contour == 3)cout << "인식한 숫자: 8" << endl;
	else if (count_contour == 1) {//외각선이 1개일때
		if (count_x == 1) {//x축으로 나눈 외각선이 1개일때
			if (count_y == 3)cout << "인식한 숫자: 3" << endl;
			else if (count_y == 1) {//y축으로 나눈 외각선이 1개일때
				if (ar * 100 < 40)
					cout << "인식한 숫자: 1" << endl;
				else if (ar * 100 > 80 && abs(cf.x - cf.y) >= 10)
					cout << "인식한 숫자: 2" << endl;
				else if (abs(cf.x - cf.y) >= 15)
					cout << "인식한 숫자: 7" << endl;
			}
			else if (count_y == 2) {//y축으로 나눈 외각선이 2개일때
				if (ar * 100 > 50) {
					if (abs(cf.x - cf.y) >= 10) {
						if (ar * 100 < 70)
							cout << "인식한 숫자: 7" << endl;
						else
							cout << "인식한 숫자: 2" << endl;
					}
					if (abs(cf.x = cf.y) >= 5)
					{
						cout << "인식한 숫자: 2" << endl;
					}
					else
						cout << "인식한 숫자: 5" << endl;
				}
				else if (abs(cf.x - cf.y) >= 10)
					cout << "인식한 숫자: 7" << endl;
				else
					cout << "인식한 숫자: 4" << endl;
			}
			else if (count_y == 3)
				cout << "인식한 숫자: 3" << endl;
		}
		else if (count_x == 2) {
			if (count_y == 1) {
				if (ar * 100 > 80)
					cout << "인식한 숫자: 4" << endl;
			}
		}
	}
	else if (count_contour == 2) {//외각선이 2개일때
		if (count_x==1)
		{
			if (count_y == 1)
			{
				if (abs(cf.x - cf.y) < 7)
					cout << "인식한 숫자: 0" << endl;
				else if (ar * 100 <= 70)
					cout << "인식한 숫자: 6" << endl;
				else if (ar * 100 >= 70)
					cout << "인식한 숫자: 0" << endl;
			}
			else if (count_y == 2) {//y축으로 나눌때 외각선이 2개일때
				cout << "인식한 숫자: 4" << endl;
			}
		}
		else if (count_x == 2) {
			if (ar * 100 > 70)
				cout << "인식한 숫자: 4" << endl;
			else
				cout << "인식한 숫자: 9" << endl;
		}
	}
}
void Exit()
{
	cout << "프로그램 종료" << endl;
	exit(1);
	return;
}
Mat morph(Mat& src)
{
	Mat dst, edge;
	cvtColor(src(Rect(0, 0, 500, 500)), dst, COLOR_BGR2GRAY);
	threshold(dst, edge, 150, 255, THRESH_BINARY_INV | THRESH_OTSU);
	Mat labels, stats, centroids;
	int cnt = connectedComponentsWithStats(edge, labels, stats, centroids);
	int morph_size = 5;
	if (cnt > 2) {
		while (true) {
			morphologyEx(edge, edge, MORPH_CLOSE, Mat(morph_size, morph_size, CV_8UC1));
			cnt = connectedComponentsWithStats(edge, labels, stats, centroids);
			if (cnt <= 2) break;
			morph_size += 3;
		}
	}
	return edge;
}
Mat bounding_src(Mat& src)
{
	Mat bin = morph(src);
	Mat labels, stats, centroids;
	int cnt = connectedComponentsWithStats(bin, labels, stats, centroids);
	if (cnt < 2) {
		cerr << "Bounding box not found. Possibly too small." << endl;
		return Mat();
	}
	int* p = stats.ptr<int>(1);
	int min_size = 10; // 최소 크기 설정
	int x = max(0, p[0] - min_size);
	int y = max(0, p[1] - min_size);
	int width = min(p[2] + 2 * min_size, bin.cols - x);
	int height = min(p[3] + 2 * min_size, bin.rows - y);
	Mat bin_img;
	bin_img = bin(Rect(x, y, width, height));
	imshow("bin", bin_img);
	return bin_img;
}
int left(Mat src) {//왼쪽 추출
	Mat clone = bounding_src(src);
	int centerX = clone.cols / 2; // 이미지의 가로 중심점 계산
	Mat left = clone(Rect(0, 0, centerX, clone.rows));// 가운데를 중심으로 왼쪽 영역 추출
	vector<vector<Point>> contours;
	findContours(left, contours, RETR_LIST, CHAIN_APPROX_NONE);// 왼쪽 영역에서 외곽선 찾기
	int left_contours = contours.size(); // 찾은 외곽선 개수 반환
	cout << "y축으로 반으로 잘랐을때 외각선: " << left_contours << endl;
	imshow("left", left);
	return left_contours;
}
int top(Mat src)//위에 추출
{
	Mat bin = bounding_src(src); // 전처리된 이미지 얻기
	int centerY = bin.rows / 2;// 이미지의 세로 중심점 계산
	Mat top = bin(Rect(0, 0, bin.cols, centerY));// 가운데를 중심으로 윗 부분 영역 추출
	vector<vector<Point>> contours;
	findContours(top, contours, RETR_LIST, CHAIN_APPROX_NONE);// 윗 부분 영역에서 외곽선 찾기
	int top_contours = contours.size();// 찾은 외곽선 개수 반환
	cout << "x축으로 반으로 잘랐을때 외각선: " << top_contours << endl;
	imshow("top", top);
	return top_contours;
}
int Contour(Mat& src)
{
	int count_contour = 0;
	Mat bin = bounding_src(src);
	if (bin.empty()) {
		cerr << "Failed to process bounding box." << endl;
		return 0;
	}
	vector<vector<Point>> contours;
	findContours(bin, contours, RETR_LIST, CHAIN_APPROX_NONE);
	count_contour = contours.size();
	cout << "외각선의 개수:" << count_contour << endl;
	return count_contour;
}
Point CenterPt(Mat& src)
{
	Mat bin = bounding_src(src);
	if (bin.empty()) {
		return Point(-1, -1);
	}
	Mat labels, stats, centroids;
	int cnt = connectedComponentsWithStats(bin, labels, stats, centroids);
	if (cnt < 2) {
		return Point(-1, -1);
	}
	int width = stats.at<int>(1, 2);
	int height = stats.at<int>(1, 3);
	int center_x = centroids.at<double>(1, 0);
	int center_y = centroids.at<double>(1, 1);
	int per_x = (double)center_x / width * 100;
	int per_y = (double)center_y / height * 100;
	return Point(per_x, per_y);
}
double aspect_ratio(Mat& src) {
	Mat bin = bounding_src(src);
	if (bin.empty()) {
		cerr << "Bounding box not found. Possibly too small." << endl;
		return 0.0;
	}
	vector<vector<Point>> contours;
	findContours(bin, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	int largest_contour_index = 0;
	double largest_area = 0.0;
	for (size_t i = 0; i < contours.size(); ++i) {
		double area = contourArea(contours[i]);
		if (area > largest_area) {
			largest_area = area;
			largest_contour_index = i;
		}
	}
	Rect bounding_box = boundingRect(contours[largest_contour_index]);
	double aspect_ratio = static_cast<double>(bounding_box.width) / bounding_box.height;
	return aspect_ratio;
}
