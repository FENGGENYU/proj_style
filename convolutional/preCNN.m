startup;
curpath=fullfile([sPath,vPath]);
curpathtxt=fullfile([curpath,'txt\']);
if ~exist(curpathtxt,'dir')
    mkdir(curpathtxt,'dir');
end
curpathimage=fullfile([curpath,'imagesname\']);
if ~exist(curpathimage,'dir')
    mkdir(curpathimage,'dir');
end
for i=1:views
    txtname=sprintf('picname%d.txt',i);
    fid=fopen([curpathtxt,txtname],'w');
    if(i~=views)
        for j=1:Nt
            
            
            if(mod(j,views)==i)
                tempname=sprintf('%d.bmp',j);
                
                fprintf(fid,'%s.txt\r\n',  tempname(1:end-4));
            end
        end
    else
        for j=1:Nt
            if(mod(j,views)==0)
                tempname=sprintf('%d.bmp',j);
                fprintf(fid,'%s.txt\r\n',  tempname(1:end-4));
            end
        end
    end
    fclose(fid);
    fprintf('%d\n', i);
end


for i=1:views
    txtname=sprintf('picname%d.txt',i);
    fid=fopen([curpathimage,txtname],'w');
    dirname=fullfile([curpath,imPath,num2str(i)]);
    if ~exist(dirname,'dir')
        mkdir(dirname,'dir');
    end
    if(i~=views)
        for j=1:Nt
            if(mod(j,views)==i)
                tempname=sprintf('%d.bmp',j);
                imPathsoure=fullfile([curpath,imPath,tempname]);
                imPathnew=strcat([dirname,'\',tempname]);
                
                pic=imread(imPathsoure);
                imwrite(pic, imPathnew, 'bmp');
                fprintf(fid,'%s\r\n',  tempname);
            end
        end
    else
        for j=1:Nt
            
            
            if(mod(j,views)==0)
                tempname=sprintf('%d.bmp',j);
                imPathsoure=fullfile([curpath,imPath,tempname]);
                imPathnew=strcat([dirname,'\',tempname]);
                
                pic=imread(imPathsoure);
                imwrite(pic, imPathnew, 'bmp');
                fprintf(fid,'%s\r\n',  tempname);
            end
            
        end
    end
    fclose(fid);
    fprintf('%d\n', i);
end