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
	int n; // number of steps

	std::cout << "Input number of steps for Gauss and Laplace operations: \n";
	std::cin >> n;
	Mat_<Vec3b> src1 = imread("images/apple.bmp", IMREAD_COLOR);
	Mat_<Vec3b> src2 = imread("images/orange.bmp", IMREAD_COLOR);

	Mat_<Vec3b> g1 = src1.clone();
	Mat_<Vec3b> g2 = src2.clone();


	std::vector<Mat_<Vec3b>> gpA;
	gpA.push_back(g1);
	for (int i = 0; i < n; i++)
	{
		pyrDown(gpA[i], g1);
		gpA.push_back(g1);
	}

	//// Gauss for first image
	//for (int i = 0; i < n; i++)
	//{
	//	imshow("GAUSS - A - " + std::to_string(i), gpA[i]);
	//}

	//// Resizable first image Gauss 0
	//namedWindow("Resizable Gauss A 0", 0);
	//resizeWindow("Resizable Gauss A 0", 740, 530);
	//imshow("Resizable Gauss A 0", gpA[0]);

	std::vector<Mat_<Vec3b>> gpB;
	gpB.push_back(g2);
	for (int i = 0; i < n; i++)
	{
		pyrDown(gpB[i], g2);
		gpB.push_back(g2);
	}

	//// Gauss for second image
	//for (int i = 0; i < n; i++)
	//{
	//	imshow("GAUSS - B - " + std::to_string(i), gpB[i]);
	//}
	 

	std::vector<Mat_<Vec3b>> lpA;
	lpA.push_back(gpA[n - 1]);
	for (int i = n - 1; i > 0; i--)
	{
		Mat_<Vec3b> ge;
		Mat_<Vec3b> l;
		Size size = Size(gpA[i - 1].cols, gpA[i - 1].rows);
		pyrUp(gpA[i], ge, size);
		subtract(gpA[i - 1], ge, l);
		lpA.push_back(l);
	}

	//// Laplace for first image
	//for (int i = 0; i < n; i++)
	//{
	//	imshow("LAPLACE - A - " + std::to_string(i), lpA[i]);
	//}

	// Resizable first image Laplace n - 1
	namedWindow("Resizable Laplace A n - 1", 0);
	resizeWindow("Resizable Laplace A n - 1", 740, 530);
	imshow("Resizable Laplace A n - 1", lpA[n - 1]);

	std::vector<Mat_<Vec3b>> lpB;
	lpB.push_back(gpB[n - 1]);
	for (int i = n - 1; i > 0; i--)
	{
		Mat_<Vec3b> ge;
		Mat_<Vec3b> l;
		Size size = Size(gpB[i - 1].cols, gpB[i - 1].rows);
		pyrUp(gpB[i], ge, size);
		subtract(gpB[i - 1], ge, l);
		lpB.push_back(l);
	}

	//// Laplace for seconds image
	//for (int i = 0; i < n; i++)
	//{
	//	imshow("LAPLACE - B -" + std::to_string(i), lpB[i]);
	//}

	std::vector<Mat_<Vec3b>> LS;

	for (int i = 0; i < n; i++)
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
	for (int i = 1; i < n; i++)
	{
		Size size = Size(LS[i].cols, LS[i].rows);
		pyrUp(ls_, ls_, size);
		add(ls_, LS[i], ls_);
	}

	// Pyramid blending
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

	// Direct connection
	imshow("Direct connection", real);


	// Reconstructing image from Laplace - first image
	std::vector<Mat_<Vec3b>> LS_A;

	for (int i = 0; i < n; i++)
	{
		Mat_<Vec3b> ls_a(lpA[i].rows, lpA[i].cols);
		for (int j = 0; j < lpA[i].rows; j++)
		{
			for (int k = 0; k < lpA[i].cols; k++)
			{
				ls_a(j, k) = lpA[i](j, k);
			}
		}
		LS_A.push_back(ls_a);
	}

	Mat_<Vec3b> reconstructed_first_image = LS_A[0];
	for (int i = 1; i < n; i++)
	{
		Size size = Size(LS_A[i].cols, LS_A[i].rows);
		pyrUp(reconstructed_first_image, reconstructed_first_image, size);
		add(reconstructed_first_image, LS_A[i], reconstructed_first_image);
	}

	// reconstructed image
	imshow("Reconstructed first image", reconstructed_first_image);
	
	float mae_red = 0.0f, mae_green = 0.0f, mae_blue = 0.0f;
	
	for (int i = 0; i < src1.rows; i++)
	{
		for (int j = 0; j < src1.cols; j++)
		{
			mae_blue += abs(src1(i, j)[0] - reconstructed_first_image(i, j)[0]);
			mae_green += abs(src1(i, j)[1] - reconstructed_first_image(i, j)[1]);
			mae_red += abs(src1(i, j)[2] - reconstructed_first_image(i, j)[2]);
		}
	}

	mae_blue /= (src1.rows * src1.cols);
	mae_green /= (src1.rows * src1.cols);
	mae_red /= (src1.rows * src1.cols);

	std::cout << "Red MAE: " << mae_red << '\n';
	std::cout << "Green MAE: " << mae_green << '\n';
	std::cout << "Blue MAE: " << mae_blue << '\n';

	// Added 128 for contrast
	imshow("Difference", (src1 - reconstructed_first_image) + 128);

	waitKey();
	return 0;
}
