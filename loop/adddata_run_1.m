% add the all views' images feature into the matrix X
%% start up
startup;

%% add data into X
X=cell(1,views);
kk=0;
for v=1:views
    picnamepathtemp=[sPath vPath 'txt\'];
    picnamepath=sprintf('picname%d.txt',v);
    picnamepath=strcat(picnamepathtemp,picnamepath);
    allname=importdata(picnamepath);
    apidf=importdata(fullfile(sPath,vPath,'BMPK\',num2str(v),'\ceteridf\Apidfpatchname.txt'));
    %apidf=importdata(fullfile(sPath,vPath,'BMPK\','ceteridf\Apidfpatchname.txt'));
    rows=5*length(apidf);
    V1=zeros(rows,models);
    for i=1:size(V1,2);
        name=allname{i};
        claname=sprintf('%d\\',v);
        claname=strcat([sPath,vPath,'BMPImages\',claname]);
        A = importdata(fullfile(claname,name));
        V1(:,i)=A;
    end
    kk=kk+1;
    X{kk}=V1;
   fprintf('%d\n', v);
end
vname=sprintf('V%d',orderdata);
vdatapath=fullfile([sPath,'\loop\',num2str(orderdata)]);
    if ~exist(vdatapath,'dir')
        mkdir(vdatapath,'dir');
    end
    vname=fullfile([vdatapath,'\',vname]);
save(vname,'X');

