% calculator the images'HOG feature
startup;
%%
for viewsi=1:6
    count=0;
    V=[];
    curpath=fullfile([sPath,vPath,pPath,num2str(viewsi),'\']);
    numpatch=dir(curpath);
    numpatch={numpatch.isdir};
    numpatch=cell2mat(numpatch);
    numpatch=length(find(numpatch==0));
%     V=zeros(numpatch,1296);     % HOG feature output. the rows of the V is the all patches' number.
    for i=1:models
        for j=1:numb
            tPath=sprintf('%d_%d.bmp',i,j);
            tPath=fullfile([curpath,tPath]);
            if ~exist(tPath,'file')
                continue;
            end
            
            patch=imread(tPath);
            patch=imresize(patch,[prsize,pcsize]);
            HOG=hogcalculator(patch);
            if sum(HOG) < 25.3356
                delete(tPath);
                continue;
            end
            count=count+1;
            V(count,:)=HOG;
            fprintf('%d\n', count);
        end
    end
    vname=sprintf('V-%d-%d.mat',numpatch,viewsi);
    vdatapath=fullfile([path,'\',num2str(orderdata)]);
    if ~exist(vdatapath,'dir')
        mkdir(vdatapath,'dir');
    end
    vname=fullfile([vdatapath,'\',vname]);
    save(vname,'V');
     fprintf('%d\n', viewsi);
end

