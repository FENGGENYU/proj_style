%% startup
startup;
startup4style;
load([sPath vPath 'labels.mat']);
vdatapath=fullfile([sPath,'\loop\',num2str(orderdata)]);
vname=sprintf('V%d',orderdata);
vname=fullfile([vdatapath,'\',vname]);
load(vname);

%% add labels into the feature maxtrix X
[XL,Y,G,neworder]=makedata(X,labels,pectlbs);
