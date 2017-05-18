%% set the start up pra

%% set the percent of the semi-supervised
pectlbs=0.3;
min_th = 0;
%% add path
viewsN=12;                       % the number of the view choose by user
view_ii = [1:12];               % the view's order number choose by user

iternum=1;                      % iteration number                   
param=0.05;                     % ¦Ì£¬parameter of fliter the kernel
param_1=5;                      % parameter of the best reasonable kernel number (scales:2.5-3.5)
param_2=2;             %2         % parameter of the export style number                  (sacale:viewsN/2-viewsN)
stylocN=2;                      % parameter of the style numb each cluster
view_dis_param=10;    %
export=0;

cpath = fullfile(['constraint\' num2str(orderdata) '\']);
imagePath=[sPath,vPath 'BMPImages\1\'];
imageName=importdata(fullfile(sPath,vPath,'imagesname\picname1.txt'));
%% PSLF¡®s pra
iBest=10; %set the init of semi_PSLF
etapra=0.2; 
betapra=0.05; 
lamda=20;
gammapra=10;




   
    


 






