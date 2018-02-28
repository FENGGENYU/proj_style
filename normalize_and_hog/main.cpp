#include "FileZ.h"
#include "CommonZ.h"
#include "model.h"
#include "readbmp.h"
#include "windows.h"
#include "direct.h"
#include "time.h"
#include "stdio.h"
#include <iostream>
#include <opencv2/opencv.hpp>
using namespace cv;
using namespace std;

GLint window_size = 200;
GLint patch_size = 0;
GLint black_begin = 0;
GLint model_begin = 0;

GLMmodel *model;
GLfloat radius = 0.0;


int model_current = 0;

int snum = 0;
int view_num = 0;
string projection_path;
string patch_path;
string models_path;
string seed_path;

FileZ fz;

clock_t start;
clock_t finish;
bool seed_exist;

string option;

//Sampling seeds
void read_seed(string filename)
{
	cout << "read seed" << endl;
	FILE *file;
	file = fopen(filename.data(), "r");
	if (!file)
	{
		fprintf(stderr, "setseed failed: can't open file \"%s\".\n", filename);
		system("PAUSE");
		exit(1);
	}
	char st1[30];
	/*
	GLfloat unused[3];
	for (int k = 0; k < model_current; k++)
	{
		fgets(st1, 30, file);

		for (int i = 0; i < snum; i++)
		{
			fscanf(file, "%f %f %f\n", &unused[0], &unused[1], &unused[2]);
		}
	}*/
	fgets(st1, 30, file);
	fgets(st1, 30, file);
	model->numseeds = snum;
	model->seeds = (GLfloat *)malloc(sizeof(GLfloat)* 3 * (model->numseeds));

	for (int i = 0; i < model->numseeds; i++)
	{

		fscanf(file, "%f %f %f\n", &model->seeds[3 * i + 0], &model->seeds[3 * i + 1], &model->seeds[3 * i + 2]);
	}

	fclose(file);

}

//Random Sampling seeds
void setSeeds()
{
	cout << "Set seed ..." << endl;

	int N = 999;
	model->numseeds = snum;
	model->seeds = (GLfloat *)malloc(sizeof(GLfloat)*
		3 * (model->numseeds));

	for (int i = 0; i<model->numseeds; i++)
	{
		//srand((unsigned)time(NULL));
		int index = rand() % model->numtriangles;

		double A[] = { model->vertices[model->triangles[index].vindices[0] * 3 + 0],
			model->vertices[model->triangles[index].vindices[0] * 3 + 1],
			model->vertices[model->triangles[index].vindices[0] * 3 + 2] };

		double B[] = { model->vertices[model->triangles[index].vindices[1] * 3 + 0],
			model->vertices[model->triangles[index].vindices[1] * 3 + 1],
			model->vertices[model->triangles[index].vindices[1] * 3 + 2] };

		double C[] = { model->vertices[model->triangles[index].vindices[2] * 3 + 0],
			model->vertices[model->triangles[index].vindices[2] * 3 + 1],
			model->vertices[model->triangles[index].vindices[2] * 3 + 2] };

		double r1 = rand() % (N + 1) / (float)(N + 1);
		double r2 = rand() % (N + 1) / (float)(N + 1);

		double temp_seed[3];

		temp_seed[0] = (1 - sqrt(r1))*A[0] + sqrt(r1)*(1 - r2)*B[0] + sqrt(r1)*r2*C[0];
		temp_seed[1] = (1 - sqrt(r1))*A[1] + sqrt(r1)*(1 - r2)*B[1] + sqrt(r1)*r2*C[1];
		temp_seed[2] = (1 - sqrt(r1))*A[2] + sqrt(r1)*(1 - r2)*B[2] + sqrt(r1)*r2*C[2];

		int temp = 0;
		while (temp<10000)
		{
			bool tag = true;
			for (int ttt = 0; ttt<i; ttt++){
				double distance_to_seed = pow(temp_seed[0] - model->seeds[3 * ttt + 0], 2) +
					pow(temp_seed[1] - model->seeds[3 * ttt + 1], 2) +
					pow(temp_seed[2] - model->seeds[3 * ttt + 2], 2);

				if (distance_to_seed < pow(radius,2) * 2)
				{
					tag = false;
					break;
				}
			}
			if (tag)
				break;
			else
			{
				index = (rand() + index) % model->numtriangles;
				temp++;
			}
		}

		model->seeds[3 * i + 0] = temp_seed[0];
		model->seeds[3 * i + 1] = temp_seed[1];
		model->seeds[3 * i + 2] = temp_seed[2];


	}

}

void writeSeeds()
{
	ofstream ofile;
	ofile.open(seed_path, ios::app);
	if (ofile.fail())
	{
		cout << "failed to open seed file" << endl;
		exit(2);
	}

	ofile << fz.files[model_current].name << endl;
	for (int j = 0; j < model->numseeds; j++)
	{
		ofile << model->seeds[3 * j + 0] << " " << model->seeds[3 * j + 1] << " " << model->seeds[3 * j + 2] << endl;
	}

	ofile.close();
}



void del()
{
	delete[] model;
}

BOOL WriteBitmapFile(const char * filename, int width, int height, unsigned char * bitmapData)
{
	//填充BITMAPFILEHEADER
	BITMAPFILEHEADER bitmapFileHeader;
	memset(&bitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = 0x4d42;	//BM
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//填充BITMAPINFOHEADER
	BITMAPINFOHEADER bitmapInfoHeader;
	memset(&bitmapInfoHeader, 0, sizeof(BITMAPINFOHEADER));
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = width;
	bitmapInfoHeader.biHeight = height;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB;
	bitmapInfoHeader.biSizeImage = width * abs(height) * 3;

	//////////////////////////////////////////////////////////////////////////
	FILE * filePtr;			//连接要保存的bitmap文件用
	unsigned char tempRGB;	//临时色素
	int imageIdx;

	//交换R、B的像素位置,bitmap的文件放置的是BGR,内存的是RGB
	for (imageIdx = 0; imageIdx < bitmapInfoHeader.biSizeImage; imageIdx += 3)
	{
		tempRGB = bitmapData[imageIdx];
		bitmapData[imageIdx] = bitmapData[imageIdx + 2];
		bitmapData[imageIdx + 2] = tempRGB;
	}

	filePtr = fopen(filename, "wb");
	if (NULL == filePtr)
	{
		return FALSE;
	}

	fwrite(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);

	fwrite(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

	fwrite(bitmapData, bitmapInfoHeader.biSizeImage, 1, filePtr);

	fclose(filePtr);
	return TRUE;
}

void saveScreenShot(int clnHeight, int clnWidth, GLfloat angle)
{
	//int clnHeight,clnWidth;	//client width and height
	static void * screenData;
	//  RECT rc;
	int len = clnWidth * clnHeight * 3;
	screenData = malloc(len);
	memset(screenData, 0, len);
	glReadPixels(0, 0, clnWidth, clnHeight, GL_RGB, GL_UNSIGNED_BYTE, screenData);

	/*int blackindex = (model_current)*view_num + angle / angle_interval + 1;
	string blackbmpfilename = projection_path + "\\"
	+ std::to_string(blackindex) + ".bmp";*/
	int view_current = angle / 30 + 1;

	int blackindex = (model_current)*view_num + view_current + black_begin;

	/*string projection_file = projection_path + "\\" +fz.files[model_current].name
		+ "_"  + std::to_string(view_current) + ".bmp";*/

	string projection_file = projection_path + "\\" + to_string((long double)blackindex) + ".bmp";

	BmpImage* image = readbmp(projection_file);

	Patch * mypatch = getpatch((unsigned char *)screenData, clnWidth, clnHeight);
	HOGDescriptor *desc = new HOGDescriptor(cvSize(patch_size, patch_size), cvSize(16, 16), cvSize(8, 8), cvSize(8, 8), 9);
	
	//如果看得到被选中的区域
	if (mypatch->x1 > -0.5)
	{
		//左下角点
		BmpImage* blackpatch = NULL;
		if (ceil(mypatch->x1*image->width) + patch_size - 1 <= image->width && ceil(mypatch->y1*image->height) + patch_size - 1 <= image->height)
			blackpatch = imcrop(image, (int)ceil(mypatch->x1*image->width), (int)ceil(mypatch->y1*image->height), patch_size - 1);
		//右下角
		else if (mypatch->x2*image->width - patch_size + 1 >= 1 && ceil(mypatch->y1*image->height) + patch_size - 1 <= image->height)
			blackpatch = imcrop(image, (int)floor(mypatch->x2*image->width - patch_size + 1), (int)ceil(mypatch->y1*image->height), patch_size - 1);
		//左上角
		else if (ceil(mypatch->x1*image->width) + patch_size - 1 <= image->width && mypatch->y2*image->height - patch_size + 1 >= 1)
			blackpatch = imcrop(image, (int)ceil(mypatch->x1*image->width), (int)floor(mypatch->y2*image->height - patch_size + 1), patch_size - 1);
		//右上角
		else if (mypatch->x2*image->width - patch_size + 1 >= 1 && mypatch->y2*image->height - patch_size + 1 >= 1)
			blackpatch = imcrop(image, (int)floor(mypatch->x2*image->width - patch_size + 1), (int)floor(mypatch->y2*image->height - patch_size + 1), patch_size - 1);
		else//图片太小
		{
			free(screenData);
			return;
		}

		//char seed_index[100];
		//sprintf(seed_index, "%02d", (model->seed_current + 1));
		//string index_seed(seed_index);

		string patch_file = patch_path + "\\" + to_string((long double)view_current) + "\\" +
			to_string((long double)model_current + 1 + model_begin) + "_" +
			to_string((long double)model->seed_current + 1) + ".bmp";

		//cout << "What the hell" << endl;
		WriteBitmapFile(patch_file.data(), patch_size, patch_size, (unsigned char*)blackpatch->dataOfBmp);
		
		patch_file = patch_path + "\\" + to_string((long double)view_current) + "\\" +
			to_string((long double)model_current + 1 + model_begin) + "_" +
			to_string((long double)model->seed_current + 1) + ".txt";
		
		Mat M(patch_size, patch_size, CV_8UC3, (unsigned char*)blackpatch->dataOfBmp);

		//IplImage *srcImage = cvLoadImage(patch_img_file.data(), 0);
		vector<float> czimagetempHOG;
		
		desc->compute(M, czimagetempHOG);

		ofstream ofile;
		ofile.open(patch_file, ios::out);
		if (ofile.fail())
		{
			cout << "failed to open patch file" << endl;
			exit(2);
		}
		for (vector<float>::size_type iHOG = 0; iHOG<czimagetempHOG.size(); iHOG++)
		{
			ofile << czimagetempHOG[iHOG] << endl;
		}

		ofile.close();

		delete []blackpatch->dataOfBmp;
		delete blackpatch;
	}
	//释放内存
	delete []image->dataOfBmp;
	delete image;
	delete mypatch;
	free(screenData);
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
	ifs >> tmp >> projection_path
		>> tmp >> patch_path
		>> tmp >> models_path
		>> tmp >> seed_path
		>> tmp >> snum
		>> tmp >> view_num
		>> tmp >> patch_size
		>> tmp >> radius
		>> tmp >> black_begin
		>> tmp >> model_begin
     	>> tmp >> seed_exist;

	ifs.close();

	if (_access(patch_path.c_str(), 0) == -1)
	{
		_mkdir(patch_path.data());
	}
	for (int i = 1; i <= view_num; i++)
	{
		string temp = patch_path + "\\" + to_string((long double)i);
		if (_access(temp.c_str(), 0) == -1)
		{
			_mkdir(temp.data());
		}

	}
	
	//initial model
	fz.name = models_path;
	fz.type = "obj";
	fz.getFiles();
	cout << "initial done" << endl;
}

void initRendering()
{
	cout << "initRendering" << endl;
	glClearColor(1.0, 1.0, 1.0, 0.0);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glShadeModel(GL_SMOOTH);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_NORMALIZE);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHT1);

	GLfloat mat_ambient[] = { 32.0 / 255.0f, 32.0 / 255.0f, 32.0 / 255.0f, 1.0f };
	GLfloat mat_diffuse[] = { 182.0 / 255.0f, 182.0 / 255.0f, 182.0 / 255.0f, 1.0f };
	GLfloat mat_specular[] = { 0.6f, 0.6f, 0.6f, 1.0f };


	//GLfloat mat_ambient[] = { 32.0/255.0f, 32.0/255.0f, 32.0/255.0f, 1.0f };
	//GLfloat mat_diffuse[] = { 1.0f,0.0f, 0.0f, 1.0f };
	//GLfloat mat_specular[] = { 0.6f, 0.2f, 0.0f, 1.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, mat_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, mat_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, mat_specular);

	/*glLightfv(GL_LIGHT1, GL_AMBIENT, mat_ambient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, mat_diffuse);
	glLightfv(GL_LIGHT1, GL_SPECULAR, mat_specular);*/

	glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glDisable(GL_CULL_FACE);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	/*gluLookAt(0, 1 * 2, -1.732 * 2,
		0, 0, 0,
		0, 1 * 2, 1.732 * 2);*/
	gluLookAt(0, -1.732 * 2, 1 * 2,
		0, 0, 0,
		0, 1 * 2, 1.732 * 2);
}

void reshape(int w, int h)
{
	cout << "Reshape " << endl;
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	//gluPerspective(60.0,(GLfloat)w/(GLfloat)h,1.0,20.0);
	glOrtho(-2, 2, -2, 2, 1.0, 20.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, -1.732 * 2, 1 * 2,
	0, 0, 0,
	0, 1 * 2, 1.732 * 2);
}

void render()
{
	cout << "render" << endl;
	for (int k = 0; k < fz.files.size(); k++)
	{
		start = clock();
		model = glmReadOBJ(const_cast<char *>(fz.files[model_current].path.c_str()));
		int pos = fz.files[model_current].path.find_last_of('\\');
		int length = fz.files[model_current].path.length();
		string seed_filename = seed_path + "\\"+ fz.files[model_current].path.substr(pos + 1, length -5-pos) + ".off";
		if (seed_exist)
			read_seed(seed_filename);
		else
			setSeeds();

		for (int i = 0; i < 30; i++)
		{
			for (GLfloat angle = 0; angle < 360; angle = angle + 30)
			{

				glPushMatrix();
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
				//glRotatef((GLfloat)angle, 0, 1, 0);
				glRotatef((GLfloat)angle, 0, 0, 1);
				//glCallList(mlist);

				glTranslatef(model->seeds[model->seed_current * 3 + 0],
					model->seeds[model->seed_current * 3 + 1],
					model->seeds[model->seed_current * 3 + 2]);

				glColor3f(0.0f, 0.0f, 0.8f);
				glutSolidSphere(0.15, 16, 16);

				glTranslatef(-model->seeds[model->seed_current * 3 + 0],
					-model->seeds[model->seed_current * 3 + 1],
					-model->seeds[model->seed_current * 3 + 2]);

				for (int i = 0; i < model->numtriangles; i++)
				{
					glBegin(GL_TRIANGLES);
					//glPolygonMode(GL_FRONT_AND_BACK, GL_FILL); 
					//cout << "color size is" << _msize(model->iscolored)/sizeof(bool);

					glColor3f(0.71f, 0.71f, 0.71f);


					int ifindex = model->triangles[i].findex;
					glNormal3f(model->facetnorms[3 * ifindex], model->facetnorms[3 * ifindex + 1], model->facetnorms[3 * ifindex + 2]);

					glVertex3f(model->vertices[model->triangles[i].vindices[0] * 3 + 0],
						model->vertices[model->triangles[i].vindices[0] * 3 + 1],
						model->vertices[model->triangles[i].vindices[0] * 3 + 2]);

					glVertex3f(model->vertices[model->triangles[i].vindices[1] * 3 + 0],
						model->vertices[model->triangles[i].vindices[1] * 3 + 1],
						model->vertices[model->triangles[i].vindices[1] * 3 + 2]);

					glVertex3f(model->vertices[model->triangles[i].vindices[2] * 3 + 0],
						model->vertices[model->triangles[i].vindices[2] * 3 + 1],
						model->vertices[model->triangles[i].vindices[2] * 3 + 2]);

					glEnd();

				}

				glPopMatrix();

				saveScreenShot(window_size, window_size, angle);
				//glutSwapBuffers();
			}
			//glDeleteLists(mlist, 1);
			//mlist = glmList(model, 0);
			model->seed_current++;
			//cout << model->seed_current << endl;
		}

		cout << fz.files[model_current].file << " done!" << endl;
		if (!seed_exist)
			writeSeeds();

		glmDelete(model);
		model_current++;
		finish = clock();
		cout << "it takes " << (finish - start) / 1000 << "seconds" << endl;

		
	}
}

void normalize()
{
	cout << "normalize shapes" << endl;
	for (int k = 0; k < fz.files.size(); k++)
	{
		start = clock();
		model = glmReadOBJ(const_cast<char *>(fz.files[model_current].path.c_str()));
		int pos = fz.files[model_current].path.find_last_of('\\');
		int length = fz.files[model_current].path.length();
		string file_name = fz.files[model_current].path.substr(pos + 1, length-1-pos);
		char *temp = new char[length - 1 - pos +1];
		int index = 0;
		for (int i = 0; i < length - 1 - pos;i++)
		{
			if (file_name.at(i) == ' ')
			{
				continue;
			}
			else{
				temp[index] = file_name.at(i);
				index++;
			}
		}
		temp[index] = '\0';
		
		remove(model->pathname);
		string fix_filename = fz.files[model_current].path.substr(0, pos) +"\\" + temp;

		glmWriteOBJ(model, const_cast<char *>(fix_filename.c_str()), GLM_NONE);
		
		cout << fz.files[model_current].file << " done!" << endl;
		glmDelete(model);
		delete[] temp;
		model_current++;
		finish = clock();
		
		cout << "it takes " << (finish - start) / 1000 << "seconds" << endl;
	}
	
}

int main(int argc, char * argv[])
{
	initialize("params.cfg");
	if (argc <2)
		return 0;

	option = argv[1];

	if (option == "normalize")
	{
		normalize();
		return 0;
	}
	else if (option == "sampling"){

		glutInit(&argc, argv);
		glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
		glutInitWindowSize(window_size, window_size);
		glutInitWindowPosition(window_size, 100);
		glutCreateWindow("Sampling parts");
		glutHideWindow();
		initRendering();
		reshape(window_size, window_size);
		render();
		return 0;
	}
	else{
		cout << "wrong option. please input normalize or sampling" << endl;
		return 0;
	}
	return 0;
}