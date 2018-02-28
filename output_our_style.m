addpath('style_analysis');
%% before run ,U should set the kmeans cluster No. 'K' in the file "startup.m"
startup;
%% style analysis
tic;
KMname=fullfile(kernel_Path,'kernelModel.mat');
load(KMname);
models = size(kernelModel{1},2);
neworder=reshape(1:models,models,1);

style_localization_select(pslf_Path,K,neworder,views,stylepatch_Path,models,HOG_Path,cluster_Path,kernel_Path,sample_size,dataname);
toc;