% pre-select patches
function []=run_cluster(views,K,HOG_Path,cluster_Path)
    fprintf('start run cluster\n');
    if ~exist(cluster_Path,'dir')
        mkdir(cluster_Path);
    end

    for viewsi=1:views
        vcurpath=fullfile(HOG_Path,sprintf('V-%d.mat',viewsi));
        load(vcurpath);
        [idxk,ceter]=kmeans(V,K,'emptyaction','drop');
        pos=find(isnan(ceter(:,1)));
        newceter=ceter;
        for i=1:length(pos)       
            newceter(pos(i),:)=0;
        end
        vdatapath=fullfile(cluster_Path,sprintf('kmeans-%d-%d.mat',K,viewsi));
        save(vdatapath,'idxk','ceter','pos','newceter');
        fprintf('%d\n', viewsi);
    end
    fprintf('finish run cluster\n');
end