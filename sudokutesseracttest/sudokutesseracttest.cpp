#include "opencv.hpp"
#include<leptonica/allheaders.h>
#include<baseapi.h>
#include<fstream>

#pragma comment(lib,"liblept168")
#pragma comment(lib,"libtesseract302")

using namespace cv;
using namespace std;
using namespace tesseract;
int main()
{



	Mat findImage = imread("sudo.jpg", IMREAD_GRAYSCALE); //이 이미지

	int tempsudo[81] = { 0 };
	int sudoku[9][9] = { 0 };




	Mat cloneImage = findImage.clone();
	threshold(findImage, findImage, 200, 255, THRESH_BINARY);
	//adaptiveThreshold(findImage, findImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, 2);
	imshow("findImage", findImage);
	waitKey();
	Mat srcImage2 = findImage.clone();

	Mat dstImage(srcImage2.size(), CV_8UC3);

	vector<vector<Point> > beforeContoursforRectangle;//스도쿠 이미지만 빼기위한 경계선
	vector<vector<Point> > AfterContoursforSmallRectangle; //스도쿠에서 작은 네모만 빼기위한경계선
	vector<vector<Point> > configurBackground;

	vector<Vec4i> hierarchy; //외곽선용
	vector<Vec4i> hierarchy2; //81개칸용
	vector<Vec4i> hierarchy3; //배경 검사용


	vector<vector<Point>> cutSize;//외곽선용
	vector<vector<Point>> cutSize2;//81개칸용
	vector<vector<Point>> cutSize3; //배경 검사용



	int mode = RETR_TREE;
	int method = CHAIN_APPROX_NONE;
	Rect r;
	findContours(findImage, beforeContoursforRectangle, hierarchy, mode, method); //윤곽석 검출


	cout << "beforeContoursforRectangle.size()=" << beforeContoursforRectangle.size() << endl; //윤곽선네모 개수

	cout << findImage.size() << endl; //이미지 크기

	int countMax = beforeContoursforRectangle[0].size(); //최대크기를 찾기위한 변수선언

	for (int k = 0; k < beforeContoursforRectangle.size(); k++)
	{
		if (beforeContoursforRectangle[k].size()>countMax)
			countMax = beforeContoursforRectangle[k].size();
	}

	for (int k = 0; k < beforeContoursforRectangle.size(); k++)
	{
		if (beforeContoursforRectangle[k].size()>(countMax*0.6))//스토쿠 판만 빼오기 
		{
			cutSize.push_back(beforeContoursforRectangle[k]);
		}
	}
	/*
	//테스트용
	Mat testtImage;
	for (int k = 0; k < cutSize.size(); k++)
	{
	//
	cvtColor(findImage, testtImage, COLOR_GRAY2BGR);
	r = boundingRect(cutSize[k]);//영역 사각형으로 감쌈
	Rect ROI2(r.x, r.y, r.width, r.height);
	drawContours(testtImage, cutSize, k, Scalar(255, 0, 0), 4); //경계선 그림
	cout << r.size() << endl;

	rectangle(testtImage, ROI2, Scalar(128, 255, 255), 2);//네모로 크림




	circle(testtImage, cutSize[k][0], 5, Scalar(123, 123, 123), -1);
	imshow("testtImage", testtImage);
	waitKey();
	}
	*/



	vector<vector<Point>> cuttingArea;
	int pushArea = -1;
	int smaller = cutSize[0].size();

	for (int k = 1; k < cutSize.size(); k++)
	{
		if (cutSize[k].size() < smaller)
		{
			smaller = cutSize[k].size(); //제일 작은거 가져옴 sort(cutsize.begin(),cutsize.end())가안되서 이방법 씀
			pushArea = k;
		}
	}
	cuttingArea.push_back(cutSize[pushArea]);


	r = boundingRect(cuttingArea[0]);
	Rect ROI(r.x, r.y, r.width, r.height); //자를곳을 골라서 영역 표시 

	cout << "bigSize.size()=" << cutSize.size() << endl;




	int eraseSpot = -1;
	int again = 0;



	int mode2 = RETR_LIST;
	int mode3 = RETR_LIST;
	int method2 = CHAIN_APPROX_NONE;
	int method3 = CHAIN_APPROX_NONE;

	Mat roi = cloneImage(ROI); //해당 영역만 가지고옴
	Mat newImage = repeat(roi, 1, 1);
	Mat contourImage = newImage.clone(); //배경에 음영있는것들 검출
	for (int a = 0; a < contourImage.rows; a++)
	{
		for (int b = 0; b < contourImage.cols; b++)
		{
			float te = contourImage.at<uchar>(a, b);
			if (te >= 160 && te < 190)
				contourImage.at<uchar>(a, b) = 250;
		}
	}

	threshold(contourImage, contourImage, 230, 255, THRESH_BINARY);


	imshow("contourImage", contourImage);
	waitKey();
	vector<int> configureRectSizeStorage; //빈칸과 숫자칸 저장
	vector<int> configureRect; //빈칸숫자칸의 최대 최소값을 알기위해 만듬
	vector<Rect> countRect;
	findContours(contourImage, configurBackground, hierarchy3, mode3, method3);

	for (int k = 0; k < configurBackground.size(); k++)
	{
		cutSize3.push_back(configurBackground[k]); //그 경계선위치들을 넣음
	}

	Mat configureBackgroundImage(contourImage.size(), CV_8UC1);
	for (int k = 0; k < cutSize3.size(); k++)
	{
		//
		cvtColor(contourImage, configureBackgroundImage, COLOR_GRAY2BGR);
		r = boundingRect(cutSize3[k]);//영역 사각형으로 감쌈
		Rect ROI2(r.x, r.y, r.width, r.height);
		drawContours(configureBackgroundImage, cutSize3, k, Scalar(255, 0, 0), 4); //경계선 그림
		//cout << r.size() << endl;

		rectangle(configureBackgroundImage, ROI2, Scalar(128, 255, 255), 2);//네모로 크림

		if ((r.width*r.height)>(configureBackgroundImage.rows*configureBackgroundImage.cols) - 20000)
		{		//최대영역은 받지 않음
			//imshow("resultImage", resultImage);
			//waitKey();
			goto here234;

		}
		configureRectSizeStorage.push_back(r.width*r.height); //sorting하기 위해 정리
	here234:{}

		circle(configureBackgroundImage, cutSize3[k][0], 5, Scalar(123, 123, 123), -1);
		//imshow("resultImage", configureBackgroundImage);
		//waitKey();
	}


	sort(configureRectSizeStorage.begin(), configureRectSizeStorage.end());

	int tSpot = -1;
	int tagain = 0;


	for (int k = 0; k < configureRectSizeStorage.size(); k++)
	{
		if (k == configureRectSizeStorage.size() - 1)
			break;

		if ((configureRectSizeStorage[k + 1] - configureRectSizeStorage[k]) >500)
		{
			tSpot = k; //500이 차이나면 따로 넣을곳 정리 
			break;
		}
	}

	for (int k = tSpot + 1; k < configureRectSizeStorage.size(); k++)
	{
		if (tSpot == -1)
			configureRect.push_back(configureRectSizeStorage[tagain++]); //그냥 집어 넣음
		else
			configureRect.push_back(configureRectSizeStorage[k]); //81개 네모 영역만 집어넣음
	}

	int minRectSize2 = configureRect.front(); //최소 네모 사이즈
	int maxRectSize2 = configureRect[configureRect.size() - 1]; //최대 네모 사이즈
	Mat lastConfigure;
	for (int k = 0; k < cutSize3.size(); k++)
	{

		cvtColor(contourImage, lastConfigure, COLOR_GRAY2BGR);
		//imshow("testImage", testImage);
		r = boundingRect(cutSize3[k]);
		Rect ROI2(r.x, r.y, r.width, r.height);
		//cout << r.size() << endl;

		rectangle(lastConfigure, ROI2, Scalar(128, 255, 255), 2);//그림

		if ((minRectSize2 <= (r.width*r.height)) && ((r.width*r.height) <= maxRectSize2))
			countRect.push_back(ROI2); //해당 영역만 비교 하기 위해 넣어줌
		//imshow("lastConfigure", lastConfigure);
		//waitKey();
	}





	vector<int>params; //압축양식사용

	params.push_back(IMWRITE_JPEG_QUALITY);
	params.push_back(9);

	imwrite("sudoOnly.jpg", newImage, params);
	Mat sudokuImage = imread("sudoOnly.jpg", IMREAD_GRAYSCALE);
	Mat realImage = imread("sudoOnly.jpg", IMREAD_GRAYSCALE);
	Mat testSudo = sudokuImage.clone(); //나중에 확인할거
	Size size(5, 5);
	Mat rectKernel = getStructuringElement(MORPH_RECT, size);
	Mat elipseKernel = getStructuringElement(MORPH_ELLIPSE, size);
	Mat crossKernel = getStructuringElement(MORPH_CROSS, size);


	if (countRect.size() != 81)
	{
		cout << "안되지롱" << endl;
		waitKey();

		goto jumpThreshold;
	}

	threshold(sudokuImage, sudokuImage, 230, 255, THRESH_BINARY); //영상 이진화 
	imshow("teswtsteswtse", sudokuImage);
	waitKey();
	goto jumpAdaptiveThreshold;
	//morphologyEx(sudokuImage, sudokuImage, MORPH_CLOSE, rectKernel, Point(-1, -1), 1);

jumpThreshold:
	//threshold(sudokuImage, sudokuImage, 200, 255, THRESH_BINARY);
	//threshold(sudokuImage, sudokuImage, 200, 255, THRESH_OTSU+THRESH_BINARY); //영상 이진화 

	adaptiveThreshold(sudokuImage, sudokuImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 21, 1);
	//morphologyEx(sudokuImage, sudokuImage, MORPH_GRADIENT, crossKernel, Point(-1, -1), 1);
	erode(sudokuImage, sudokuImage, crossKernel, Point(-1, -1), 1);
	dilate(sudokuImage, sudokuImage, elipseKernel, Point(-1, -1), 1);
jumpAdaptiveThreshold:



	imshow("sudokuImage", sudokuImage);
	waitKey();

	Mat testImage = realImage.clone();//스도쿠 된것만 복사시킴
	Mat resultImage(testImage.size(), CV_8UC3);
	Mat testSudoImage(realImage.size(), CV_8UC3);
	vector<int> rectSizeStorage; //빈칸과 숫자칸 저장
	vector<int> realRect; //빈칸숫자칸의 최대 최소값을 알기위해 만듬
	vector<Rect> sudokuRect; //칸마다 잘라진 영역 저장된곳
	vector<Rect> temp;
	threshold(testImage, testImage, 200, 255, THRESH_OTSU + THRESH_BINARY);
	//waitKey();
	findContours(sudokuImage, AfterContoursforSmallRectangle, hierarchy2, mode2, method2);
	//스도쿠 원형만 자른곳에서 다시 경계선 그림
	for (int k = 0; k < AfterContoursforSmallRectangle.size(); k++)
	{
		cutSize2.push_back(AfterContoursforSmallRectangle[k]); //그 경계선위치들을 넣음
	}



	for (int k = 0; k < cutSize2.size(); k++)
	{
		//
		cvtColor(testImage, resultImage, COLOR_GRAY2BGR);
		r = boundingRect(cutSize2[k]);//영역 사각형으로 감쌈
		Rect ROI2(r.x, r.y, r.width, r.height);
		drawContours(resultImage, cutSize2, k, Scalar(255, 0, 0), 4); //경계선 그림
		//cout << r.size() << endl;

		rectangle(resultImage, ROI2, Scalar(128, 255, 255), 2);//네모로 크림

		if ((r.width*r.height)>(resultImage.rows*resultImage.cols) - 20000)
		{		//최대영역은 받지 않음
			//imshow("resultImage", resultImage);
			//waitKey();
			goto here;

		}
		rectSizeStorage.push_back(r.width*r.height); //sorting하기 위해 정리
	here:{}

		circle(resultImage, cutSize2[k][0], 5, Scalar(123, 123, 123), -1);
		//imshow("resultImage", resultImage);
		//waitKey();
	}


	sort(rectSizeStorage.begin(), rectSizeStorage.end());

	eraseSpot = -1;
	again = 0;


	for (int k = 0; k < rectSizeStorage.size(); k++)
	{
		if (k == rectSizeStorage.size() - 1)
			break;

		if ((rectSizeStorage[k + 1] - rectSizeStorage[k]) >500)
		{
			eraseSpot = k; //500이 차이나면 따로 넣을곳 정리 
		}
	}

	for (int k = eraseSpot + 1; k < rectSizeStorage.size(); k++)
	{
		if (eraseSpot == -1)
			realRect.push_back(rectSizeStorage[again++]); //그냥 집어 넣음
		else
			realRect.push_back(rectSizeStorage[k]); //81개 네모 영역만 집어넣음
	}


	//소팅이 이미 되서 들어가므로 할 필요 없음
	int minRectSize = realRect.front(); //최소 네모 사이즈
	int maxRectSize = realRect[realRect.size() - 1]; //최대 네모 사이즈

	for (int k = 0; k < cutSize2.size(); k++)
	{

		cvtColor(testImage, resultImage, COLOR_GRAY2BGR);
		//imshow("testImage", testImage);
		r = boundingRect(cutSize2[k]);
		Rect ROI2(r.x, r.y, r.width, r.height);
		//cout << r.size() << endl;

		rectangle(resultImage, ROI2, Scalar(128, 255, 255), 2);//그림

		if ((minRectSize <= (r.width*r.height)) && ((r.width*r.height) <= maxRectSize))
			temp.push_back(ROI2); //해당 영역만 비교 하기 위해 넣어줌
		//	imshow("resultImage2", resultImage);
		//waitKey();
	}

	for (int k = temp.size() - 1; k >= 0; k--)
		sudokuRect.push_back(temp[k]); //반대로 시작하므로 처음1칸부터 비교하기위해 역으로 넣음

	temp.clear();



	vector<int> testSortingOk;
	//각 줄이 순서대로 소팅이 안되있으므로 각 줄마다 왼쪽부터 오른쪽으로 되게 바꿈
	for (int i = 0; i < 81; i += 9)
	{
		int ty = sudokuRect[i].y;
		int ti = i;
		testSortingOk.clear();
		for (int j = i; j < ti + 9; j++)
		{
			testSortingOk.push_back(sudokuRect[j].x);
		}
		sort(testSortingOk.begin(), testSortingOk.end());
		int k = 0;
		for (int j = i; j < ti + 9; j++)
		{
			if (testSortingOk[k] == sudokuRect[j].x)
				temp.push_back(sudokuRect[j]);
			else
			{
				for (int t = i; t <= ti + 9; t++)
				{
					if (testSortingOk[k] == sudokuRect[t].x)
					{
						temp.push_back(sudokuRect[t]);
						break;
					}
				}
			}

			k++;
		}

	}

	sudokuRect.clear();
	erode(resultImage, resultImage, crossKernel, Point(-1, -1), 1);
	dilate(resultImage, resultImage, elipseKernel, Point(-1, -1), 1);
	for (int k = 0; k < temp.size(); k++)
		sudokuRect.push_back(temp[k]);
	for (int k = 0; k < sudokuRect.size(); k++)
	{
		rectangle(resultImage, sudokuRect[k], Scalar(128, 255, 255), 2);//그림
		imshow("testSudoImage34", resultImage);
		waitKey();
	}

		const char *path = "/tessdate";
	TessBaseAPI tess; //tesseract 선언

		//TessBaseAPI *api = new TessBaseAPI();

	tess.Init(NULL, "eng", tesseract::OEM_DEFAULT); //한국어로 인식 기본 숫자 되어있음
	/*
		if (api->Init(NULL, "eng"))
		{
			fprintf(stderr, "could not initailze teseract\n");
			exit(1);
		}
		*/

	string tessRecogNum[81][1]; //글자로 인식해오므로 넣을곳 만들어줌
	int sudoTestte[81]; //스도쿠 번호 넣을곳을 만들어줌
	int d = 0;

	for (int i = 0; i<sudokuRect.size(); i++)
	{
		cvtColor(testSudo, testSudoImage, COLOR_GRAY2BGR);
		rectangle(testSudoImage, sudokuRect[i], Scalar(128, 255, 255), 2);//그림

		Mat cutImage = testSudo(sudokuRect[i]);
		//	imshow("cutImage", cutImage);
		Mat tempImage = repeat(cutImage, 1, 1);


		Mat numberImage = tempImage.clone();//스도쿠 된것만 복사시킴
		//imshow("numberImage", numberImage);
		imwrite("numberImage.jpg", numberImage, params);
		Mat resultNImage(tempImage.size(), CV_8UC3);
		waitKey();

		int height = tempImage.size().height;
		int width = tempImage.size().width;

		int continueNum = 0; //0이 계속되는 수 선언
		int testPixel = (height*width); //최대 픽셀 받아옴

		//그 칸이 빈칸일 경우 골라서 0을 넣음 
		/*
		for (int a = 0; a < tempImage.rows; a++)
		{
			for (int b = 0; b < tempImage.cols; b++)
			{
				float te = tempImage.at<uchar>(a, b);
				if (continueNum >= testPixel)
					break;
				if (te >= 190 && te <= 200)
					tempImage.at<uchar>(a, b) = 255;

				if (te > 200)
					continueNum++;
			}
			if (continueNum >= (testPixel - 10))
				break;
		}
		*/
		if (continueNum >= testPixel){
			sudoTestte[d++] = atoi("0");
			tessRecogNum[i][0] = '0';
			continue;
		}

		//imshow("tempImage", tempImage);

		//waitKey();

	tess.SetImage((uchar*)tempImage.data, tempImage.size().width, tempImage.size().height, tempImage.channels(), tempImage.step1()); //이미지의 데이터 넓이 높이와 채널을 받아옴

		//Pix *image = pixRead("numberImage.jpg");
		//api->SetImage(image);
//
		tess.Recognize(0); //인식 시킴
		const char * out = tess.GetUTF8Text(); //그걸 utf8text로 인식시킴
		//const char * out = api.GetUTF8Text();
		tessRecogNum[i][0] = out;
		if (atoi(out) > 9 || atoi(out) < 0)
			sudoTestte[d++] = 0;
		else
		sudoTestte[d++] = atoi(out);
		//printf("%s", out);

		//cout << endl;

		imshow("testSudoImage", testSudoImage);
		//waitKey();
	}

	cout << endl;
	for (int i = 0; i < 81; i++)
	{
		if (i % 9 == 0 && i>1)
			cout << endl;
		cout << sudoTestte[i] << " ";

	}


	waitKey();


	return 0;
}

