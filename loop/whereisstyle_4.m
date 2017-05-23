%% start up
startup;
startup4style;

vdatapath=fullfile([sPath,'\loop\',num2str(orderdata),'\',num2str(pectlbs*100)]);
vname=sprintf('VL%d_result',orderdata);
vname=fullfile([vdatapath,'\',num2str(viewsN),'\',vname]);
load(vname);
patch_num=[803,837,828,790,819,808,781,800,808,774,825,837];
% vname=sprintf('VL%d',orderdata);
% vname=fullfile([vdatapath,'\',vname]);
% load(vname);

fprintf('whereisstyle\n');
ikern=0;
for iker=view_ii
    ikern=ikern+1;
    curpath=fullfile([sPath,vPath,pPath,num2str(iker),'\']);
    numpatch=patch_num(iker);
    vname=sprintf('V-%d-%d.mat',numpatch,iker);
    cname=sprintf('CT-%d-%d.mat',numpatch,iker);
    vcurpath=fullfile([path,num2str(orderdata),'\',vname]);
    ccurpath=fullfile([path,num2str(orderdata),'\',cname]);
    kcurpath=sprintf('kmeans-%d-%d-%d',numpatch,K,iker);
    kcurpath=fullfile([pwd,'\cluster\',num2str(orderdata),'\',kcurpath]);
    load(kcurpath);
    load(vcurpath);
    load(ccurpath);
    
    patchPath{ikern,1}=[sPath,vPath,bPath,num2str(iker),'\ceteridf\'];
    patchName{ikern,1}=importdata(fullfile(patchPath{iker,1},'\Apidfpatchname.txt'));
    k2po{ikern,1}=importdata(fullfile([patchPath{iker,1},'..\k2po.txt']));
     Name{ikern,1}=[];
%      k2p=k2po{iker,1};
%     patchname=patchName{iker,1} ;
    nameT=[];
    for i=1:GK
    patchname=patchName{ikern,1}(sty{ikern,i});
    k2p= k2po{ikern,1}(sty{ikern,i});
    modelsi=find(labels_output==i);
    modelsi=sort(neworder(modelsi));
    
    name=[];
     countname=0;
    for j=1:length(k2p)
        for jj=modelsi'
            disbest=inf;
            countname=countname+1;
            for jjj=1:numb
                mn=CT((jj-1)*numb+jjj,1);
                if(mn==0)
                    continue;
                end
                dis=sum((V(mn,:)-newceter(k2p(j),:)).^2);
                if(dis<disbest)
                    disbest=dis;
                    styname=sprintf('%d_%d.bmp',jj,jjj); 
                end
            end
%             name={name;styname};
            name{countname,1}=styname;
       end
    end
    nameT=union(nameT,name);
    end
   
    Name{ikern,1}=union(Name{ikern,1},nameT);
end

if export == 1
    %
    for i=1:viewsN                  %% export the result of kernel
        newPath=fullfile([patchPath{i,1},'..\filter']);
        newPath=[newPath,num2str(pectlbs*100),'to',num2str(viewsN)];
        if ~exist(newPath,'dir')
            mkdir(newPath,'dir')
        end

        usefulpatchperview=patchName{i,1};
        usefulpatchperview(idxudel{i})=[];
        for j=1:length(usefulpatchperview)

            imPathnew=strcat([newPath,'\',usefulpatchperview{j}]);
            imPathsoure=strcat([patchPath{i,1},usefulpatchperview{j}]);
            pic=imread(imPathsoure);
            imwrite(pic, imPathnew, 'bmp');
        end
    end

%     for i=1:GK              %% export the cluster result
%         newPath=[imagePath,'\',num2str(pectlbs*100),'to',num2str(viewsN),'\'];
%         if ~exist(newPath,'dir')
%             mkdir(newPath,'dir')
%         end
%         newPath=strcat(newPath,num2str(i));
%         if ~exist(newPath,'dir')
%             mkdir(newPath,'dir')
%         end
%     end
%     for j=1:models
%         newPath=[imagePath,'\',num2str(pectlbs*100),'to',num2str(viewsN),'\'];
%         newPath=strcat([newPath,num2str(labels_best(j,1))]);
%         imPathnew=strcat([newPath,'\',imageName{neworder(j,1)}]);
%         imPathsoure=strcat([imagePath,imageName{neworder(j,1)}]);
%         pic=imread(imPathsoure);
%         imwrite(pic, imPathnew, 'bmp');
%     end

%     % export the  style 
% 
%     for i=1:viewsN
%         Name{i,1}=[];
%         patchname=patchName{i,1} ;
%         patchname=patchname(sty{i});
%         k2p=k2po{i,1};
%         k2p= k2p(sty{i});
%         for j=1:length(k2p)
%             name=dir(fullfile(patchPath{i,1},'..\',num2str(k2p(j)),'\'));
%             name={name.name};
%             Name{i,1}=union(Name{i,1},name);
%         end
%     end
end

style_patch_path = fullfile(sPath,'style_patch');
if ~exist(style_patch_path,'dir')
    mkdir(style_patch_path);
end

stylename=[style_patch_path,'\style'];
%stylename=[stylename,num2str(pectlbs*100),'to',num2str(viewsN),'_2.txt'];
stylename=[stylename,'_index.txt'];
fstyle=fopen(stylename,'w');
for i=1:models
    out_style = zeros(1,numb);
    for j=1:numb
        pname=sprintf('%d_%d.bmp',i,j);
        count=0;
        for k=1:viewsN            
           [pos,~]=ismember(Name{k,1},pname);
            pos=find(pos==1);
            if~isempty(pos)
                count=count+1;
            end
        end
        out_style(j) = count;
    end
    [unused,index_style] = sort(out_style,'descend');
    pname1=sprintf('%d_%d.bmp',i,index_style(1));
    pname2=sprintf('%d_%d.bmp',i,index_style(2));
    fprintf(fstyle,'%s\r\n',pname1);
    fprintf(fstyle,'%s\r\n',pname2);
end
fclose(fstyle);

fprintf('output style index file successfully\n');

% fprintf('output style\n');
% stylename=fullfile([sPath,vPath,'\style']);
% stylename=[stylename,num2str(pectlbs*100),'to',num2str(viewsN),'.txt'];
% fstyle=fopen(stylename,'w');
% for i=1:models
%     for j=1:numb
%         pname=sprintf('%d_%d.bmp',i,j);
%         count=0;
%         for k=1:viewsN
%             
%             [pos,~]=ismember(Name{k,1},pname);
%             pos=find(pos==1);
%             if~isempty(pos)
%                 count=count+1;
%             end
%         end
%         if(count>param_2)
%             fprintf(fstyle,'%s\r\n',pname);
%         end
%     end
% end
% fclose(fstyle);


