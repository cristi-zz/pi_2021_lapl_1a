// OpenCVApplication.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "common.h"
#include <vector>


void testOpenImage()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat src;
		src = imread(fname);
		imshow("opened image",src);
		waitKey();
	}
}

void testOpenImagesFld()
{
	char folderName[MAX_PATH];
	if (openFolderDlg(folderName)==0)
		return;
	char fname[MAX_PATH];
	FileGetter fg(folderName,"bmp");
	while(fg.getNextAbsFile(fname))
	{
		Mat src;
		src = imread(fname);
		imshow(fg.getFoundFileName(),src);
		if (waitKey()==27) //ESC pressed
			break;
	}
}

void testColor2Gray()
{
	char fname[MAX_PATH];
	while(openFileDlg(fname))
	{
		Mat_<Vec3b> src = imread(fname, IMREAD_COLOR);

		int height = src.rows;
		int width = src.cols;

		Mat_<uchar> dst(height, width);

		for (int i=0; i<height; i++)
		{
			for (int j=0; j<width; j++)
			{
				Vec3b v3 = src(i,j);
				uchar b = v3[0];
				uchar g = v3[1];
				uchar r = v3[2];
				dst(i,j) = (r+g+b)/3;
			}
		}
		
		imshow("original image",src);
		imshow("gray image",dst);
		waitKey();
	}
}


int main()
{
	
	Mat_<Vec3b> src1 = imread("images/apple.bmp", IMREAD_COLOR);
	Mat_<Vec3b> src2 = imread("images/orange.bmp", IMREAD_COLOR);

	Mat_<Vec3b> g1 = src1.clone();
	Mat_<Vec3b> g2 = src2.clone();


	std::vector<Mat_<Vec3b>> gpA;
	gpA.push_back(g1);
	for (int i = 0; i < 6; i++)
	{
		pyrDown(gpA[i], g1);
		gpA.push_back(g1);
	}

	std::vector<Mat_<Vec3b>> gpB;
	gpB.push_back(g2);
	for (int i = 0; i < 6; i++)
	{
		pyrDown(gpB[i], g2);
		gpB.push_back(g2);
	}

	
	/* for (int i = 0; i < 6; i++)
	 {
		 imshow("gpA" + std::to_string(i), gpA[i]);
	 }*/
	 

	std::vector<Mat_<Vec3b>> lpA;
	lpA.push_back(gpA[5]);
	for (int i = 5; i > 0; i--)
	{
		Mat_<Vec3b> ge;
		Mat_<Vec3b> l;
		Size size = Size(gpA[i - 1].cols, gpA[i - 1].rows);
		pyrUp(gpA[i], ge, size);
		subtract(gpA[i - 1], ge, l);
		lpA.push_back(l);
	}


	std::vector<Mat_<Vec3b>> lpB;
	lpB.push_back(gpB[5]);
	for (int i = 5; i > 0; i--)
	{
		Mat_<Vec3b> ge;
		Mat_<Vec3b> l;
		Size size = Size(gpB[i - 1].cols, gpB[i - 1].rows);
		pyrUp(gpB[i], ge, size);
		subtract(gpB[i - 1], ge, l);
		lpB.push_back(l);
	}

	std::vector<Mat_<Vec3b>> LS;

	for (int i = 0; i < 6; i++)
	{
		Mat_<Vec3b> ls(lpA[i].rows, lpA[i].cols);
		for (int j = 0; j < lpA[i].rows; j++)
		{
			for (int k = 0; k < lpA[i].cols; k++)
			{
				if (k < (lpA[i].cols / 2))
				{
					ls(j, k) = lpA[i](j, k);
				}
				else
				{
					ls(j, k) = lpB[i](j, k);
				}
			}
		}
		LS.push_back(ls);
	}

	Mat_<Vec3b> ls_ = LS[0];
	for (int i = 1; i < 6; i++)
	{
		Size size = Size(LS[i].cols, LS[i].rows);
		pyrUp(ls_, ls_, size);
		add(ls_, LS[i], ls_);
	}

	imshow("Pyramid blending", ls_);

	Mat_<Vec3b> real(src1.rows, src1.cols);
	for (int j = 0; j < src1.rows; j++)
	{
		for (int k = 0; k < src1.cols; k++)
		{
			if (k < (src1.cols / 2))
			{
				real(j, k) = src1(j, k);
			}
			else
			{
				real(j, k) = src2(j, k);
			}
		}
	}

	imshow("Direct connection", real);

	/*for (int i = 0; i < 6; i++)	{
		imshow("lpA" + std::to_string(i), lpA[i]);
	}*/

	waitKey();
	return 0;
}
