# Semi-Supervised-Co-Analysis-of-3D-Shape-Styles-from-Projected-Lines

### Intruduction

In this repository, we release codes for style patch sampling, feature encoding, label-constrainted style clustering and style patch back projection.

You can try style localization presented in our paper and run demo in complete process with all released code.

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

Style localization presented in figures(Fig 1, Fig7, Fig 20, Fig 22) of our paper are released, these shapes are mainly selected from datasets of [Hu et al 2017] and our data sets.

A figure may contain shapes from varied datasets. For user's convenience, our code is set to output results of shapes from one dataset for all figures one pass.

**Step 1. Data setup :**

The features of patch and style kernels have already been calculated for user's convenience. 

Each subfolder in folder 'data' contains files of one dataset. 

You can download features according to the link below and put them in 'data' folder, then run style localization one dataset each time.

The size of feature files are a little big, so I put them in network disk alone:
```
https://www.dropbox.com/sh/b4wrexn5txqit1d/AABuZqyZOGrWj4zLeRRRXCb5a?dl=0
```

Take one dataset 'our_furniture' for example, your files should be placed as follow:

```
data\our_furniture\cluster
data\our_furniture\hog
data\our_furniture\kernel
data\our_furniture\models
data\our_furniture\pslf
data\our_furniture\sample_points
```

**Step 2. Style localization :**
	
(a). Run scripts in 'scripts' folder for one dataset each time. For example: 

```
$ cd scripts\our_furniture
$ .\style_localization.sh
$ .\show_style.sh
```

Please run these commonds in order.

In the directory 'data\our_furniture\style_patch', there are output pictures which show representative style patch localization on selected shapes. 

The style localization is colored blue. If you can't find blue in one picture, it may mean that the style area can't be seen in this view.

### Usage 2: Complete Runing process

If you want to calculate features by yourself, you can try the complete running process.

Complete process is a little time consuming for large data set in current CPU version. 

We've provided a small furniture set named 'demo' which contains 30 models of two kinds of style for fast experience.

The following settings are examples of 'demo'.

**Step 1. Data setup :**

You can download 3dlines and models(.obj) of demo and our complete data set according to the link below.
```
https://www.dropbox.com/sh/rh3om1bxde0ivzn/AACAlUVZ8t6MBzGLzFf9SUr_a?dl=0
```
Please unzip and put 3dlines projected images, models and labels of demo in 'data\demo' folder.

Note one subfolder in 'data' is prepared for only one data set, eg. 'data\furniture400'. 

No subfolder in 'data\demo\3dlines' and 'data\demo\models'. You files should be placed as follow:

```
data\demo\3dlines\*.bmp
data\demo\models\*.obj
data\demo\labels.mat
```

**Step 2. Feature extraction and style analysis :**
	
Please run these commonds in order:

```
$ cd scripts\demo
$ .\sample.sh
$ .\cluster.sh
$ .\convolute.sh
$ .\style_localization.sh
$ .\show_style.sh
```

In the directory 'data\demo\style_patch', there are output pictures which show representative style patch localization on all shapes. 

The style localization is colored blue. If you can't find blue in one picture, it may mean that the style area can't be seen in this view.

**If you want run more big data sets...**

Please follow step 1 to set up dataset that you want to run.

Then new and change following files that indicates path of dataset.

Specifically, change the name of dataset 'demo' anywhere it appears in these files to the dataset name you want, eg. furniture400, .

```
Back_projected\params_demo.cfg  	-> Back_projected\params_furniture400.cfg
convolutional\params_demo.cfg   	-> convolutional\params_furniture400.cfg
sample_and_hog\params_demo.cfg  	-> sample_and_hog\params_furniture400.cfg
scripts\demo\sample.sh          	-> scripts\furniture400\sample.sh
scripts\demo\cluster.sh          	-> scripts\furniture400\cluster.sh
scripts\demo\convolute.sh          	-> scripts\furniture400\convolute.sh
scripts\demo\style_localization.sh  	-> scripts\furniture400\style_localization.sh
scripts\demo\show_style.sh          	-> scripts\furniture400\show_style.sh
```
Finally, you can follow step 2 to run the code.

## Citation
If you use this code, please cite the following paper.
```
@article {yu_tog18,
     title = {Semi-Supervised Co-Analysis of 3D Shape Styles from Projected Lines},
     author = {Fenggen Yu and Yan Zhang and Kai Xu and Ali Mahdavi-Amiri and Hao Zhang},
     journal = {ACM Transactions on Graphics},
     volume = {37},
     number = {2},
     pages = {to appear},
     year = {2018}
}
```

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
