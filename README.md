# Semi-Supervised-Co-Analysis-of-3D-Shape-Styles-from-Projected-Lines

### Intruduction

In this repository, we release codes for style patch sampling, feature encoding, label-constrainted style clustering and style patch back projection.

You can try style localization presented in our paper and run demo of complete process with all released code.

### Platform

Our code has been tested on Windows 8.1.

### Installation

MATLAB, OPENGL, OPENCV should be installed.

Our code has been tested on MATLAB 2016a and OPENCV 2.4.8.

Our code of patch sampling and feature encoding is complied on VS2010.

Our code of back projection is complied on VS2012.

If you have set up the environment, you can use the executive file we provided directly.

You can also compile the code by yourself, if you have any question in compiling or other process, please contact me: yufg_nju@foxmail.com.

### Usage 1: style localization

Style localization presented in our paper are released, these shapes are mainly selected from datasets of [Hu et al 2017] and our data sets.

**Step 1. Data setup :**

The features of patch and style kernels have already been calculated for user's convenience. 

Each subfolder in folder 'data' contains files of one dataset. 

You can download features according to the link below and put them in 'data' folder, then run style localization one dataset each time.

The size of feature files are a little big, so I put them in network disk alone:
```
https://pan.baidu.com/s/1nwhU9NN
```

**Step 2. Style localization :**
	
(a). Set the name of dataset that you want to run. 

Please set your data set name to variable 'dataname' in 'startup.m'.

```
dataname='demo'; %data set name you are going to run
```

It should be consistent to the name of subfolder in 'data' folder, eg. hu_building or our_furniture. 

So that the program can read data from the certain folder.

(b). Style localization for each shape.

If you want to try data sets from [Hu et al 2017] and cars from our data sets, please run following script in MATLAB:
```
output_hu_style.m
```
If you want to try furniture and building from our data sets, please run following script in MATLAB:
```
output_our_style.m
```
The style localizationresult is output to 'data\data_set_name\style_path\style_index.txt'.

**Step 3. Show style localization :**
	
You just need to change name of data set in 'Back_projected\params.cfg', and make sure that the paths is right to the data set folder you want. 
```
model_path:       ..\data\data_set_name\models
back_projection_path:     ..\data\data_set_name\style_patch
seed_path:        ..\data\data_set_name\sample_points
style_path:       ..\data\data_set_name\style_patch\style_index.txt
```
Then run the executive file:
```
$ Back_projected\back_projection.exe
```
In the directory 'data\data_set_name\style_patch', there are output pictures which show representative style patch localization on selected shapes. 

The style localization is colored blue. If you can't find blue in one picture, it may mean that the style area can't be seen in this view.

### Usage 2: Complete Runing process

If you want to calculate features by yourself, you can try the complete running process.

Complete process is a little time consuming for large data set in current CPU version. 

We've provided a small furniture set named 'demo' which contains 30 models of two kinds of style for fast experience.

The following settings are examples of 'demo'.

**Step 1. Data setup :**

You can download 3dlines and models(.obj) of demo and our complete data set according to the link below.
```
https://pan.baidu.com/s/1cQGegI
```
Please unzip and put 3dlines projected images and models of demo in 'data\demo' folder.

Note one subfolder in 'data' is prepared for only one data set, eg. 'data\furniture'. 

Make sure the 3dlines projected files are put in 'data\data_set_name\3dlines' and the obj files are put in 'data\data_set_name\models'. 

No subfolder in 'data\demo\3dlines' and 'data\demo\models'.

```
data\demo\3dlines\*.bmp
data\demo\models\*.obj
```

**Step 2. Sample patches and calculate hog features for patches :**
	
You just need to change name of data set in 'sample_and_hog\params.cfg', and make sure that the paths is right to the data set folder you want. 
```
projection_path: ..\data\demo\3dlines
patch_path:      ..\data\demo\patch
model_path:      ..\data\demo\models
seed_path:       ..\data\demo\sample_points
```
Then run the executive file:
```
$ sample_and_hog\sample.exe sampling
```
This can output directories: 'data\demo\patch' and 'data\demo\sample_points'. 

**Step 3. Feature preparation and Clustering :**
	
Please set your data set name to variable 'dataname' in 'startup.m'.

```
dataname='demo_sample'; %data set name you are going to run
```
Please feature_extraction = 1 in 'Complete_run.m'.
```
feature_extraction = 1; % 1 for feature preparation, 0 for feature fusion
```
Then run 'Complete_run.m' in MATLAB:
```
$ Complete_run.m
```
This can output directories: 'data\demo\cluster', 'data\demo\hog', 'data\demo\kernel', 'data\demo\imagesname', 'data\demo\txt'. 

**Step 4. Convolute images :**
	
You just need to change name of data set in 'convolutional\params.cfg', and make sure that the paths is right to the data set folder you want. 

```
curpath ..\data\demo
```
Then run the executive file:
```
$ convolute.exe
```
This can output txt files of convolutional features in 'data\demo\3dlines'. 

**Step 5. Feature fusion, style clustering and style localization :**
	
Please set feature_extraction = 0 in 'Complete_run.m'.
```
feature_extraction = 0; % 1 for feature preparation, 0 for feature fusion
```
Then run 'Complete_run.m' in MATLAB:
```
$ Complete_run.m
```
You can see the output style clustering purity and change of number of style kernels in MATLAB terminal.

This can also output txt files of style localization in 'data\demo\style_patch\style_index.txt'. 

**Step 6. Show style localization :**
	
You just need to change name of data set in 'Back_projected\params.cfg', and make sure that the paths is right to the data set folder you want. 
```
model_path:       ..\data\demo\models
back_projection_path:     ..\data\demo\style_patch
seed_path:        ..\data\demo\sample_points
style_path:       ..\data\demo\style_patch\style_index.txt
```
Then run the executive file:
```
$ Back_projected\back_projection.exe
```
In the directory 'data\demo\style_path', there are output pictures which show representative style patch localization on selected shapes. 

The style localization is colored blue. If you can't find blue in one picture, it may mean that the style area can't be seen in this view.

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
