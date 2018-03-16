function output_our_style(dname)
addpath('style_analysis');
%% before run ,U should set the kmeans cluster No. 'K' in the file "startup.m"
%% 
dataname=dname; %data set name you are going to run
%select from
%demo
%hu_building, hu_car, hu_drinkware, hu_furniture, hu_leg
%our_building, our_car, our_furniture
%%
%% start up parameters
K=50;    % kmeans's cluster NO.
run_demo=0;
%% add path
Data_Path='data';
Data_Path=fullfile(Data_Path,dataname);
lines_Path=fullfile(Data_Path,'3dlines');
patch_Path=fullfile(Data_Path,'patch');
kernel_Path=fullfile(Data_Path,'kernel');
HOG_Path=fullfile(Data_Path,'hog');
cluster_Path=fullfile(Data_Path,'cluster');
txt_Path=fullfile(Data_Path,'txt');
imgname_Path=fullfile(Data_Path,'imagesname');
pslf_Path=fullfile(Data_Path,'pslf');
label_Path=fullfile(Data_Path,'labels.mat');
stylepatch_Path=fullfile(Data_Path,'style_patch');

%% feature parameters
sample_size=30;
views=12;
pcsize = 48;   %  to accomplish  the calculator of the images HOG .

%% set the percent of the semi-supervised
pectlbs=1;
label_or_constraint=0;

%% calculate number of shapes
FILES_TO_PROCESS        = '*.bmp';
IMGS = dir(fullfile(lines_Path, FILES_TO_PROCESS));
IMGS = {IMGS.name};
Nt=length(IMGS);
models = Nt/views;

%% style analysis
tic;
KMname=fullfile(kernel_Path,'kernelModel.mat');
load(KMname);
models = size(kernelModel{1},2);
neworder=reshape(1:models,models,1);

style_localization_select(pslf_Path,K,neworder,views,stylepatch_Path,models,HOG_Path,cluster_Path,kernel_Path,sample_size,dataname);
toc;