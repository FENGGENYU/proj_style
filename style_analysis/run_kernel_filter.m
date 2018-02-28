%Run pslf and kernel filter
function []=run_kernel_filter(XL,Y,G,pslf_Path,views,run_demo,kernel_Path,models)
	fprintf('start run kernel filter\n');

	etapra=0.2; % common proportion
	betapra=0.05;
	gammapra=10;
    view_ii = [1:views];           % the view's order number choose by user
    iternum=10;                      % max iteration number                   
    param=0.06-models*0.0001;                     % ¦Ì£¬parameter of fliter the kernel
    %stylocN=3;                      % parameter of the style numb each cluster
    
	idxudeldel=[];
	puriT=[];                       % the purity of all data set
    
	if run_demo == 1
		vdatapath=fullfile(pslf_Path,'result.mat');
		load(vdatapath);
		newX=newX(:,view_ii);
		Uusei=U(:,view_ii);
		Vusei=V(:,view_ii);
	else
		newX=XL;
        KMname=fullfile(kernel_Path,'kernelModel.mat');
        load(KMname);
	end

	%%   find kernel belong to which models
    newX=newX(:,view_ii);
	kernelModel=kernelModel(view_ii,:);

    GK=size(Y,1);
    for i=1:views
        remain_kernel{i,1} = [1:size(kernelModel{i},1)];
        for uni=1:GK
            styw{i,uni} = [1:size(kernelModel{i},1)];
        end
    end
	%%  start iteration
    for iter=1:iternum
        if run_demo == 1
            [Vr, U,Uless,V,Vless, W] = pslf(newX,Y,etapra,betapra,gammapra,Uusei,Vusei);
        else
            [Vr, U,Uless,V,Vless, W] = pslf(newX,Y,etapra,betapra,gammapra);
        end
        newY = W'*Vr;
        for i=1:models
            pos=find(newY(:,i)==max(newY(:,i)));
            labelc(i,1)=pos(1,1);
        end
        puri=1-cluster_er(labelc,G);
        puriT=[puriT;puri];
        VrT{iter}=Vr;
        idxKT(:,iter)=labelc;
        %% start to filter kernel
        %% calculator the number per class based on the cluster result
        
        clanum=zeros(GK,1);
        for i=1:GK
            clanum(i,1)=length(find(labelc==i));
        end
        %calculate wli, n*k, matrix wli=wil= 1/nl - 1/n, i belong to l
        ModelCla=modelCla(labelc,clanum,models,GK);
        %% calculator kernel-cluster support
        shachupatch=cell(views,1);
        stylelocadelete=cell(views,1);
        for  viewii=1:views
            shachupatch{viewii,1}=kernelModel{viewii,1}*ModelCla;
            stylelocadelete{viewii,1}=shachupatch{viewii,1};
            %shachupatch{viewii,1}=abs(shachupatch{viewii,1});
        end
        %% threshold of fliter the kernel
        trehold=(models*param)./clanum;

        %% filter style kernel kernel next round
%         for treholdi=1:views
%             for  sorti=1:GK
%                 styloctemp=sort(stylelocadelete{treholdi,1}(:,sorti),'descend');
%                 styloctemp=styloctemp(stylocN);
%                 styloc{treholdi,sorti}=find(stylelocadelete{treholdi,1}(:,sorti)>=styloctemp);
%                 stywei{treholdi,sorti}=stylelocadelete{treholdi,1}(styloc{treholdi,sorti},sorti);
%             end
%         end
% 
%         stylocff{iter,1}=styloc;
%         styweiff{iter,1}=stywei;
        for treholdi=1:views
            for  sorti=1:GK
                loca{treholdi,sorti}=find(shachupatch{treholdi,1}(:,sorti)>trehold(sorti,1));
            end
        end
        for treholdi=1:views
            idxuse=[];
            for uni=1:GK
                idxuse=union(idxuse,loca{treholdi,uni});
                styw{treholdi,uni} = remain_kernel{treholdi,1}(loca{treholdi,uni});
            end
            idxdeltemp=zeros(size(newX{1,view_ii(treholdi)},1)/5,1);
            idxdeltemp(idxuse,1)=1;
            remain_kernel{treholdi,1} = remain_kernel{treholdi,1}(idxuse);
            idxudel{treholdi}=find(idxdeltemp==0);
        end


        %% out the new X feature matrix
        %newX=XL;
        newX=newX(:,view_ii);
        for k=1:views
            count=0;
            for i=idxudel{k}'
                newX{k}((i-count-1)*5+1:(i-count)*5,:)=[];
                kernelModel{k,1}(i-count,:)=[];
                count=count+1;
            end
        end
        if run_demo == 1
            %Uusei=Uuse;
            Uusei=Uusei(:,view_ii);
            for k=1:views
                count=0;
                for i=idxudel{k}'
                    Uusei{k}((i-count-1)*5+1:(i-count)*5,:)=[];
                    count=count+1;
                end
            end
        end
        
        idxudeldel{iter,1}=idxudel;

        delta_kernel=0;
        for k=1:views
            delta_kernel = delta_kernel + length(idxudeldel{iter,1}{k});
        end
        if iter > 1
            delta_puri = puriT(iter) - puriT(iter-1);
        else
            %Ucopy = U;
            %Wcopy = W;
            delta_puri = puriT(iter);
        end

        fprintf('puri: %f, delta_puri: %f, delta_kernel: %d \n',puri,delta_puri,delta_kernel);

        if abs(delta_puri) < 0.03 && delta_kernel < 36
            labels_output=labelc;
            break;
        end
    end
    if ~exist(pslf_Path,'dir')
        mkdir(pslf_Path,'dir');
    end
	vname=fullfile(pslf_Path,'result.mat');
    %U = Ucopy;
    %W = Wcopy;
	save(vname,'styw','labels_output','U','W','Vr','kernelModel','newX');
	fprintf('finish run kernel filter\n');
end

