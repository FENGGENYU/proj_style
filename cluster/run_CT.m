% calculator the images'HOG feature
%because some blank patch are delete, so mark them in CT ARRAY
%%
function []=run_CT(views,models,sample_size,HOG_Path,patch_Path)
    if ~exist(HOG_Path,'dir')
        mkdir(HOG_Path);
    end

    for viewsi=1:views
        count=0;
        V=[];
        CT=[];
        curpath=fullfile(patch_Path,num2str(viewsi));
        for i=1:models
            for j=1:sample_size
                tPath=sprintf('%d_%d.txt',i,j);
                tPath=fullfile(curpath,tPath);
                if ~exist(tPath,'file')
                    CT=[CT;0];
                    continue;
                end
                fseed=fopen(tPath,'r');
                formatSpec = '%f';
                HOG = fscanf(fseed,formatSpec);
                fclose(fseed);
                if sum(HOG) <25
                    delete(tPath);
                    delete(fullfile(curpath,sprintf('%d_%d.bmp',i,j)));
                    CT=[CT;0];
                    continue;
                end
                count=count+1;
                V(count,:)=HOG';
                CT=[CT;count];
            end
            fprintf('view:%d  model:%d\n',viewsi, i);
        end
        vname=fullfile(HOG_Path,sprintf('V-%d.mat',viewsi));
        save(vname,'V','CT');
        fprintf('%d\n', viewsi);
    end
end