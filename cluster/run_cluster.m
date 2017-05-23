% pre-select patches

startup;
%%
for viewsi=1:views
    
    curpath=fullfile([sPath,vPath,pPath,num2str(viewsi),'\']);
    vname=sprintf('V-%d.mat',viewsi);
    vcurpath=fullfile([path,num2str(orderdata),'\',vname]);
    load(vcurpath);
%     tic;
    parpool('local',4);
    p=gcp('nocreate');
    opts=statset('UseParallel','always');
    [idxk,ceter]=kmeans(V,K,'Options',opts,'emptyaction','drop');
    delete(p);
%     toc;
    pos=find(isnan(ceter(:,1)));
    
    newceter=ceter;
    for i=1:length(pos)
        
        newceter(pos(i),:)=0;
        
    end
    vname=sprintf('kmeans-%d-%d.mat',K,viewsi);
     vdatapath=fullfile([sPath,'\cluster\',num2str(orderdata)]);
    if ~exist(vdatapath,'dir')
        mkdir(vdatapath,'dir');
    end
    vname=fullfile([vdatapath,'\',vname]);
    save(vname,'idxk','ceter','pos','newceter');
     fprintf('%d\n', viewsi);
end
