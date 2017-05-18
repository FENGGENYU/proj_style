/* use the ideal of CNN to calculator the images' feature*/

#include <direct.h>
#include<iostream>
#include <time.h>
#include <fstream>
#include<omp.h>
#include<vector>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

IplImage* jiabian(string *filename, int zd)
{
	//Mat img = imread(*filename);
	IplImage* image = cvLoadImage( filename->c_str(), 0 ); 
	
	int flag=0;	
	Mat img(image,0); ; 
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

	ifstream ReadFile;
	int n=0;
	char line[512];
	string temp;
	ReadFile.open(filename,ios::in);
	if(ReadFile.fail())
	{
	
		return 0;
	}else
	{
	
		while(getline(ReadFile,temp)){
		
			n++;
		}
		return n;
	}
	ReadFile.close();
}
typedef vector<float>* Vrq;
int level_main(int clan,string *curpath){

	const int scalenum = 1;
	
	string imagePath=*curpath+"\\BMPimages\\";
	imagePath=imagePath+std::to_string(long double(clan))+"\\";
	string patchPath = *curpath+"\\BMPK\\";	
	       patchPath=patchPath+std::to_string(long double(clan))+"\\ceteridf\\";
	//patchPath=patchPath+"\\ceteridf\\";
		   string imagenameFile = *curpath + "\\imagesname\\picname";
	imagenameFile=imagenameFile+std::to_string(long double(clan))+".txt";
	int imagenum = CL(imagenameFile.c_str());///
	string patchnameFile=patchPath+"Apidfpatchname.txt";
	int patchnum=CL(patchnameFile.c_str());
	string patchscaleFile = *curpath + "\\nscale.txt";
	ifstream finscaleshog(patchscaleFile);
	int size_hog;
    finscaleshog >> size_hog;
	finscaleshog.close();
	HOGDescriptor *desc=new HOGDescriptor(cvSize(size_hog,size_hog),cvSize(16,16),cvSize(8,8),cvSize(8,8),9);

	ifstream finimagename(imagenameFile);
	vector<string>allimagename(imagenum);
	for(int i_0=0;i_0<imagenum;i_0++){	

		string imagename;
		getline(finimagename,imagename);
		allimagename[i_0]=imagename;
	}
	finimagename.close();



	
	for(int i=0;i<imagenum;i++){	

		string imagefile=imagePath+allimagename[i];		
		//IplImage* image = cvLoadImage( imagefile.c_str(), 0 );  
		IplImage* image = jiabian(&imagefile, size_hog);
	
		  


		Vrq **VM[scalenum]; 
		int scaleorder[scalenum];

		clock_t t1 = clock();

		ifstream finscales(patchscaleFile);			
		for(int iScale=0;iScale<scalenum;iScale++){

			int sc;
			finscales>>sc;
			scaleorder[iScale]=sc;
			CvSize image_patch= cvSize(sc,sc);
			CvSize imagetempSize=cvSize(size_hog,size_hog);
			int rowsS=image->height-sc+1;
			int colsS=image->width-sc+1;
			VM[iScale]=new Vrq*[rowsS];
			for(int irows=0;irows<rowsS;irows++){
				VM[iScale][irows]=new Vrq[colsS];
			}
			printf("hello");
			IplImage* imagetemp = cvCreateImage(image_patch,image->depth,image->nChannels);
			IplImage* czimagetemp = cvCreateImage(imagetempSize, imagetemp->depth, imagetemp->nChannels);  

			for (int m = 0; m < rowsS; m++)
			{
				for (int n = 0; n < colsS; n++)
				{	
					cvSetImageROI(image,cvRect(n,m,image_patch.width, image_patch.height));
					cvCopy(image,imagetemp);
					cvResetImageROI(image);
					cvResize(imagetemp, czimagetemp, CV_INTER_AREA);

					vector<float>*czimagetempHOG = new vector<float>;				  
					desc->compute(czimagetemp,*czimagetempHOG);
					VM[iScale][m][n]=czimagetempHOG;

				}
			}

			cvReleaseImage(&imagetemp);
			cvReleaseImage(&czimagetemp);

			cout<<"第"<<iScale<<"个scale"<<endl;
		}
		finscales.close();



		string imageRept=imagefile.substr(0,imagefile.length()-4);
		ofstream foutimageRept(imageRept+".txt");
		ifstream finpatchname(patchnameFile);
		for(int j=0;j<patchnum;j++){

			string patchname;
			getline(finpatchname,patchname);
			string patchfile=patchPath+patchname;		    
			IplImage* patch = cvLoadImage( patchfile.c_str(), 0 );





			CvSize czpatchSize=cvSize(size_hog,size_hog);		    
			IplImage* czpatch = cvCreateImage(czpatchSize, patch->depth, patch->nChannels);  
			cvResize(patch, czpatch, CV_INTER_AREA); 
			vector<float>czpathHOG;
			desc->compute(czpatch,czpathHOG);

			int rows=image->height-patch->height+1;
			int cols=image->width-patch->width+1;  

			float **ConV=new float*[rows];
			for(int k=0;k<rows;k++)
				ConV[k]=new float[cols];

			int pos=-1;
			for(int posi=0;posi<scalenum;posi++){

				if(patch->height==scaleorder[posi]){

					pos=posi;
					break;
				}


			}



			for (int mm = 0; mm < rows; mm++){
				for (int nn = 0; nn < cols; nn++){			                        

					float vals=0.0;
					for(vector<float>::size_type iHOG=0;iHOG<czpathHOG.size();iHOG++){

						vals+=czpathHOG[iHOG]*(*VM[pos][mm][nn])[iHOG];
					}

					ConV[mm][nn] = vals;
				}
			}

			cvReleaseImage(&czpatch);





			float pool[5]={0};
			int countpools=0;
			for(int ii=0;ii<2;ii++){

				for(int jj=0;jj<2;jj++)
				{
					countpools++;
					for(int iii=ii*rows/2;iii<(ii+1)*rows/2;iii++){

						for(int jjj=jj*cols/2;jjj<(jj+1)*cols/2;jjj++){

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

				foutimageRept << pool[i] ;

				foutimageRept << endl;
			}

			cout<<"第"<<j<<"个patch"<<endl;
              cvReleaseImage(&patch);

		}


		foutimageRept.close();
		finpatchname.close();
		cout<<"第"<<i<<"个图片"<<endl;

		ifstream finscales1(patchscaleFile);
		for(int iScale1=0;iScale1<scalenum;iScale1++)
		{

			int sc1;
			finscales1>>sc1;					
			int rows1=image->height-sc1+1;
			int cols1=image->width-sc1+1;

			for(int iirows=0;iirows<rows1;iirows++)
			{
				for(int c = 0; c < cols1; c++)
				{
					delete VM[iScale1][iirows][c];
				}

				delete []VM[iScale1][iirows];
			}
			delete []VM[iScale1];

	}

		finscales1.close();
		clock_t t2 = clock();
		std::cout<<"time: "<<t2-t1<<std::endl;

		cvReleaseImage(&image);
	}
	delete desc;
	return 0;
}
	
void main(){
	string curpath = "J:\\runwithonebutton\\randpatch\\models\\24";
	string pathviews = curpath + "\\views.txt";
	ifstream finviews(pathviews);
	int views;
	finviews >> views;
	finviews.close();
	for (int i = 1; i <= views; i++)
		level_main(i,&curpath);
}

