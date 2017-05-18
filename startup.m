%% start up
clear;
K=50;    % kmeans's cluster NO.
orderdata=24;
models=30;
%% add path
vPath='\randpatch\models\';
vPath=fullfile([vPath,num2str(orderdata),'\']);
sPath=pwd;
imPath='BMPImages\';
pPath='BMPpatch\';
bPath='BMPK\';
path=fullfile([pwd,'\hogfeature\']);
numb=30; %patch num?
views=12;

prsize = 48;          % 'cause we have two different size image, we have to resize them to a common size
pcsize = 48;   %  to accomplish  the calculator of the images HOG .
if ~exist([sPath,vPath,'\nscale.txt'],'file')
fnscales=fopen([sPath,vPath,'nscale.txt'],'w');
fprintf(fnscales,'%d\r\n',  prsize);
fclose(fnscales);
end

if ~exist([sPath,vPath,'\views.txt'],'file')
fviews=fopen([sPath,vPath,'views.txt'],'w');
fprintf(fviews,'%d\r\n',  views);
fclose(fviews);
end
    


 






