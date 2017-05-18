# Semi-Supervised-Co-Analysis-of-3D-Shape-Styles-from-Projected-Lines
This is a submission code for SIGGRAPH ASIA 2017

### Intruduction

In this repository, we release code for feature encoding and label-constrainted style clustering.
We've provided a mixed models set which contains 30 models of two style in directory 'all_model'.
Features of all these shapes have already been encoded for simply running our program.

###Installation

MATLAB 2016a and OpenGL should be installed.

### Usage

1. Run Easy_run.m in matlab (Label-constrainted style clustering.). 
   You can get purity of the clustered result and the location file(.txt) about style patches.

2. Run back_projection.exe under fold 'Showstyle_exe'. 
   You can find images which show style patches localization on all shapes in the directory 'style_patch'.

### Results

You can find images which show style patches localization on all shapes in the directory 'style_patch'.

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