% calculator the images'HOG feature
startup;
%%
for viewsi=1:views
    count=0;
    curpath=fullfile([sPath,vPath,pPath,num2str(viewsi),'\']);
    numpatch=dir(curpath);
    numpatch={numpatch.isdir};
    numpatch=cell2mat(numpatch);
    numpatch=length(find(numpatch==0));
    CT=[];
%     V=zeros(numpatch,1296);     % HOG feature output. the rows of the V is the all patches' number.
    for i=1:models
        for j=1:numb
            tPath=sprintf('%d_%d.bmp',i,j);
            tPath=fullfile([curpath,tPath]);
            if ~exist(tPath,'file')
                CT=[CT;0];
                continue;
            end
            count=count+1;
            CT=[CT;count];
%             fprintf('%d\n', count);
        end
    end
    vname=sprintf('CT-%d-%d.mat',numpatch,viewsi);
    vdatapath=fullfile([path,'\',num2str(orderdata)]);
    if ~exist(vdatapath,'dir')
        mkdir(vdatapath,'dir');
    end
    vname=fullfile([vdatapath,'\',vname]);
    save(vname,'CT');
     fprintf('%d\n', viewsi);
end


