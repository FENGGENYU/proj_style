
function [XL,Y,G,neworder]=make_label_split_data(pslf_Path,pectlbs,label_Path)
   fprintf('start make_label_split_ input data\n');
   load(label_Path);
   vname=fullfile(pslf_Path,'input.mat');
   load(vname);
   L=unique(labels);
   sampleN=[];
 
   %rand get the label
   % for i=1:length(L)
       % classIdxm= find(labels==L(i,1));
       % Ll(i,1)=length(find(labels==L(i,1)));
       % Ll(i,2)=floor(Ll(i,1)*pectlbs);
       % samplen{i,1} = classIdxm( randperm( Ll(i,1),  Ll(i,2)) );
       % sampleN=[sampleN;samplen{i,1}];
   % end
       % sampleN=sort(sampleN);

   %%get the label in order
    for i=1:length(L)
        classIdxm= find(labels==L(i,1));
        Ll(i,1)=length(find(labels==L(i,1)));
        Ll(i,2)=floor(Ll(i,1)*pectlbs);
        %samplen{i,1} = classIdxm(1+Ll(i,2):Ll(i,2)*2,1);
        samplen{i,1} = classIdxm( 1:Ll(i,2),1);
        sampleN=[sampleN;samplen{i,1}];
    end
       sampleN=sort(sampleN);

%       %%get the label in reverse order
%       for i=1:length(L)
%        classIdxm= find(labels==L(i,1));
%        Ll(i,1)=length(find(labels==L(i,1)));
%        Ll(i,2)=floor(Ll(i,1)*pectlbs);
%        samplen{i,1} = classIdxm( Ll(i,1)-Ll(i,2)+1:Ll(i,1),1);
%        sampleN=[sampleN;samplen{i,1}];
%    end
%        sampleN=sort(sampleN);
       
   %% creat labels matrix Y used for semi-PSLF
   Yc=sum(Ll(:,2));
   Y=zeros(length(L),Yc);
   count=0;
   for i=1:length(L)
       count=count+Ll(i,2);
       Y(i,count-Ll(i,2)+1:count)=1;
   end
   %%  creat groundtruth label G
   G=zeros(size(labels));
   G(1:Yc,1)=labels(sampleN,1);
   sampleNn=setdiff(1:length(labels),sampleN);
   G(Yc+1:end,1)=labels(sampleNn,1);
   
   neworder=[sampleN;sampleNn'];
   
   %% creat the feature matrix newX 
  for i = 1:length(X)
      XL{1,i}=[X{1,i}(:,sampleN),X{1,i}(:,sampleNn)];
  end
  fprintf('finish make_label_split_ input data\n');
end