function [miniterK]=ccdis(idxmax,Vr)

%% calculator distance within the class
K=length(unique(idxmax));
minK=zeros(K,1);
for i=1:K
    uniquetemp=unique(idxmax);
    temp=find(idxmax==uniquetemp(i));

    countmin=0;
    if length(temp)>0
        for k=temp'
            countmin=countmin+1;
            for kk=temp(countmin:end,1)'
                
                minK(i,1)=minK(i,1)+sum((Vr(:,k)-Vr(:,kk)).^2)^0.5;
            end
        end
        if(length(temp)==1)
            minK(i,1)=0;
        else
            minK(i,1)= minK(i,1)/(length(temp)-1);
        end
    end
    
end
miniterK=sum(minK);
    
%% claculator the distance  between class
% maxK=zeros(K*(K-1)/2,1);
% count=0;
% for i=1:K-1
%     
%     for j=i+1:K
%         count=count+1;
%         tempc1=find(idxmax==i);
%          tempc2=find(idxmax==j);
%          for c1=tempc1'
%              for c2=tempc2'
%                 maxK(count,1)=maxK(count,1)+sum((Vr(:,c1)-Vr(:,c2)).^2)^0.5; 
%              end
%          end
%          maxK(count,1)=maxK(count,1)*((length(tempc1)+length(tempc2)))/(length(tempc1)*length(tempc2));
%     end
% end
% maxiterK=sum(maxK)/(K-1);
% 
% miniterK=maxiterK/miniterK;
