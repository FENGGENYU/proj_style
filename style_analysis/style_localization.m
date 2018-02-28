%% output style localization index file
function []=style_localization(pslf_Path,K,neworder,views,stylepatch_Path,models,HOG_Path,cluster_Path,kernel_Path,sample_size)

	fprintf('start style_localization\n');
    
    if ~exist(stylepatch_Path,'dir')
        mkdir(stylepatch_Path);
    end
    
	result_name=fullfile(pslf_Path,'result.mat');
	load(result_name);
    L=unique(labels_output);
    GK=length(L);
	for iker=1:views
		vname=sprintf('V-%d.mat',iker);
		vcurpath=fullfile(HOG_Path,vname);
		kname=sprintf('kmeans-%d-%d',K,iker);
		kcurpath=fullfile(cluster_Path,kname);
		load(kcurpath);
		load(vcurpath);
		
        k2po_path = fullfile(kernel_Path,num2str(iker));
		k2po=importdata(fullfile(k2po_path,'k2po.txt'));
		Name{iker,1}=[];
		nameT=[];
		for i=1:GK
			k2p= k2po(styw{iker,i});
			modelsi=find(labels_output==i);
			modelsi=sort(neworder(modelsi));
			
			name=[];
			countname=0;
			for j=1:length(k2p)
                for jj=modelsi'
                    disbest=inf;
                    countname=countname+1;
                    for jjj=1:sample_size
                        mn=CT((jj-1)*sample_size+jjj,1);
                        if(mn==0)
                            continue;
                        end
                        dis=sum((V(mn,:)-newceter(k2p(j),:)).^2);
                        if(dis<disbest)
                            disbest=dis;
                            styname=sprintf('%d_%d.bmp',jj,jjj); 
                        end
                    end
                    name{countname,1}=styname;
                end
			end
			nameT=union(nameT,name);
            if isempty(nameT)
                nameT=[];
            end
		end
		Name{iker,1}=union(Name{iker,1},nameT);
        fprintf('%d\n',iker);
	end

	fstyle=fopen(fullfile(stylepatch_Path,'style_index.txt'),'w');
	for i=1:models
		out_style = zeros(1,sample_size);
		for j=1:sample_size
			pname=sprintf('%d_%d.bmp',i,j);
			count=0;
			for k=1:views          
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
        %pname3=sprintf('%d_%d.bmp',i,index_style(3));
		fprintf(fstyle,'%s\r\n',pname1);
		fprintf(fstyle,'%s\r\n',pname2);
        %fprintf(fstyle,'%s\r\n',pname3);
	end
	fclose(fstyle);

	fprintf('output style index file successfully\n');
end

