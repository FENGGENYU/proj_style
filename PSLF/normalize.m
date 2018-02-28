function fea = normalize(fea,row,type)
% if row == 1, normalize each row of fea 
% if row == 0, normalize each column of fea 
% if type == 1, normalize to unit norm 
% if type == 0, normalize to sum = 1 
%
%
%   Written by Deng Cai (dengcai AT gmail.com)
%   Modified by Yu Jiang
%

if ~exist('row','var')
    row = 1;
end

if ~exist('type','var')
    type = 1;
end

if type
    if row
        nSmp = size(fea,1);
        feaNorm = max(1e-14,full(sum(fea.^2,2)));
        fea = spdiags(feaNorm.^-.5,0,nSmp,nSmp)*fea;
    else
        nSmp = size(fea,2);
        feaNorm = max(1e-14,full(sum(fea.^2,1))');
        fea = fea*spdiags(feaNorm.^-.5,0,nSmp,nSmp);
    end

else
    if row
        nFea = size(fea,2);
        feaNorm = max(1e-14,full(sum(fea,2)));
        fea = fea./repmat(feaNorm,1,nFea);
    else
        nFea = size(fea,1);
        feaNorm = max(1e-14,full(sum(fea,1)));
        fea = fea./repmat(feaNorm,nFea,1);
    end
end
            
return;


