/* use kernel to convolute the bounding box of all images*/

#include <direct.h>
#include<iostream>
#include <time.h>
#include <fstream>
#include<omp.h>
#include<vector>
#include <opencv2/opencv.hpp>
#include <math.h>
using namespace cv;
using namespace std;

int view_begin=0;
int view_end=0;
string curpath;
string user_name;
string user_name_File;
string order_data;

IplImage* jiabian(IplImage* image, int zd)
{
	//Mat img = imread(*filename);
	//IplImage* image = cvLoadImage( filename->c_str(), 0 ); 
	
	int flag=0;	
	Mat img(image,0);
	Mat imgtemp; 
	int rows = img.rows;
	int cols = img.cols;
	

	if(image->nChannels==1){

		if (rows < zd)
		{
			copyMakeBorder(img, imgtemp, 0, zd - rows, 0, 0, BORDER_CONSTANT, Scalar(255));
			CvSize imgsize=cvSize(imgtemp.cols,imgtemp.rows);
			IplImage* imgtemp_1=cvCreateImage(imgsize, image->depth, image->nChannels); 
			cvCopy(&IplImage(imgtemp),imgtemp_1);
			cvReleaseImage(&image);
			return  imgtemp_1;
		}
		if (cols < zd)
		{
			copyMakeBorder(img, imgtemp, 0, 0, 0, zd-cols, BORDER_CONSTANT, Scalar(255));
			CvSize imgsize=cvSize(imgtemp.cols,imgtemp.rows);
			IplImage* imgtemp_1=cvCreateImage(imgsize, image->depth, image->nChannels); 
			cvCopy(&IplImage(imgtemp),imgtemp_1);
			cvReleaseImage(&image);
			return  imgtemp_1;
		}
	}
	else{
		if (rows < zd)
		{
			copyMakeBorder(img, imgtemp, 0, zd - rows, 0, 0, BORDER_CONSTANT, Scalar(255,255,255));
			CvSize imgsize=cvSize(imgtemp.cols,imgtemp.rows);
			IplImage* imgtemp_1=cvCreateImage(imgsize, image->depth, image->nChannels); 
			cvCopy(&IplImage(imgtemp),imgtemp_1);
			cvReleaseImage(&image);
			return  imgtemp_1;
		}
		if (cols < zd)
		{
			copyMakeBorder(img, imgtemp, 0, 0, 0, zd-cols, BORDER_CONSTANT,  Scalar(255,255,255));
			CvSize imgsize=cvSize(imgtemp.cols,imgtemp.rows);
			IplImage* imgtemp_1=cvCreateImage(imgsize, image->depth, image->nChannels); 
			cvCopy(&IplImage(imgtemp),imgtemp_1);
			cvReleaseImage(&image);
			return  imgtemp_1;
		}

	}

	return image;
    
}
	


int CL(const char *filename){
	cout <<"CL"<<endl;
	ifstream ReadFile;
	int n=0;
	string temp;
	ReadFile.open(filename,ios::in);
	if(ReadFile.fail())
	{
	    ReadFile.close();
		return 0;
	}else
	{
	
		while(getline(ReadFile,temp)){
		
			n++;
		}
		ReadFile.close();
		return n;
	}
}
typedef vector<float>* Vrq;
int level_main(int clan,string *curpath){

	string imagePath = *curpath + "\\3dlines\\";

	string imagenameFile = *curpath + "\\imagesname\\picname";
	imagenameFile=imagenameFile+std::to_string(long double(clan))+".txt";
	int imagenum = CL(imagenameFile.c_str());

	string patchPath = *curpath+"\\kernel\\"+std::to_string((long double)clan)+"\\";
	string patchnameFile=patchPath+"Apidfpatchname.txt";
	int patchnum=CL(patchnameFile.c_str());

	int size_hog=48;

	HOGDescriptor *desc=new HOGDescriptor(cvSize(size_hog,size_hog),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);

	int step=1;

	ifstream finimagename(imagenameFile);
	vector<string>allimagename(imagenum);
	for(int i_0=0;i_0<imagenum;i_0++){	

		string imagename;
		getline(finimagename,imagename);
		allimagename[i_0]=imagename;
	}
	finimagename.close();
	
	
	for(int i=0;i<imagenum;i++){	

		clock_t t1 = clock();

		string imagefile=imagePath+allimagename[i];		
		IplImage* image_original = cvLoadImage( imagefile.c_str(), 0 ); 
		cout<<imagefile<<endl;
        //calculate hog for all areas
		
		CvSize image_patch= cvSize(size_hog,size_hog);
		IplImage* image = jiabian(image_original, size_hog);

		int rows=image->height-size_hog+1;
		int cols=image->width-size_hog+1;

		Vrq **VM=new Vrq*[rows];

		for(int irows=0;irows<rows;irows++){
			VM[irows]=new Vrq[cols];
		}
		
		IplImage* imagetemp = cvCreateImage(image_patch,image->depth,image->nChannels);
		for (int m = 0; m < rows; m+=step)
		{
			for (int n = 0; n < cols; n+=step)
			{	
				cvSetImageROI(image,cvRect(n,m,image_patch.width, image_patch.height));
				cvCopy(image,imagetemp);
				cvResetImageROI(image);
					
				vector<float>*czimagetempHOG = new vector<float>;				  
				desc->compute(imagetemp,*czimagetempHOG);
				VM[m][n]=czimagetempHOG;

			}
		}

		cvReleaseImage(&image);
		cvReleaseImage(&imagetemp);
		cout<<"done"<<endl;

		string imageRept=imagefile.substr(0,imagefile.length()-4);
		ofstream foutimageRept(imageRept+".txt");
		ifstream finpatchname(patchnameFile);
		for(int j=0;j<patchnum;j++){

			string patchname;
			getline(finpatchname,patchname);
			string patchfile=patchPath+patchname;		    
			IplImage* patch = cvLoadImage( patchfile.c_str(), 0 );

			vector<float>czpathHOG;
			desc->compute(patch,czpathHOG);

			float **ConV=new float*[rows];
			for(int k=0;k<rows;k++)
				ConV[k]=new float[cols]();

			for (int mm = 0; mm < rows; mm+=step){
				for (int nn = 0; nn < cols; nn+=step){			                        

					float vals=0.0;
					for(vector<float>::size_type iHOG=0;iHOG<czpathHOG.size();iHOG++){

						vals+=czpathHOG[iHOG]*(*VM[mm][nn])[iHOG];
					}

					ConV[mm][nn] = vals;
				}
			}

			float pool[5]={0};
			int countpools=0;
			
			int fixrows= rows - rows%4;
			int fixcols= cols - cols%4;
			
			for(int ii=0;ii<2;ii++){

				for(int jj=0;jj<2;jj++)
				{
					countpools++;
					
					for(int iii=ii*ceil((float)fixrows/2);iii<(ii+1)*ceil((float)fixrows/2);iii+=step){

						for(int jjj=jj*ceil((float)fixcols/2);jjj<(jj+1)*ceil((float)fixcols/2);jjj+=step){

							if(pool[countpools]<ConV[iii][jjj])
								pool[countpools]=ConV[iii][jjj];

						}
					}
					
				}
			}

			for(int knima=0;knima<rows;knima++)
				delete []ConV[knima];
			delete []ConV;

			for(int iiii=1;iiii<5;iiii++){
				if(pool[0]<pool[iiii])
					pool[0]=pool[iiii];
			}
			
			for (int i = 0; i < 5; i++){		                            
				foutimageRept << pool[i];
				foutimageRept << endl;
			}

			cout<<"The "<<j<<" patch"<<endl;
            cvReleaseImage(&patch);
		}


		foutimageRept.close();
		finpatchname.close();

		cout<<"The "<<i<<" image"<<endl;

		for(int iirows=0;iirows<rows;iirows+=step)
		{
			for(int c = 0; c < cols; c+=step)
			{
				delete VM[iirows][c];
			}

			delete []VM[iirows];
		}
		if(step==2)
		{
			for(int iirows=1;iirows<rows;iirows+=step)
			{
				delete []VM[iirows];
			}
		}
		delete VM;

		clock_t t2 = clock();
		std::cout<<"time: "<<t2-t1<<std::endl;

		
	}
	delete desc;
	return 0;
}

void initialize(string params)
{
	//initialize parameters
	if (params.empty())
	{
		cout << "please input the parameters file" << endl;
		cin.get();
		exit(0);
	}

	ifstream ifs;
	ifs.open(params);
	if (ifs.fail())
	{
		cout << "can't open parameters file" << endl;
	}
	string tmp;
	ifs >> tmp >> curpath
		>> tmp >> view_begin
		>> tmp >> view_end;

	ifs.close();
}

void main(int argv, char* args[]){
	
	initialize("params.cfg");

	for (int i = view_begin; i <= view_end; i++)
		level_main(i,&curpath);
}

