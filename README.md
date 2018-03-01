# Semi-Supervised-Co-Analysis-of-3D-Shape-Styles-from-Projected-Lines
This is a release code for our paper accepted on "ACM Transactions On Graphics".

### Intruduction

In this repository, we release codes for style patch sampling, feature encoding, label-constrainted style clustering and style patch back projection.

### Platform

Our code has been tested on Windows 8.1.

### Installation

MATLAB, OPENGL, OPENCV should be installed.

Our code has been tested on MATLAB 2016a and OPENCV 2.4.8.

Our patch sampling and feature encoding code is complied on VS2010.

Our back projection code is complied on VS2012.

If you have set up the environment, you can use the executive file we provided.

You can also compile the code by yourself, if you have any question in compiling or other process, please contact me: yufg_nju@foxmail.com.

### Usage 1: style localization

	Style localization presented in our paper are released, shapes are mainly selected from our datasets and datasets of [Hu et al 2017].

1. Data setup :

	The patch features and style kernels have already been calculated for your convenience. Each subfolder in 'data' indicates a dataset. 

	You can download according to the link below and put them in 'data' folder, then run style localization one dataset each time.

	The size of feature files are a little big, so I put them in network disk alone:
	  
		https://pan.baidu.com/s/1nwhU9NN

2. style localization :
	
	(a). Set the name of dataset that you want to run. 

	Please set your data set name to variable 'dataname' in 'proj_style\startup.m'. It should be consistent to the name of subfolder in 'data' folder, eg. hu_building. 
	
	So that the program can read data from the certain folder.

	(b). Calculate style localization for each shape.

	If you want to run data sets from [Hu et al 2017] and cars from our data sets, please run 
	 
		output_hu_style.m

	If you want to run furniture and building from our data sets, please run 
	 
		output_our_style.m
		
	The result is outputed to proj_style\data\data_set_name\style_path\style_index.txt.

2. Run matlab scripts :
	
	(c). Show style localization. You just need to change name of data set in Back_projected\params.cfg, and make sure that the path is right to the data set folder you want. 

	Then run the executive file:

		Back_projected\back_projection.exe

	In the directory 'proj_style\data\data_set_name\style_path', there are images which show representative style patch localization on selected shapes. 

	The style localization is colored blue. If you can't find blue in one picture, it may mean that the style area can't be seen in this view.

	These images are named as 'model index\_patch index\_view index.bmp'.

### Usage 2: Complete process

	Complete process is a little time consuming for large data set, so we've provided a small furniture set named 'demo' which contains 30 models of two kinds of style.

1. Data setup :

	You can download 3dlines and models according to the link below.
	  
		https://pan.baidu.com/s/1cQGegI
	
	Please unzip and put them in 'data' folder, then run one dataset each time.
	
	Note one subfolder in 'data' only for one data set. 
	
	Make sure the bmp files are put in 'proj_style\data\data_set_name\3dlines' and the obj files are put in 'proj_style\data\data_set_name\models'.
	
2. Sample patches and calculate hog for patches :
	
   (a). Set the name of dataset that you want to run. 

   ing to the name of subfolder in 'data' folder, eg. hu_building, so that the program can read data from the certain folder.
   
   (b). Calculate style localization for each shape.

   If you want to run data sets from [Hu et al 2017] and cars from our data sets, please run 
     
        output_hu_style.m
  
   If you want to run furniture and building from our data sets, please run 
     
        output_our_style.m
		
   The result is output to proj_style\data\dataname\style_path\style_index.txt.
   
### License

Copyright (c) 2017

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
