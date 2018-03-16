function Complete_run(dname, feature_extraction)
addpath('cluster');
addpath('convolutional');
addpath('PSLF');
addpath('style_analysis');
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

%%
if feature_extraction == 1
    %% data preparation
    tic;
    run_CT(views,models,sample_size,HOG_Path,patch_Path);
    run_cluster(views,K,HOG_Path,cluster_Path);
    run_cluster_idf(views,kernel_Path,HOG_Path,cluster_Path,patch_Path,models,sample_size,K);
    preCNN(views,txt_Path,imgname_Path,Nt);
    fprintf('already complete to calculator the cluster,please start CNN by using VS2010 and OpenCV 2.4.8\n');
    toc;
else
    %% style analysis
    tic;
    add_data_to_matrix(lines_Path,kernel_Path,txt_Path,views,models,pslf_Path);
    [XL,Y,G,neworder]=make_label_split_data(pslf_Path,pectlbs,label_Path);
    run_kernel_filter(XL,Y,G,pslf_Path,views,run_demo,kernel_Path,models);
    style_localization(pslf_Path,K,neworder,views,stylepatch_Path,models,HOG_Path,cluster_Path,kernel_Path,sample_size);
    toc;
end
end