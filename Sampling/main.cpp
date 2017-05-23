
#include "FileZ.h"
#include "CommonZ.h"
#include "model.h"
#include "readbmp.h"
#include "windows.h"
#include "direct.h"
#include "time.h"
#include "stdio.h"
#include <iostream>

using namespace std;

GLfloat angle_interval = 0;
GLint patch_size = 0;
GLint black_begin = 0;
GLint model_begin = 0;

GLMmodel *model;
double threshold = 0.0;
GLfloat angle = 0;
GLfloat angle_max = 0;
GLfloat radius = 0.0;


int model_current = 0;

int snum = 0;
int view_num = 0;
string projection_path;
string patch_path;
string models_path;
string seed_path;

string render_path;
FileZ fz;
GLint mlist;

clock_t start;
clock_t finish;


//Sampling seeds
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

void GL_myInitial()
{
	GLfloat light_position[] = { 1.0, 1.0, 1.0, 0.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	//GLfloat light_ambient [] = {0.75f, 0.75f, 0.75f, 1.0f};
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDepthFunc(GL_LESS);
	glEnable(GL_DEPTH_TEST);
}

BOOL WriteBitmapFile(const char * filename, int width, int height, unsigned char * bitmapData)
{
	//BITMAPFILEHEADER
	BITMAPFILEHEADER bitmapFileHeader;
	memset(&bitmapFileHeader, 0, sizeof(BITMAPFILEHEADER));
	bitmapFileHeader.bfSize = sizeof(BITMAPFILEHEADER);
	bitmapFileHeader.bfType = 0x4d42;	//BM
	bitmapFileHeader.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);

	//BITMAPINFOHEADER
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
	FILE * filePtr;			//
	unsigned char tempRGB;	
	int imageIdx;


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

void saveScreenShot(int clnHeight, int clnWidth)
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
	int view_current = angle / angle_interval + 1;

	int blackindex = (model_current)*view_num + view_current + black_begin;

	/*string projection_file = projection_path + "\\" +fz.files[model_current].name
		+ "_"  + std::to_string(view_current) + ".bmp";*/

	string projection_file = projection_path + "\\" + std::to_string(blackindex) + ".bmp";

	BmpImage* image = readbmp(projection_file);

	Patch * mypatch = getpatch((unsigned char *)screenData, clnWidth, clnHeight);

	if (mypatch->x1 > -0.5)
	{

		BmpImage* blackpatch = NULL;
		if (ceil(mypatch->x1*image->width) + patch_size - 1 <= image->width && ceil(mypatch->y1*image->height) + patch_size - 1 <= image->height)
			blackpatch = imcrop(image, (int)ceil(mypatch->x1*image->width), (int)ceil(mypatch->y1*image->height), patch_size - 1);

		else if (mypatch->x2*image->width - patch_size + 1 >= 1 && ceil(mypatch->y1*image->height) + patch_size - 1 <= image->height)
			blackpatch = imcrop(image, (int)floor(mypatch->x2*image->width - patch_size + 1), (int)ceil(mypatch->y1*image->height), patch_size - 1);

		else if (ceil(mypatch->x1*image->width) + patch_size - 1 <= image->width && mypatch->y2*image->height - patch_size + 1 >= 1)
			blackpatch = imcrop(image, (int)ceil(mypatch->x1*image->width), (int)floor(mypatch->y2*image->height - patch_size + 1), patch_size - 1);

		else if (mypatch->x2*image->width - patch_size + 1 >= 1 && mypatch->y2*image->height - patch_size + 1 >= 1)
			blackpatch = imcrop(image, (int)floor(mypatch->x2*image->width - patch_size + 1), (int)floor(mypatch->y2*image->height - patch_size + 1), patch_size - 1);
		else
		{
			free(screenData);
			return;
		}

		//char seed_index[100];
		//sprintf(seed_index, "%02d", (model->seed_current + 1));
		//string index_seed(seed_index);

		string patch_file = patch_path + "\\" + to_string(view_current) + "\\" +
			to_string(model_current + 1 + model_begin) + "_" +
			to_string(model->seed_current + 1) + ".bmp";

		//cout << "What the hell" << endl;
		WriteBitmapFile(patch_file.data(), patch_size, patch_size, (unsigned char*)blackpatch->dataOfBmp);
		free(blackpatch->dataOfBmp);
		free(blackpatch);

		/*string render_file = render_path + "\\" +
			fz.files[model_current].name + "_" +
			index_seed + "_" +
			to_string(view_current) +
			".bmp";
			WriteBitmapFile(render_file.data(), clnWidth, clnHeight, (unsigned char*)screenData);*/
	}
	
	free(image->dataOfBmp);
	free(image);
	free(mypatch);
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
		>> tmp >> render_path
		>> tmp >> seed_path
		>> tmp >> snum
		>> tmp >> view_num
		>> tmp >> angle_max
		>> tmp >> patch_size
		>> tmp >> angle_interval
		>> tmp >> radius
		>> tmp >> black_begin
		>> tmp >> model_begin;

	ifs.close();

	for (int i = 1; i <= view_num; i++)
	{
		string temp = patch_path + "\\" + to_string(i);
		if (_access(temp.c_str(), 0) == -1)
		{
			_mkdir(temp.data());
		}

	}

	//mkdir
	/*for (int i = 1; i <= view_num; i++)
	{
		string temp = patch_path + "\\" + to_string(i);
		FileZ tmp;
		tmp.name = temp;
		if (!tmp.isExist())
		{
			_mkdir(temp.data());
		}
	}*/

	//initialize view
	//angle = -angle_interval;
	angle = 0;
	threshold = pow(radius, 2) * 3;
	
	//initial model
	fz.name = models_path;
	fz.type = "obj";
	fz.getFiles();
	model = glmReadOBJ(const_cast<char *>(fz.files[model_current].path.c_str()));
	setSeeds();
	

}

void initRendering()
{
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
	mlist = glmList(model, 0);
}

void reshape(int w, int h)
{
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
	/*gluLookAt(0, 1 * 2, -1.732 * 2,
		0, 0, 0,
		0, 1 * 2, 1.732 * 2);*/
}

void render()
{
	/*if(!model[model_current]->isstyle[model->seed_current])
	return;*/

	glPushMatrix();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glRotatef((GLfloat)angle, 0, 1, 0);
	glRotatef((GLfloat)angle, 0, 0, 1);
	glCallList(mlist);

	glPopMatrix();
	
	
	saveScreenShot(700, 700);
	glutSwapBuffers();

	if (angle > angle_max)
	{
		angle = 0;
		model->seed_current++;
		if (model->seed_current == snum)
		{
			cout << fz.files[model_current].file << " done!" << endl;

			//save seeds
			writeSeeds();
			glmDelete(model);		
			model_current++;
			//read the next model
			if (model_current == fz.files.size())
			{
				cout << "All shapes " << fz.files.size() << " have been handled!" << endl;
				finish = clock();
				cout << "it takes " << (finish - start) / 1000 << "seconds" << endl;
				cin.get();
				exit(0);
			}
			else
			{
				model = glmReadOBJ(const_cast<char *>(fz.files[model_current].path.c_str()));
				setSeeds();
			}
					
		}
		//Notice: delete the previous list before create a new one,
		//otherwise, memory explosion will happen !
		glDeleteLists(mlist, 1);
		mlist = glmList(model, 0);
	}
	else{
		angle = angle + angle_interval;
	}
	//Sleep(1000);
	glutPostRedisplay();
}

void update(void) {

	//Sleep(1000);
	//if(model[model_current]->isstyle[model->seed_current])
	saveScreenShot(700, 700);
	glutPostRedisplay();

	
}



int main(int argc, char * argv[])
{

	start = clock();

	initialize("params.cfg");
	//isincube();
	//GL_myInitial();//

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(700, 700);
	glutInitWindowPosition(700, 100);     
	glutCreateWindow("Sampling patches");
	initRendering();
	glutDisplayFunc(render);
	glutReshapeFunc(reshape);
//	glutIdleFunc(&update);
	glutMainLoop();
	
	//cin.get();
	return 0;

}