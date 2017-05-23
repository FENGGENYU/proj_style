% output kernel for convoluting
startup;
%% 
for viewi=1:views
    curpath=fullfile([sPath,vPath,pPath,num2str(viewi),'\']);
    vname=sprintf('V-%d.mat',viewi);
    vcurpath=fullfile([path,num2str(orderdata),'\',vname]);
    kcurpath=sprintf('kmeans-%d-%d',K,viewi);
    kcurpath=fullfile([pwd,'\cluster\',num2str(orderdata),'\',kcurpath]);
    load(kcurpath);
    load(vcurpath);
    
    %find the closest patch to be kernel
    patchk=zeros(K,1);
    disk=1./patchk;
    [V_length,unused] = size(V); 
    for ni=1:V_length
        oi=idxk(ni,1);
        dis=sum((newceter(oi,:)-V(ni,:)).^2);
        if(dis<disk(oi,1))
            patchk(oi,1)=ni;
            disk(oi,1)=dis;
        end
    end

    fprintf('writing kernel images\n');
     impathk=strcat([sPath,vPath,bPath,num2str(viewi),'\ceteridf']);

     %write kernel
    if ~exist(impathk,'dir')  
     mkdir(impathk,'dir')
    end
    fk2p=fopen([sPath,vPath,bPath,num2str(viewi),'\k2po.txt'],'w');
    fidai=fopen([impathk,'\Apidfpatchname.txt'],'w');
    count=0;
    cnt=0;
    patchkernel=[];
    for nm=1:models

        for nmi=1:numb
            patchname=sprintf('%d_%d.bmp',nm,nmi); 
            imPathksoure=strcat([curpath,patchname]);
             if ~exist(imPathksoure,'file')

              continue;
             end
            count = count+1;
            pos=find(patchk==count);

            if ~isempty(pos)
                 imPathkn=fullfile(impathk,patchname);
                 pic=imread(imPathksoure);
                 imwrite(pic, imPathkn, 'bmp'); 

                 cnt=cnt+1;
                 patchkernel(cnt,1)=idxk(count,1);
                 fprintf(fidai,'%s\r\n', patchname);
                 fprintf(fk2p,'%d\r\n', patchkernel(cnt,1));
            end

        end
    end     
    fclose(fidai);
    fclose(fk2p);
  
    %%
    fprintf('counting kernel model\n');
    count1=0;
    kernelModel{viewi,1}=ones(length(patchkernel),models)*-1;
    for nn=1:models
        for nni=1:numb
             patchname=sprintf('%d_%d.bmp',nn,nni);
             patchnamepath=fullfile([curpath,patchname]);
            if ~exist(patchnamepath,'file')
                continue;
            end
            count1=count1+1;
            cla=idxk(count1,1);

             posk=find(patchkernel==cla);
             kernelModel{viewi,1}(posk,nn)=1;
        end
    end
    fprintf('%d\n', viewi);
    
end

KMname=fullfile([sPath,'\cluster\',num2str(orderdata),'\kernelModel.mat']);
save(KMname,'kernelModel');
