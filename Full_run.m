addpath('cluster');
addpath('convolutional');
addpath('PSLF');
addpath('style_analysis');
%% 
startup;
feature_extraction = 0;
if feature_extraction == 1
    %% data preparation
    tic;
    %run_CT(views,models,sample_size,HOG_Path,patch_Path);
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
