% load the V to cluster
%% start up
startup;
%%
for viewsi=1:views
    
    curpath=fullfile([sPath,vPath,pPath,num2str(viewsi),'\']);
    numpatch=dir(curpath);
    numpatch={numpatch.isdir};
    numpatch=cell2mat(numpatch);
    numpatch=length(find(numpatch==0));
    vname=sprintf('V-%d-%d.mat',numpatch,viewsi);
    vcurpath=fullfile([path,num2str(orderdata),'\',vname]);
    load(vcurpath);
%     tic;
    matlabpool  open;
    opts=statset('UseParallel','always');
    [idxk,ceter]=kmeans(V,K,'Options',opts,'emptyaction','drop');
    matlabpool close;
%     toc;
    pos=find(isnan(ceter(:,1)));
    
    newceter=ceter;
    for i=1:length(pos)
        
        newceter(pos(i),:)=0;
        
    end
    vname=sprintf('kmeans-%d-%d-%d.mat',numpatch,K,viewsi);
     vdatapath=fullfile([sPath,'\cluster\',num2str(orderdata)]);
    if ~exist(vdatapath,'dir')
        mkdir(vdatapath,'dir');
    end
    vname=fullfile([vdatapath,'\',vname]);
    save(vname,'idxk','ceter','pos','newceter');
     fprintf('%d\n', viewsi);
end
