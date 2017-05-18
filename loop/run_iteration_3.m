%% startup
%adddata_run_1;
%creatdata_run_2;

vdatapath=fullfile([sPath,'\loop\',num2str(orderdata),'\',num2str(pectlbs*100)]);
vname=sprintf('VL%d',orderdata);
vname=fullfile([vdatapath,'\',vname]);
load(vname);
newX=XL;
newX=newX(:,view_ii);


Uusei=Uuse;
Vusei=Vuse;
Uusei=Uusei(:,view_ii);
Vusei=Vusei(:,view_ii);

newX=XL;
newX=newX(:,view_ii);

miniterK(1,1)=inf;              % the compactness
idxudeldel=[];
puriT=[];                        % the purity
puriT_te=[];
puriT_tr=[];


%%   find kernel belong to which models
KMname=fullfile([sPath,'\cluster\',num2str(orderdata),'\kernelModel.mat']);
load(KMname);
kernelModel=kernelModel(view_ii,:);


%%  start iteration
for iter=1:iternum
    [Vr, U,Uless,V,Vless, W, numIter,tElapsed,object,alpha] = pslf3(newX,Y,etapra,betapra,gammapra,Uusei,Vusei);
    newY = W'*Vr;
    for i=1:models
        pos=find(newY(:,i)==max(newY(:,i)));
        labelc(i,1)=pos(1,1);
    end
    
    puri=1-cluster_er(labelc,G);
    miniterK(iter,1)=ccdis(labelc,Vr);

    puriT=[puriT;puri];
    
    VrT{iter}=Vr;
    idxKT(:,iter)=labelc;

    %% start to filter kernel
    %% calculator the number per class based on the cluster result
    GK=size(Y,1);
    clanum=zeros(GK,1);
    for i=1:GK
        clanum(i,1)=length(find(labelc==i));
    end
    %calculate wli, n*k, matrix wli=wil= 1/nl - 1/n, i belong to l
    ModelCla=modelCla(labelc,clanum,models,GK);
    %% calculator kernel-cluster support
    shachupatch=cell(viewsN,1);
    stylelocadelete=cell(viewsN,1);
    for  viewii=1:viewsN
        shachupatch{viewii,1}=kernelModel{viewii,1}*ModelCla;
        stylelocadelete{viewii,1}=shachupatch{viewii,1};
        shachupatch{viewii,1}=abs(shachupatch{viewii,1});
    end
    %% threshold of fliter the kernel
    trehold=(models*param)./clanum;

    %% output the kernel next round
    for treholdi=1:viewsN
        for  sorti=1:GK
            styloctemp=sort(stylelocadelete{treholdi,1}(:,sorti),'descend');
            styloctemp=styloctemp(stylocN);
            styloc{treholdi,sorti}=find(stylelocadelete{treholdi,1}(:,sorti)>=styloctemp);
            stywei{treholdi,sorti}=stylelocadelete{treholdi,1}(styloc{treholdi,sorti},sorti);
        end
    end

    stylocff{iter,1}=styloc;
    styweiff{iter,1}=stywei;
    for treholdi=1:viewsN
        for  sorti=1:GK
            loca{treholdi,sorti}=find(shachupatch{treholdi,1}(:,sorti)>trehold(sorti,1));
        end
    end
    for treholdi=1:viewsN
        idxuse=[];
        for uni=1:GK
            idxuse=union(idxuse,loca{treholdi,uni});
        end
        idxdeltemp=zeros(size(XL{1,view_ii(treholdi)},1)/5,1);
        idxdeltemp(idxuse,1)=1;
        idxudel{treholdi}=find(idxdeltemp==0);
    end


    %% out the new X feature matrix
    newX=XL;
    Uusei=Uuse;
    newX=newX(:,view_ii);
    Uusei=Uusei(:,view_ii);

    for k=1:viewsN
        count=0;
        for i=idxudel{k}'
            newX{k}((i-count-1)*5+1:(i-count)*5,:)=[];
            count=count+1;

        end
    end
    for k=1:viewsN
        count=0;
        for i=idxudel{k}'
            Uusei{k}((i-count-1)*5+1:(i-count)*5,:)=[];
            count=count+1;
        end
    end
    idxudeldel{iter,1}=idxudel;
end

 posbest=find(puriT==max(puriT));
if(length(posbest)==1)
    labels_best=idxKT(:,posbest);
     if posbest>1
    idxudel=idxudeldel{posbest-1,1};
    sty = stylocff{posbest-1,1};
    styw = styweiff{posbest-1,1};
     else
     idxudel=idxudeldel{posbest,1};
      sty = stylocff{posbest,1};
      styw = styweiff{posbest,1};
     end
else
    posbest=posbest(2,1);
    labels_best=idxKT(:,posbest);
    idxudel=idxudeldel{posbest-1,1};
     sty = stylocff{posbest-1,1};
     styw = styweiff{posbest-1,1};
end
count=0;
for i=1:viewsN
    count=count+length(idxudel{i});
end
count=K-count/viewsN;
    
vname=sprintf('VL%d_bestresult',orderdata);
vname=fullfile([vdatapath,'\',num2str(viewsN),'\',vname]);
if ~exist([vdatapath,'\',num2str(viewsN)],'dir')
    mkdir([vdatapath,'\',num2str(viewsN)],'dir');
end
fprintf('final purity of cluster result.---> %2.2f%%\n',puri*100);
save(vname,'puriT','posbest','param','param_1','labels_best','idxudel','GK','count','XL','Y','G','neworder','sty','styw');
%whereisstyle_4;
%selectview;

