function optionFinal=mergeOption(option,optionDefault)
% Merge two struct options into one struct
% Usage:
% optionFinal=mergeOption(option,optionDefault)
% option: struct
% optionDefault: struct
% Contact Information:
% Yifeng Li
% University of Windsor
% li11112c@uwindsor.ca; yifeng.li.cn@gmail.com
% Mar. 18, 2011

optionFinal=optionDefault;
if isempty(option)
    return;
end
names=fieldnames(option);
for i=1:numel(names)
    optionFinal=setfield(optionFinal,names{i},getfield(option,names{i}));
end
end