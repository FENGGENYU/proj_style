#ifndef _READBMP_H
#define _READBMP_H


#include <gl\glut.h>
#include <string>
using namespace std;
//static GLint    ImageWidth;
//static GLint    ImageHeight;
//static GLint    PixelLength;
//static GLubyte* PixelData;
#define   WIDTHBYTES(bits) (((bits)+31)/32*4)

typedef unsigned char BYTE;
typedef unsigned short WORD;
typedef unsigned long DWORD;
typedef long LONG;

typedef struct mytagBITMAPFILEHEADER
{
DWORD bfSize;               
WORD   bfReserved1; 
WORD   bfReserved2; 
DWORD bfOffBits;         
} myBITMAPFILEHEADER; 

typedef struct mytagBITMAPINFOHEADER{

DWORD   biSize;                  
LONG    biWidth;        
LONG    biHeight;       
WORD    biPlanes;        
WORD    biBitCount;      
DWORD   biCompression;          
DWORD   biSizeImage;            
LONG    biXPelsPerMeter;  
LONG    biYPelsPerMeter;   
DWORD   biClrUsed;                 
DWORD   biClrImportant;             
} myBITMAPINFOHEADER; 


typedef struct mytagRGBQUAD { 

BYTE     rgbBlue; 
BYTE     rgbGreen; 
BYTE     rgbRed; 
//BYTE     rgbReserved; 
} myRGBQUAD;
typedef struct tagRGBQUAD2 { 

BYTE     rgbBlue; 
BYTE     rgbGreen; 
BYTE     rgbRed; 
BYTE     rgbReserved; 
} RGBQUAD2;

typedef struct BmpImageInfo { 

myRGBQUAD* dataOfBmp; 
GLint width;
GLint height;
GLint depth;
} BmpImage;
typedef struct BlackPatchindex { 

GLfloat x1;
GLfloat y1;
GLfloat x2;
GLfloat y2;
} Patch;

BmpImage* readbmp(string Filename);
Patch* getpatch(unsigned char *bitmapData,int width, int height);
BmpImage* imcrop(BmpImage* image,int x, int y,int size);

#endif