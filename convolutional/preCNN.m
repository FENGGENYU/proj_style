function []=preCNN(views,txt_Path,imgname_Path,Nt)
    fprintf('start preCNN\n');
    if ~exist(txt_Path,'dir')
        mkdir(txt_Path,'dir');
    end

    if ~exist(imgname_Path,'dir')
        mkdir(imgname_Path,'dir');
    end

    for i=1:views
        fid=fopen(fullfile(txt_Path,sprintf('picname%d.txt',i)),'w');
        if(i~=views)
            for j=1:Nt
                if(mod(j,views)==i)
                    fprintf(fid,'%d.txt\r\n',j);
                end
            end
        else
            for j=1:Nt
                if(mod(j,views)==0)
                    fprintf(fid,'%d.txt\r\n',j);
                end
            end
        end
        fclose(fid);
        fprintf('%d\n', i);
    end


    for i=1:views
        fid=fopen(fullfile(imgname_Path,sprintf('picname%d.txt',i)),'w');
        if(i~=views)
            for j=1:Nt
                if(mod(j,views)==i)
                    tempname=sprintf('%d.bmp',j);
                    fprintf(fid,'%s\r\n',tempname);
                end
            end
        else
            for j=1:Nt
                if(mod(j,views)==0)
                    tempname=sprintf('%d.bmp',j);
                    fprintf(fid,'%s\r\n',  tempname);
                end
            end
        end
        fclose(fid);
        fprintf('%d\n', i);
    end
    fprintf('finish preCNN\n');
end