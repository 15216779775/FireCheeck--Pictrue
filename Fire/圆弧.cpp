#include <opencv2/opencv.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
using namespace cv;
using namespace std;
#define  _ShowImage 1
////////////////////////////////////////////////////////
/**
* @name  getAllImgsFilePath
* @brief ��ȡָ��·���µ������ļ�ȫ·��(����������㰡.)
* @param[in] szDir:ָ��·��(����C:\\_Sunyard\\Project\\�㽭ũ��\\data\\��Ƭ\\Merge���\\*.*";)
* @param[in/out]  testImgFiles,�����ļ�ȫ·����������
* @return 0 �ɹ� / -1 ʧ��
* @author [lintao 2015-10-09]
**/
////////////////////////////////////////////////////////
int getAllImgsFilePath(char *szDir, vector<string>& testImgFiles)
{
	WIN32_FIND_DATA ffd;
	HANDLE hFind = FindFirstFile((szDir), &ffd);
	//testImgFiles.push_back(ffd.cFileName);//do ������..��push��ȥ��
	// List all the files in the directory with some info about them.
	do
	{
		if (ffd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			//_tprintf(TEXT("  %s   <DIR>\n"), ffd.cFileName);
			//cout<<ffd.cFileName<<endl;
		}
		else
		{
			//_tprintf(TEXT("  %s  \n"), ffd.cFileName);
			//cout<<ffd.cFileName<<endl;
			string searchImgDirWithType = szDir;                         //����Ŀ¼���������ļ����͵�
			int npos = searchImgDirWithType.find_last_of("\\");           //
			string searchImgDir = searchImgDirWithType.substr(0, npos + 1); //��Ŀ¼    //substr ����pos��ʼ��n���ַ���ɵ��ַ���
			string imgWholePath = searchImgDir + ffd.cFileName;
			testImgFiles.push_back(imgWholePath);
		}
	} while (FindNextFile(hFind, &ffd) != 0);
	FindClose(hFind);
	return 0;
}
//////////////////////////////////////////////////////////////////////////
//�ж������ǲ���һ��Բ��ͨ�����������pi*r*r�Ĳ��죬�ж��ǲ���Բ��
//contour����������
//minRadius: �뾶�������ٴ���minRadius
//////////////////////////////////////////////////////////////////////////
bool isCircleContour(vector<Point>& contour, double minRadius)
{
	bool isCircle = false;
	//double perimeter = arcLength(contour,TRUE);
	double perimeter = arcLength(contour, false);
	double perimeter2 = arcLength(contour, true);
	Rect rect = boundingRect(contour);
	double r = (rect.width + rect.height) / 4.0;	//�뾶Լ����������Ӿصı߳�/2
	double area = contourArea(contour);
	double pi = 3.1415926;
	//cout<<"x/y/width/height\t"<<rect.x<<"  "<<rect.y<<"  "<<rect.width<<"  "<<rect.height<<endl;
	//cout<<"perimeter:FALSE\t"<<perimeter<<endl;
	//cout<<"perimeter2:TRUE\t"<<perimeter2<<endl;
	//cout<<"area:\t"<<area<<endl;
	//cout<<"r: "<<r<<"  ������ܳ�: "<<2*pi*r<<"  ��������: "<<pi*r*r<<endl;
	double perimeterDiffRatio = fabs(2 * pi*r - perimeter)*2.0 / (2 * pi*r + perimeter) * 100;
	double areaDiffRatio = fabs(pi*r*r - area)*2.0 / (pi*r*r + area) * 100;
	double sideDiffRatio = abs(rect.width - rect.height)*2.0 / (rect.width + rect.height) * 100;
	//CString CstrInfo3;
	//CstrInfo3.Format(_T(" perimeterDiffRatio: %f\n areaDiffRatio: %f \n  sideDiffRatio:%f"),perimeterDiffRatio,areaDiffRatio,sideDiffRatio);
	//AfxMessageBox(CstrInfo3);
	if (r>minRadius)
	{
		cout << "�ܳ�����ٷֱ�:  " << perimeterDiffRatio << "%" << endl;;
		cout << "�������ٷֱ�:  " << areaDiffRatio << "%" << endl;
		cout << "�߳�����ٷֱ�:  " << sideDiffRatio << "%" << endl;;
		cout << endl << endl;
	}
	//ԭʼ��
	//const double areaDiffThresh = 18.0;       //Ŀǰͳ�Ƶģ����������2.58%
	//const double perimeterDiffThresh = 15;    //Ŀǰͳ�Ƶģ������ܳ���5.65%
	//const double sideDiffThresh = 15;     //Ŀǰͳ�Ƶģ����ı߳���7.3%
	//const double avgDiffThresh = 15.0;        //���������ֵ�ļ�Ȩƽ��Ҫ��С��5.0;
	const double areaDiffThresh = 5;        //Ŀǰͳ�Ƶģ����������2.58%
	const double perimeterDiffThresh = 7.25; //Ŀǰͳ�Ƶģ������ܳ���5.65%
	const double sideDiffThresh = 10;       //Ŀǰͳ�Ƶģ����ı߳���7.3%
	const double avgDiffThresh = 5.0;       //���������ֵ�ļ�Ȩƽ��Ҫ��С��5.0;
	double avdDiffRatio = (areaDiffRatio + perimeterDiffRatio + sideDiffRatio) / 3.0;
	//������������ܳ��ǰ뾶��2*pi����
	if ((areaDiffRatio <areaDiffThresh)
		&& (perimeterDiffRatio <perimeterDiffThresh)
		&& (sideDiffRatio <sideDiffThresh)
		&& (avdDiffRatio<avgDiffThresh)
		&& (r>minRadius))
	{
		isCircle = true;
	}
	return isCircle;
}
int countShootingRingsNum(Mat& orgImg)
{
	if (orgImg.empty())
		return -1;
	//ת�����Ҷ�ͼ
	Mat grayImage;
	if (orgImg.channels() == 3)
		cvtColor(orgImg, grayImage, CV_BGR2GRAY);
	if (orgImg.channels() == 1)
		grayImage = orgImg.clone();
	imshow("grayImage", grayImage);
	//ģ�飺��ֵ��
	int isBlackCharacter = false;
	Mat binImg;
	double otsuThresh = 0.0;
	if (isBlackCharacter)
		otsuThresh = threshold(grayImage, binImg, 0, 255, CV_THRESH_BINARY_INV | CV_THRESH_OTSU);
	else
		otsuThresh = threshold(grayImage, binImg, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
	//CString CstrInfo3;
	//CstrInfo3.Format(_T(" otsuThresh: %f"),otsuThresh);
	//AfxMessageBox(CstrInfo3);
	imshow("binImg", binImg);
	//��ʴ����
	int erodeSize = 3;
	int dilateSize = 3;
	Mat elementForErode = getStructuringElement(MORPH_RECT, Size(erodeSize, erodeSize), Point(-1, -1));
	Mat elementForDilate = getStructuringElement(MORPH_RECT, Size(dilateSize, dilateSize), Point(-1, -1));
	dilate(binImg, binImg, elementForDilate);
	//erode(binImg ,binImg , elementForErode );
	imshow("���͸�ʴ���Binary Image", binImg);
	//waitKey();
	//ģ�飺��ȡ����(���������������_tooLessPixels)
	vector< vector<Point> > contours;   // ����
	vector< vector<Point> > filterContours; // ɸѡ�������
	vector< Vec4i > hierarchy;    // �����Ľṹ��Ϣ
	contours.clear();
	hierarchy.clear();
	filterContours.clear();
	Mat binaryImageCopy1 = binImg.clone();
	//findContours(binaryImageCopy1, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
	findContours(binaryImageCopy1, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	// ȥ��α����: ���������ص���̫�ٵ�
	const int _tooLessPixels = 10 * 10;
	for (size_t i = 0; i < contours.size(); i++)
	{
		if (fabs(contourArea(contours[i])) > _tooLessPixels)
		{
			//�ȶ�push��ȥ
			filterContours.push_back(contours[i]);
		}
	}
	Mat rgbImg = binImg.clone();
	cvtColor(rgbImg, rgbImg, CV_GRAY2BGR);
	//binaryImage.setTo(0);
	drawContours(rgbImg, filterContours, -1, Scalar(0, 255, 0), 5); //8, hierarchy);
																	//cvNamedWindow("filterContours",0);
	imshow("filterContours", rgbImg);
	//ģ�飺��ӾصĿ�߱ȱ���ʧ���Ĺ��˵�
	const double w2hThresh2 = 2.0;      //��ӾصĿ�߱ȱ�������3���˵�
	const double h2wThresh2 = 2.0;      //��ӾصĿ�߱ȱ�������3���˵�
	for (int i = 0; i<filterContours.size(); i++)
	{
		Rect tmpRect = boundingRect(filterContours[i]);
		if ( //(tmpRect.width>maxRectWidth)
			 // (tmpRect.height>maxRectHeight)
			((double)tmpRect.width / (double)tmpRect.height > w2hThresh2)
			|| ((double)tmpRect.height / (double)tmpRect.width > h2wThresh2)
			)
		{
			filterContours.erase(filterContours.begin() + i);
			i--;//ɾ����һ������������һ��
		}
	}
	//����Բ�Ĺ��˵�
	double minRadius = 20 / 2;            //Ŀǰ��С��Բֱ��30
	for (int i = 0; i<filterContours.size(); i++)
	{
		if (!isCircleContour(filterContours[i], minRadius))
		{
			//selectContours.push_back(filterContours[i]);
			filterContours.erase(filterContours.begin() + i);
			i--;//ɾ����һ������������һ��
		}
	}
	Mat rgbImg2 = binImg.clone();
	cvtColor(rgbImg2, rgbImg2, CV_GRAY2BGR);
	//binaryImage.setTo(0);
	drawContours(rgbImg2, filterContours, -1, Scalar(0, 255, 0), 5); //8, hierarchy);
																	 //cvNamedWindow("filterContours2",0);
	imshow("filterContours2", rgbImg2);
	Mat grayImg;
	cvtColor(orgImg, grayImg, CV_BGR2GRAY);
	Mat diffImg;
	Mat greenImg;
	absdiff(grayImg, greenImg, diffImg);
	normalize(diffImg, diffImg, 0, 255, NORM_MINMAX);
	imshow("��ɫͨ���ͻҶ�ͼ����ͼͼ", diffImg);
	return 0;
}
int testCountShootingRingsNum()
{
	//char *szDir = "C:\\Projects\\ComputerVisionProject\\���Ͽ�������Ŀ\\����еĻ���\\data\\webData\\*.*";
	char *szDir = "C:\\Projects\\ComputerVisionProject\\���Ͽ�������Ŀ\\����еĻ���\\data\\*.*";
	vector<string> testImgFiles;
	getAllImgsFilePath(szDir, testImgFiles);
	//system("time");
	for (int i = 0; i<testImgFiles.size(); i++)
	{
		//����ļ���
		int npos = testImgFiles[i].find_last_of("\\");
		string fileName = testImgFiles[i].substr(npos + 1, testImgFiles[i].length() - (npos + 1));  //��ȡĿ¼
		cout << fileName << endl;
		Mat orgImg = imread(testImgFiles[i]);
		if (orgImg.empty())
			continue;
		imshow("orgImg", orgImg);
		countShootingRingsNum(orgImg);
		waitKey();
	}
	system("time");
	return 0;
}
void main1()
{
	testCountShootingRingsNum();
}

