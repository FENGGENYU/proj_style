% add the all views' images feature into the matrix X
function []=add_data_to_matrix(lines_Path,kernel_Path,txt_Path,views,models,pslf_Path)
    fprintf('start add_data_to_matrix\n');
    if ~exist(pslf_Path,'dir')
        mkdir(pslf_Path);
    end
    %% add data into X
    X=cell(1,views);
    kk=0;
    for v=1:views
        allname=importdata(fullfile(txt_Path, sprintf('picname%d.txt',v)));
        apidf=importdata(fullfile(kernel_Path,num2str(v),'Apidfpatchname.txt'));
        rows=5*length(apidf);
        V1=zeros(rows,models);
        for i=1:size(V1,2);
            name=allname{i};
            fileID = fopen(fullfile(lines_Path,name),'r');
            formatSpec = '%f';
            A = fscanf(fileID,formatSpec);
            fclose(fileID);
            V1(:,i)=A;
        end
       kk=kk+1;
       X{kk}=V1;
       fprintf('%d\n', v);
    end
   
    vname=fullfile(pslf_Path,'input.mat');
    save(vname,'X');
    fprintf('finish add_data_to_matrix\n');
end
