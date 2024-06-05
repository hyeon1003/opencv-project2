//opencv 필기체 숫자인식 프로젝트
//readme파일 업로드시 정지 당하기 떄문에 주석으로 대체
#include<iostream>
#include<opencv2/opencv.hpp>
using namespace std;
using namespace cv;
void menu(Mat& img);//기본UI창 함수선언
void Savefile(Mat& src);//save함수선언
void Roadfile(Mat& src);//road 함수선언
void Clear(Mat& src);//입력창 삭제함수선언
void Run(Mat& src);//인식 함수 선언
void Exit();//종료함수 선언
int Contour(Mat& src);//외각선 검출함수 선언
void Contour2(Mat& src);//2번째특성검출 함수 선언
void on_mouse(int event, int x, int y, int flags, void* userdata);//마우스이벤트 함수선언
int main(void)//메인함수
{
	Mat src(500, 900, CV_8UC3, Scalar(255, 255, 255));//윈도우크기 500*700
	menu(src);//메뉴함수 호출
	namedWindow("src");//마우스이벤트 함수사용을 위해 미리 윈도우 생성
	setMouseCallback("src", on_mouse, &src);//마우스 함수 호출
	while (true)//무한반복문
	{
		imshow("src", src);//이미지 최신화
		waitKey(1);//0.001초마다
	}
	return 0;//정상적인 종료
}
void menu(Mat& img)//메뉴함수 정의
{
	Mat src = img;
	rectangle(src, Rect(0, 0, 700, 500), Scalar(0, 0, 0), 1);//윈도우창에 사각형을 그림
	line(src, Point(499, 0), Point(499, 499), Scalar(0, 0, 0), 1);//숫자입력창과 옵션을 구분하는 라인
	for (int i = 1; i <= 4; ++i) {//save,load,clear,run,exit를 구분하는 라인
		line(src, Point(500, i * 100 - 1), Point(699, i * 100 - 1), Scalar(0, 0, 0), 1);
	}
	for (int i = 0; i < 200;)
	{
		line(src, Point(700, i), Point(900, i), Scalar::all(0), 1);
		line(src, Point(900, i), Point(900, i + 100), Scalar::all(0), 1);
		if (i == 0)i += 99;
		else i += 100;
	}
	String massage[] = { "Save","Load","Clear","Run","Exit","contour","contour" };//ui를 배열로 저장
	int y = 0;//반복하면서 100씩더할 예정
	Mat dst = src(Rect(500, 0, 200, 500));//문자열을 윈도우에 쓰기위해서 
	Size dstsize = dst.size();//dst의 사이즈를 저장할 객체
	for (int i = 0; i < 5; i++)//UI에 아까저장한 배열을 이용하여 가운데에 문자열 출력
	{
		Size sizeText = getTextSize(massage[i], FONT_HERSHEY_TRIPLEX, 1.0, 1, 0);
		Point org((dstsize.width - sizeText.width) / 2, (dstsize.height / 5 + sizeText.height) / 2 + y);//가운데에 문자열을 출력하기 위해 계산 y에 100씩 더해서 ui에 가운데출력
		putText(dst, massage[i], org, FONT_HERSHEY_TRIPLEX, 1.0, 1);//문자열 출력함수 호출
		y += 100;//아래로 100씩 차이가 나므로 100씩더해줌
	}
	y = 0;
	Mat dst2 = src(Rect(700, 0, 200, 200));//새로운 특징추출
	Size dst2size = dst2.size();
	for (int i = 5; i < 7; i++)
	{
		Size sizeText = getTextSize(massage[i], FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0);
		Point org((dst2size.width - sizeText.width) / 2, (dst2size.height / 2 + sizeText.height) / 2 + y);
		putText(dst2, massage[i], org, FONT_HERSHEY_TRIPLEX, 1.0, 1.0, 0);
		y += 100;
	}

}
void on_mouse(int event, int x, int y, int flags, void* userdata)//마우스 콜백함수 정의
{
	Mat src = *(Mat*)userdata;//원본 이미지를 src에 저장
	static Point ptPrev(-1, -1);//이전 위치를 저장하는 객체
	switch (event)
	{
	case EVENT_LBUTTONDOWN://마우스 왼쪽 버튼이 눌렸을떄
	{
		ptPrev = Point(x, y);//이전 버튼위치를 현재위치로 저장
		if (ptPrev.inside(Rect(500, 0, 200, 100)))//save 위치일때
		{
			Savefile(src);//함수호출하여 저장
		}
		else if (ptPrev.inside(Rect(500, 100, 200, 100)))//load 위치일때
		{
			Roadfile(src);//함수호출하여 파일을 load
		}
		else if (ptPrev.inside(Rect(500, 200, 200, 100)))//clear 위치일때
		{
			Clear(src);//함수호출후 숫자입력창 clear
		}
		else if (ptPrev.inside(Rect(500, 300, 200, 100)))//run 위치일때
		{
			Run(src);//함수 호출후 인식
		}
		else if (ptPrev.inside(Rect(500, 400, 200, 100)))//exit 위치일때
		{
			Exit();//함수 호출후 프로그램 종료
		}
		else if (ptPrev.inside(Rect(700, 0, 200, 100)))
		{
			Contour(src);//특성함수 호출
		}
		else if (ptPrev.inside(Rect(700, 100, 200, 100)))
		{
			Contour2(src);//특성함수 호출
		}
		break;
	}
	case EVENT_MOUSEMOVE://마우스를 움직일때
	{
		if (flags & EVENT_FLAG_LBUTTON) {//왼쪽 버튼이 눌려있으면
			if (Rect(0, 0, 500, 500).contains(ptPrev) && Rect(0, 0, 500, 500).contains(Point(x, y))) {//숫자입력창500*500 위치일때(과거,현재)
				line(src, ptPrev, Point(x, y), Scalar::all(0), 5, LINE_AA, 0);//이전위치와 현재위치를 라인으로 그려줌 
				ptPrev = Point(x, y);//이전위치를 현재위치로 저장
			}
		}
		break;
	}
	}
}
void Savefile(Mat& src)//save 함수 정의
{
	String file;//저장할 파일이름을 저장할 객체선언
	cout << "파일명입력:";
	cin >> file;//파일이름을 입력
	Mat dst = src(Rect(3, 3, 496, 496));
	resize(dst, dst, Size(500, 500));
	imwrite(file, dst);//컬러영상파일로 프로젝트 폴더에 저장
	cout << file << "파일이 저장됨" << endl;
}
void Roadfile(Mat& src)//load 함수 정의
{
	String file;//load할 파일이름을 저장할 객체선언
	cout << "파일명을 입력:";
	cin >> file;//파일이름을 입력
	Mat dst = imread(file, IMREAD_COLOR);//dst에 파일을 컬러로 불러옴
	if (dst.empty()) { cerr << "파일이 존재하지않습니다." << endl; return; }//불러오는 영상이 비어있을때
	dst(Rect(3, 3, 496, 496)).copyTo(src(Rect(3, 3, 496, 496)));//원본500*500(숫자입력창)에 깊은복사 라인은 유지
	cout << file << "파일을 불러옴" << endl;
}
void Clear(Mat& src)//clear 함수정의
{
	Mat dst = src(Rect(3, 3, 496, 496));//dst를 src숫자입력창 얕은복사
	dst = Scalar::all(255);//dst를 흰색으로 초기화(입력창 초기화)
	cout << "입력창 삭제됨" << endl;
}
void Run(Mat& src)//인식함수 정의
{
	cout << "인식결과:" << endl;
}
void Exit()//종료함수 정의
{
	cout << "프로그램 종료" << endl;
	exit(1);//프로그램 종료함수 호출
	return;
}
int Contour(Mat& src)//외각선 검출 함수(외각선 개수 리턴)
{
	int count_contour = 0;
	cout << "아직구현중" << endl;
	return count_contour;
}
void Contour2(Mat& src)
{
	cout << "아직구현중" << endl;
}
