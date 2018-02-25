# Semi-Supervised-Co-Analysis-of-3D-Shape-Styles-from-Projected-Lines
This is a submission code for ACM Transactions On Graphics

### Intruduction

In this repository, we release codes for style patch sampling, feature encoding, label-constrainted style clustering and style patch back projection.

We've provided a mixed models set which contains 30 models of two kinds of style in directory 'all_model'.

Projective features of all these shapes have already been calculated for conveniently running our program.

The example style clustering program is run with 30% style labels constrainted.

Both style clustering results and style patches localization for all shapes can be obtained finally.

### Installation

MATLAB 2016a should be installed.

### Usage

1. Run label-constrainted style clustering in matlab:
  
        Easy_run.m
  
   You can get:
   
   (a). Purity of the style clustering result.
   
   (b). New output directory 'style_cluster_result': Style clustering results for each shape.
   
   (c). New output directory 'style_patch': The representative style patch index file(.txt) for all shapes.

2. Get style patches back projected to shape:
  
        Back_projected\back_projection.exe
  
   You can get:
   
   (a). In the directory 'style_patch', there are images which show representative style patch localization on all shapes. 
   These images are named as 'model index\_patch index\_view index.bmp'.

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
