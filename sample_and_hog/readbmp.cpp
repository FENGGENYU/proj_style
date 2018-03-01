#include "readbmp.h"
#include "math.h"  
#include "stdio.h"
#include "stdlib.h"   
#include "malloc.h"
#include <fstream>
#include <iostream>
using namespace std;

Patch* getpatch(unsigned char *bitmapData, int width, int height)
{
	int ybottom = height;
	int ytop = 0;
	int xleft = width;
	int xright = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i*width + j) * 3;
			if (bitmapData[index] < 255)
			{
				if (i < ybottom)
					ybottom = i;
				if (i > ytop)
					ytop = i;
				if (j < xleft)
					xleft = j;
				if (j > xright)
					xright = j;
			}
		}
	}
	int rybottom = height;
	int rytop = 0;
	int rxleft = width;
	int rxright = 0;
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			int index = (i*width + j) * 3;
			if (bitmapData[index] == 0)
			{
				if (i < rybottom)
					rybottom = i;
				if (i > rytop)
					rytop = i;
				if (j < rxleft)
					rxleft = j;
				if (j > rxright)
					rxright = j;
			}
		}
	}
	Patch *mypatch = new Patch;
	if (rxright == 0)
		mypatch->x1 = -1;
	else
	{
		mypatch->x1 = (double)(rxleft - xleft + 1) / (double)(xright - xleft + 1);
		mypatch->y1 = (double)(rybottom - ybottom + 1) / (double)(ytop - ybottom + 1);
		mypatch->x2 = (double)(rxright - xleft + 1) / (double)(xright - xleft + 1);
		mypatch->y2 = (double)(rytop - ybottom + 1) / (double)(ytop - ybottom + 1);
	}
	return mypatch;
}
BmpImage* imcrop(BmpImage* image, int x, int y, int size)
{
	if (x<1 || y<1 || x + size >image->width || y + size> image->height)
	{
		cout << "imcrop error" << endl;
		exit(0);
	}
	BmpImage* patch = new BmpImage;
	patch->dataOfBmp = new myRGBQUAD[(size + 1)*(size + 1)];
	for (int i = y; i <= y + size; i++)
		for (int j = x; j <= x + size; j++)
			patch->dataOfBmp[(i - y)*(size + 1) + j - x] = image->dataOfBmp[(i - 1)*image->width + j - 1];
	patch->height = size + 1;
	patch->width = size + 1;
	patch->depth = 24;
	return patch;
}
BmpImage* readbmp(string Filename)
{

	// 打开文件
	myBITMAPFILEHEADER  bitHead;
	myBITMAPINFOHEADER bitInfoHead;
	FILE* pfile = fopen(Filename.data(), "rb");
	if (pfile == 0)
	{
		cout << "cannot open the projection file "<<Filename << endl;
		cin.get();
		exit(2);
	}
	WORD fileType;
	fread(&fileType, 1, sizeof(WORD), pfile);
	if (fileType != 0x4d42)
	{
		cout << "这不是bmp格式的文件!";
		return 0;
	}

	fread(&bitHead, sizeof(myBITMAPFILEHEADER), 1, pfile);
	//showBmpHead(&bitHead);
	//cout<<endl<<endl;

	//读取位图信息头信息
	fread(&bitInfoHead, sizeof(myBITMAPINFOHEADER), 1, pfile);
	//showBmpInforHead(&bitInfoHead);
	//cout<<endl;

	tagRGBQUAD2 *pRgb = new tagRGBQUAD2[0];
	long nPlantNum;
	if (bitInfoHead.biBitCount < 24)//有调色板
	{
		//读取调色盘结信息
		nPlantNum = long(pow(2, double(bitInfoHead.biBitCount)));    //   Mix color Plant Number;
		pRgb = new tagRGBQUAD2[nPlantNum*sizeof(tagRGBQUAD2)];
		memset(pRgb, 0, nPlantNum*sizeof(tagRGBQUAD2));
		fread(pRgb, 4, nPlantNum, pfile);

		/*cout<<"Color Plate Number: "<<nPlantNum<<endl;

		cout<<"颜色板信息:"<<endl;
		for (int i =0; i<nPlantNum;i++)
		{
		if (i%5==0)          {cout<<endl;}
		showRgbQuan(&pRgb[i]);
		}*/

		//cout<<endl;

	}
	BmpImage *image = new BmpImage;
	int width = bitInfoHead.biWidth;
	int height = bitInfoHead.biHeight;
	image->width = width;
	image->height = height;
	//分配内存空间把源图存入内存   
	int l_width = WIDTHBYTES(width* bitInfoHead.biBitCount);//计算位图的实际宽度并确保它为32的倍数
	BYTE    *pColorData = new BYTE[height*l_width];
	memset(pColorData, 0, height*l_width);
	long nData = height*l_width;

	//把位图数据信息读到数组里   
	fread(pColorData, 1, nData, pfile);

	//将位图数据转化为RGB数据
	mytagRGBQUAD* dataOfBmp;
	dataOfBmp = new mytagRGBQUAD[width*height*sizeof(mytagRGBQUAD)];//用于保存各像素对应的RGB数据
	memset(dataOfBmp, 0, width*height*sizeof(mytagRGBQUAD));

	if (bitInfoHead.biBitCount < 24)//有调色板，即位图为非真彩色 
	{
		int k;
		int index = 0;
		if (bitInfoHead.biBitCount == 1)
		{
			image->depth = 1;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
				{
					BYTE mixIndex = 0;
					k = i*l_width + j / 8;//k:取得该像素颜色数据在实际数据数组中的序号
					//j:提取当前像素的颜色的具体值    
					mixIndex = pColorData[k];
					switch (j % 8)
					{
					case 0:
						mixIndex = mixIndex;
						break;
					case 1:
						mixIndex = mixIndex >> 1;
						break;
					case 2:
						mixIndex = mixIndex >> 2;
						break;
					case 3:
						mixIndex = mixIndex >> 3;
						break;
					case 4:
						mixIndex = mixIndex >> 4;
						break;
					case 5:
						mixIndex = mixIndex >> 5;
						break;
					case 6:
						mixIndex = mixIndex >> 6;
						break;
					case 7:
						mixIndex = mixIndex >> 7;
						break;
					}

					//将像素数据保存到数组中对应的位置
					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					//dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;
				}
		}

		if (bitInfoHead.biBitCount == 2)
		{
			image->depth = 2;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
				{
					BYTE mixIndex = 0;
					k = i*l_width + j / 4;//k:取得该像素颜色数据在实际数据数组中的序号
					//j:提取当前像素的颜色的具体值    
					mixIndex = pColorData[k];
					switch (j % 4)
					{
					case 0:
						mixIndex = mixIndex;
						break;
					case 1:
						mixIndex = mixIndex >> 2;
						break;
					case 2:
						mixIndex = mixIndex >> 4;
						break;
					case 3:
						mixIndex = mixIndex >> 6;
						break;
					}

					//将像素数据保存到数组中对应的位置
					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					//dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;
				}
		}
		if (bitInfoHead.biBitCount == 4)
		{
			image->depth = 4;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
				{
					BYTE mixIndex = 0;
					k = i*l_width + j / 2;
					mixIndex = pColorData[k];
					if (j % 2 == 0)
					{//低      
						mixIndex = mixIndex;
					}
					else
					{//高
						mixIndex = mixIndex >> 4;
					}

					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					//dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;
				}
		}
		if (bitInfoHead.biBitCount == 8)
		{
			image->depth = 8;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
				{
					BYTE mixIndex = 0;
					k = i*l_width + j;
					mixIndex = pColorData[k];

					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					//dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;
				}
		}
		if (bitInfoHead.biBitCount == 16)
		{
			image->depth = 16;
			for (int i = 0; i < height; i++)
				for (int j = 0; j < width; j++)
				{
					WORD mixIndex = 0;
					k = i*l_width + j * 2;
					WORD shortTemp;
					shortTemp = pColorData[k + 1];
					shortTemp = shortTemp << 8;

					mixIndex = pColorData[k] + shortTemp;

					dataOfBmp[index].rgbRed = pRgb[mixIndex].rgbRed;
					dataOfBmp[index].rgbGreen = pRgb[mixIndex].rgbGreen;
					dataOfBmp[index].rgbBlue = pRgb[mixIndex].rgbBlue;
					//dataOfBmp[index].rgbReserved = pRgb[mixIndex].rgbReserved;
					index++;

				}
		}
	}
	else//位图为24位真彩色
	{
		image->depth = 24;
		int k;
		int index = 0;
		for (int i = 0; i < height; i++)
			for (int j = 0; j < width; j++)
			{
				k = i*l_width + j * 3;
				dataOfBmp[index].rgbRed = pColorData[k + 2];
				dataOfBmp[index].rgbGreen = pColorData[k + 1];
				dataOfBmp[index].rgbBlue = pColorData[k];
				index++;
			}
	}
	//    cout<<"像素数据信息:"<<endl;
	//	for (int i=0; i<width*height; i++)
	//{
	//	if (dataOfBmp[i].rgbRed != 255)
	//	{
	//      showRgbQuan(&dataOfBmp[i]);
	//	  cout<<","<<i/width+1<<","<<i%width+1<<endl;
	//	}
	//}
	//showRgbQuan(&dataOfBmp[139*width+13]);
	//for (int i=0; i<width*height; i++)
	//{
	//   if (i%5==0)      cout<<endl;
	//   if (i%width==0)     cout<<"*";
	//   showRgbQuan(&dataOfBmp[i]);
	//}
	// 初始化GLUT并运行
	//glutInit(&argc, argv);
	//glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//glutInitWindowPosition(100, 100);
	//glutInitWindowSize(width, height);
	//glutCreateWindow("opengl");
	//glutDisplayFunc(&display);
	//glutMainLoop();
	// 释放内存
	// 实际上，glutMainLoop函数永远不会返回，这里也永远不会到达
	// 这里写释放内存只是出于一种个人习惯
	// 不用担心内存无法释放。在程序结束时操作系统会自动回收所有内存
	fclose(pfile);
	image->dataOfBmp = dataOfBmp;
	if (bitInfoHead.biBitCount < 24)
	{
		delete[] pRgb;
	}
	//delete [] dataOfBmp;
	delete[] pColorData;
	return image;
}