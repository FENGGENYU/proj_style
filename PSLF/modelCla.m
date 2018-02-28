function [ModelCla]=modelCla(newcla,clanum,modelsN,K)

%     ModelCla=ones(modelsN,K).*(-1/(modelsN)); 
%     for MCi=1:K
%         MCtemp=find(newcla==MCi);
%         ModelCla(MCtemp,MCi)=1/clanum(MCi,1)-1/(modelsN);
%     end
    ModelCla=zeros(modelsN,K); 
    for MCi=1:K
        MCtemp=find(newcla==MCi);
        ModelCla(modelsN,MCi)=(1/clanum(MCi,1)-1/(modelsN))*-1;
        ModelCla(MCtemp,MCi)=(1/clanum(MCi,1)-1/(modelsN));
    end
end