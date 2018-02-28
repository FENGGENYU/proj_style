startup;
startup4style;

vdatapath=fullfile([sPath,'\loop\',num2str(orderdata),'\',num2str(pectlbs*100)]);
vname=sprintf('VL%d_result',orderdata);
vname=fullfile([vdatapath,'\',num2str(viewsN),'\',vname]);
load(vname);

model_path = fullfile(sPath,'all_model\');
target_path = fullfile(sPath,'style_cluster_result');
if ~exist(target_path,'dir')
    mkdir(target_path);
end

FILES_TO_PROCESS        = '*.obj';

MODELS = dir(fullfile(model_path, FILES_TO_PROCESS));
MODELS = {MODELS.name};

for i=1: length(MODELS)
    index = find(neworder == i);
    sub_path = fullfile(target_path, num2str(labels_output(index)));
    if ~exist(sub_path,'dir')
        mkdir(sub_path);
    end
    model_name = MODELS{i};
    copyfile([model_path model_name],[sub_path '\' model_name]);
end
fprintf('output style clustering results successfully\n');

