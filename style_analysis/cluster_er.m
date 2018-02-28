% Computes the error of the clustering. The label of each cluster is
% obtained via a majority vote.
% The first argument is predicted labels  N*1 and the second one is the ground truth N*1!
function [error, u_reallabels,votes] = cluster_er(u,Y)


    labels=unique(u);
    error=0;
    
    u_reallabels=zeros(size(u));
    reallabels=unique(Y);
    votes=zeros(size(reallabels,1),size(labels,1));
    
    
    for k=1:size(labels,1)
        % extract indices of current cluster
        ukIndex=u==labels(k);

        % extract real labels of current cluster
        Yk=Y(ukIndex);

        % perform majority vote
        currentvotes=zeros(size(reallabels,1),1);
        for l=1:size(reallabels,1)
            currentvotes(l) = sum(Yk==reallabels(l));
        end
        ind=find(currentvotes==max(currentvotes));

        votes(:,k)=currentvotes;
        
        % relabel u
        u_reallabels(ukIndex)=reallabels(ind(1));
        
        % compute error
        currentError= sum(Yk~=reallabels(ind(1)));
        error=error + currentError;
    end
    error=error/size(u,1);
end