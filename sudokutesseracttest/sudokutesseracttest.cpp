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



	Mat findImage = imread("sudo.jpg", IMREAD_GRAYSCALE); //�� �̹���

	int tempsudo[81] = { 0 };
	int sudoku[9][9] = { 0 };




	Mat cloneImage = findImage.clone();
	threshold(findImage, findImage, 200, 255, THRESH_BINARY);
	//adaptiveThreshold(findImage, findImage, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 21, 2);
	imshow("findImage", findImage);
	waitKey();
	Mat srcImage2 = findImage.clone();

	Mat dstImage(srcImage2.size(), CV_8UC3);

	vector<vector<Point> > beforeContoursforRectangle;//������ �̹����� �������� ��輱
	vector<vector<Point> > AfterContoursforSmallRectangle; //�������� ���� �׸� �������Ѱ�輱
	vector<vector<Point> > configurBackground;

	vector<Vec4i> hierarchy; //�ܰ�����
	vector<Vec4i> hierarchy2; //81��ĭ��
	vector<Vec4i> hierarchy3; //��� �˻��


	vector<vector<Point>> cutSize;//�ܰ�����
	vector<vector<Point>> cutSize2;//81��ĭ��
	vector<vector<Point>> cutSize3; //��� �˻��



	int mode = RETR_TREE;
	int method = CHAIN_APPROX_NONE;
	Rect r;
	findContours(findImage, beforeContoursforRectangle, hierarchy, mode, method); //������ ����


	cout << "beforeContoursforRectangle.size()=" << beforeContoursforRectangle.size() << endl; //�������׸� ����

	cout << findImage.size() << endl; //�̹��� ũ��

	int countMax = beforeContoursforRectangle[0].size(); //�ִ�ũ�⸦ ã������ ��������

	for (int k = 0; k < beforeContoursforRectangle.size(); k++)
	{
		if (beforeContoursforRectangle[k].size()>countMax)
			countMax = beforeContoursforRectangle[k].size();
	}

	for (int k = 0; k < beforeContoursforRectangle.size(); k++)
	{
		if (beforeContoursforRectangle[k].size()>(countMax*0.6))//������ �Ǹ� ������ 
		{
			cutSize.push_back(beforeContoursforRectangle[k]);
		}
	}
	/*
	//�׽�Ʈ��
	Mat testtImage;
	for (int k = 0; k < cutSize.size(); k++)
	{
	//
	cvtColor(findImage, testtImage, COLOR_GRAY2BGR);
	r = boundingRect(cutSize[k]);//���� �簢������ ����
	Rect ROI2(r.x, r.y, r.width, r.height);
	drawContours(testtImage, cutSize, k, Scalar(255, 0, 0), 4); //��輱 �׸�
	cout << r.size() << endl;

	rectangle(testtImage, ROI2, Scalar(128, 255, 255), 2);//�׸�� ũ��




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
			smaller = cutSize[k].size(); //���� ������ ������ sort(cutsize.begin(),cutsize.end())���ȵǼ� �̹�� ��
			pushArea = k;
		}
	}
	cuttingArea.push_back(cutSize[pushArea]);


	r = boundingRect(cuttingArea[0]);
	Rect ROI(r.x, r.y, r.width, r.height); //�ڸ����� ��� ���� ǥ�� 

	cout << "bigSize.size()=" << cutSize.size() << endl;




	int eraseSpot = -1;
	int again = 0;



	int mode2 = RETR_LIST;
	int mode3 = RETR_LIST;
	int method2 = CHAIN_APPROX_NONE;
	int method3 = CHAIN_APPROX_NONE;

	Mat roi = cloneImage(ROI); //�ش� ������ �������
	Mat newImage = repeat(roi, 1, 1);
	Mat contourImage = newImage.clone(); //��濡 �����ִ°͵� ����
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
	vector<int> configureRectSizeStorage; //��ĭ�� ����ĭ ����
	vector<int> configureRect; //��ĭ����ĭ�� �ִ� �ּҰ��� �˱����� ����
	vector<Rect> countRect;
	findContours(contourImage, configurBackground, hierarchy3, mode3, method3);

	for (int k = 0; k < configurBackground.size(); k++)
	{
		cutSize3.push_back(configurBackground[k]); //�� ��輱��ġ���� ����
	}

	Mat configureBackgroundImage(contourImage.size(), CV_8UC1);
	for (int k = 0; k < cutSize3.size(); k++)
	{
		//
		cvtColor(contourImage, configureBackgroundImage, COLOR_GRAY2BGR);
		r = boundingRect(cutSize3[k]);//���� �簢������ ����
		Rect ROI2(r.x, r.y, r.width, r.height);
		drawContours(configureBackgroundImage, cutSize3, k, Scalar(255, 0, 0), 4); //��輱 �׸�
		//cout << r.size() << endl;

		rectangle(configureBackgroundImage, ROI2, Scalar(128, 255, 255), 2);//�׸�� ũ��

		if ((r.width*r.height)>(configureBackgroundImage.rows*configureBackgroundImage.cols) - 20000)
		{		//�ִ뿵���� ���� ����
			//imshow("resultImage", resultImage);
			//waitKey();
			goto here234;

		}
		configureRectSizeStorage.push_back(r.width*r.height); //sorting�ϱ� ���� ����
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
			tSpot = k; //500�� ���̳��� ���� ������ ���� 
			break;
		}
	}

	for (int k = tSpot + 1; k < configureRectSizeStorage.size(); k++)
	{
		if (tSpot == -1)
			configureRect.push_back(configureRectSizeStorage[tagain++]); //�׳� ���� ����
		else
			configureRect.push_back(configureRectSizeStorage[k]); //81�� �׸� ������ �������
	}

	int minRectSize2 = configureRect.front(); //�ּ� �׸� ������
	int maxRectSize2 = configureRect[configureRect.size() - 1]; //�ִ� �׸� ������
	Mat lastConfigure;
	for (int k = 0; k < cutSize3.size(); k++)
	{

		cvtColor(contourImage, lastConfigure, COLOR_GRAY2BGR);
		//imshow("testImage", testImage);
		r = boundingRect(cutSize3[k]);
		Rect ROI2(r.x, r.y, r.width, r.height);
		//cout << r.size() << endl;

		rectangle(lastConfigure, ROI2, Scalar(128, 255, 255), 2);//�׸�

		if ((minRectSize2 <= (r.width*r.height)) && ((r.width*r.height) <= maxRectSize2))
			countRect.push_back(ROI2); //�ش� ������ �� �ϱ� ���� �־���
		//imshow("lastConfigure", lastConfigure);
		//waitKey();
	}





	vector<int>params; //�����Ļ��

	params.push_back(IMWRITE_JPEG_QUALITY);
	params.push_back(9);

	imwrite("sudoOnly.jpg", newImage, params);
	Mat sudokuImage = imread("sudoOnly.jpg", IMREAD_GRAYSCALE);
	Mat realImage = imread("sudoOnly.jpg", IMREAD_GRAYSCALE);
	Mat testSudo = sudokuImage.clone(); //���߿� Ȯ���Ұ�
	Size size(5, 5);
	Mat rectKernel = getStructuringElement(MORPH_RECT, size);
	Mat elipseKernel = getStructuringElement(MORPH_ELLIPSE, size);
	Mat crossKernel = getStructuringElement(MORPH_CROSS, size);


	if (countRect.size() != 81)
	{
		cout << "�ȵ�����" << endl;
		waitKey();

		goto jumpThreshold;
	}

	threshold(sudokuImage, sudokuImage, 230, 255, THRESH_BINARY); //���� ����ȭ 
	imshow("teswtsteswtse", sudokuImage);
	waitKey();
	goto jumpAdaptiveThreshold;
	//morphologyEx(sudokuImage, sudokuImage, MORPH_CLOSE, rectKernel, Point(-1, -1), 1);

jumpThreshold:
	//threshold(sudokuImage, sudokuImage, 200, 255, THRESH_BINARY);
	//threshold(sudokuImage, sudokuImage, 200, 255, THRESH_OTSU+THRESH_BINARY); //���� ����ȭ 

	adaptiveThreshold(sudokuImage, sudokuImage, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 21, 1);
	//morphologyEx(sudokuImage, sudokuImage, MORPH_GRADIENT, crossKernel, Point(-1, -1), 1);
	erode(sudokuImage, sudokuImage, crossKernel, Point(-1, -1), 1);
	dilate(sudokuImage, sudokuImage, elipseKernel, Point(-1, -1), 1);
jumpAdaptiveThreshold:



	imshow("sudokuImage", sudokuImage);
	waitKey();

	Mat testImage = realImage.clone();//������ �Ȱ͸� �����Ŵ
	Mat resultImage(testImage.size(), CV_8UC3);
	Mat testSudoImage(realImage.size(), CV_8UC3);
	vector<int> rectSizeStorage; //��ĭ�� ����ĭ ����
	vector<int> realRect; //��ĭ����ĭ�� �ִ� �ּҰ��� �˱����� ����
	vector<Rect> sudokuRect; //ĭ���� �߶��� ���� ����Ȱ�
	vector<Rect> temp;
	threshold(testImage, testImage, 200, 255, THRESH_OTSU + THRESH_BINARY);
	//waitKey();
	findContours(sudokuImage, AfterContoursforSmallRectangle, hierarchy2, mode2, method2);
	//������ ������ �ڸ������� �ٽ� ��輱 �׸�
	for (int k = 0; k < AfterContoursforSmallRectangle.size(); k++)
	{
		cutSize2.push_back(AfterContoursforSmallRectangle[k]); //�� ��輱��ġ���� ����
	}



	for (int k = 0; k < cutSize2.size(); k++)
	{
		//
		cvtColor(testImage, resultImage, COLOR_GRAY2BGR);
		r = boundingRect(cutSize2[k]);//���� �簢������ ����
		Rect ROI2(r.x, r.y, r.width, r.height);
		drawContours(resultImage, cutSize2, k, Scalar(255, 0, 0), 4); //��輱 �׸�
		//cout << r.size() << endl;

		rectangle(resultImage, ROI2, Scalar(128, 255, 255), 2);//�׸�� ũ��

		if ((r.width*r.height)>(resultImage.rows*resultImage.cols) - 20000)
		{		//�ִ뿵���� ���� ����
			//imshow("resultImage", resultImage);
			//waitKey();
			goto here;

		}
		rectSizeStorage.push_back(r.width*r.height); //sorting�ϱ� ���� ����
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
			eraseSpot = k; //500�� ���̳��� ���� ������ ���� 
		}
	}

	for (int k = eraseSpot + 1; k < rectSizeStorage.size(); k++)
	{
		if (eraseSpot == -1)
			realRect.push_back(rectSizeStorage[again++]); //�׳� ���� ����
		else
			realRect.push_back(rectSizeStorage[k]); //81�� �׸� ������ �������
	}


	//������ �̹� �Ǽ� ���Ƿ� �� �ʿ� ����
	int minRectSize = realRect.front(); //�ּ� �׸� ������
	int maxRectSize = realRect[realRect.size() - 1]; //�ִ� �׸� ������

	for (int k = 0; k < cutSize2.size(); k++)
	{

		cvtColor(testImage, resultImage, COLOR_GRAY2BGR);
		//imshow("testImage", testImage);
		r = boundingRect(cutSize2[k]);
		Rect ROI2(r.x, r.y, r.width, r.height);
		//cout << r.size() << endl;

		rectangle(resultImage, ROI2, Scalar(128, 255, 255), 2);//�׸�

		if ((minRectSize <= (r.width*r.height)) && ((r.width*r.height) <= maxRectSize))
			temp.push_back(ROI2); //�ش� ������ �� �ϱ� ���� �־���
		//	imshow("resultImage2", resultImage);
		//waitKey();
	}

	for (int k = temp.size() - 1; k >= 0; k--)
		sudokuRect.push_back(temp[k]); //�ݴ�� �����ϹǷ� ó��1ĭ���� ���ϱ����� ������ ����

	temp.clear();



	vector<int> testSortingOk;
	//�� ���� ������� ������ �ȵ������Ƿ� �� �ٸ��� ���ʺ��� ���������� �ǰ� �ٲ�
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
		rectangle(resultImage, sudokuRect[k], Scalar(128, 255, 255), 2);//�׸�
		imshow("testSudoImage34", resultImage);
		waitKey();
	}

		const char *path = "/tessdate";
	TessBaseAPI tess; //tesseract ����

		//TessBaseAPI *api = new TessBaseAPI();

	tess.Init(NULL, "eng", tesseract::OEM_DEFAULT); //�ѱ���� �ν� �⺻ ���� �Ǿ�����
	/*
		if (api->Init(NULL, "eng"))
		{
			fprintf(stderr, "could not initailze teseract\n");
			exit(1);
		}
		*/

	string tessRecogNum[81][1]; //���ڷ� �ν��ؿ��Ƿ� ������ �������
	int sudoTestte[81]; //������ ��ȣ �������� �������
	int d = 0;

	for (int i = 0; i<sudokuRect.size(); i++)
	{
		cvtColor(testSudo, testSudoImage, COLOR_GRAY2BGR);
		rectangle(testSudoImage, sudokuRect[i], Scalar(128, 255, 255), 2);//�׸�

		Mat cutImage = testSudo(sudokuRect[i]);
		//	imshow("cutImage", cutImage);
		Mat tempImage = repeat(cutImage, 1, 1);


		Mat numberImage = tempImage.clone();//������ �Ȱ͸� �����Ŵ
		//imshow("numberImage", numberImage);
		imwrite("numberImage.jpg", numberImage, params);
		Mat resultNImage(tempImage.size(), CV_8UC3);
		waitKey();

		int height = tempImage.size().height;
		int width = tempImage.size().width;

		int continueNum = 0; //0�� ��ӵǴ� �� ����
		int testPixel = (height*width); //�ִ� �ȼ� �޾ƿ�

		//�� ĭ�� ��ĭ�� ��� ��� 0�� ���� 
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

	tess.SetImage((uchar*)tempImage.data, tempImage.size().width, tempImage.size().height, tempImage.channels(), tempImage.step1()); //�̹����� ������ ���� ���̿� ä���� �޾ƿ�

		//Pix *image = pixRead("numberImage.jpg");
		//api->SetImage(image);
//
		tess.Recognize(0); //�ν� ��Ŵ
		const char * out = tess.GetUTF8Text(); //�װ� utf8text�� �νĽ�Ŵ
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

