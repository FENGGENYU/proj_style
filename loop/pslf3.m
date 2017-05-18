function [Vr, U,Uint,V,Vint, W, numIter,tElapsed,object,alpha] = pslf3(X, Y,etapra,betapra,gammapra, U, V,option, W)
% alpha1||X1-U1V1||2+alpha2||X2-U2V2||2+alpha3||X3-U3V3||2+beta(||W'Vlab-Y||2+gamma||W||21)
% s.t. X,U,V>=0.
% X=[Xl,Xul]: nf*ns, ns = nl+nul
% U=[Us,Uc]: nf*nt, nt = nst+nct
% V=[Vs;Vc]=[[Vsl,Vsul];[Vcl,Vcul]]: nt*ns
% Vlab=[Vsl1;Vsl2;Vsl3;Vcl]: 3*nst+nct
% W: (nst*3+nct)*nc
% Y: nc*nl 

% v1.0
% Writtern by Yu Jiang
% 2012.12.20

% v1.1
% make some optimization
% 2012.12.24

% v1.2
% add parameter isSqrt
% 2013.01.5

% v1.3
% renamed pslf (sulf2)
% 2013.03.06

% v2.0
% major revision
% 2013.04.17

% need ('../JYLib/basefun/isIterStop');
% need ('../JYLib/basefun/normalize');

tStart=tic;

nv = length(X); % number of views

optionDefault.maxIter = 600;  %800
optionDefault.minIter = 480; %10
optionDefault.dis = 1;
optionDefault.epsilon=1e-3;
optionDefault.beta = betapra*10^-10; %监督项正则参数1000
optionDefault.gamma = gammapra; %21范正则参数10
optionDefault.alpha = 1/nv*ones(1,nv); %各个view权重
% optionDefault.nctPer = 0.25; %共享topic比例0.1/0.25
optionDefault.nctPer = etapra; %共享topic比例0.1/0.25
optionDefault.nt = 50; %总共的topic
optionDefault.isUNorm =1;
optionDefault.isXNorm =1;
optionDefault.isSqrt =1; %必要为1，V是否开根号更新
optionDefault.typeB = 1; %

if ~exist('option','var')
   option=optionDefault;
else
    option=mergeOption(option,optionDefault);
end

nt = option.nt;
beta = option.beta;
gamma = option.gamma;
alpha = option.alpha;
nctPer = option.nctPer;
isUNorm = option.isUNorm;
isXNorm = option.isXNorm;
maxIter = option.maxIter;
minIter = option.minIter;
epsilon = option.epsilon;
dis = option.dis;
isSqrt = option.isSqrt;
typeB = option.typeB;

if isXNorm %对列(每个样本)归一化
    for i = 1:nv
        %平方和为1，似乎更好
        X{i} = normalize(X{i},0,1);
        %和为1
    % 	X{i} = normalize(X{i},0,0);
    end
end

nf = zeros(1,nv);

for i = 1:nv
    nf(i) = size(X{i},1); %number of i-view feature
end
[nc,nl] = size(Y); 
ns = size(X{1},2);
nul = ns-nl; % number of unlabel samples
nct = round(nt*nctPer); % number of common factor
nst = nt-nct; % number of specific factor 
% nwf = nst*nv+nct; %叠加后的V的维数
nwf = size(Y,1); %叠加后的V的维数

if ~exist('V','var')
    Vlab=[];
    Vcl = rand(nct,nl);
    Vcul = rand(nct,nul);
    Vc = [Vcl,Vcul];
    for i = 1:nv
        Vsl{i} = rand(nst,nl);
        Vsul{i} = rand(nst,nul);
        Vs{i} = [Vsl{i},Vsul{i}];
        
        Vl{i}=[Vsl{i};Vcl];
        Vul{i}=[Vsul{i};Vcul];
        
        V{i} = [Vs{i};Vc];
        
        Vlab=[Vlab;Vsl{i}];
    end
else
    Vc = V{1}(1+nst:end,:);
    Vcl = Vc(:,1:nl);
    Vcul = Vc(:,1+nl:end);
    Vlab=[];
    for i = 1:nv
        Vs{i} = V{i}(1:nst,:);
        Vsl{i} = Vs{i}(:,1:nl);
        Vsul{i} = Vs{i}(:,1+nl:end);
        
        Vl{i}=[Vsl{i};Vcl];
        Vul{i}=[Vsul{i};Vcul];
        Vlab=[Vlab;Vsl{i}];
    end
end
    Vlab=[Vlab;Vcl];

if ~exist('U','var')
    for i = 1:nv
    	U{i} = rand(nf(i),nt);
        if isUNorm %对列归一化
            %和为1，似乎更好
            U{i} = normalize(U{i},0,0);
            %平方和为1
            % 	U{i} = normalize(U{i},0,1);
        end
    end
end

if ~exist('W','var')
    D=eye(nwf);
else
    Wi = sqrt(sum(W.*W,2)+eps);
    d = 0.5./(Wi);
    D = diag(d);
    clear Wi d  
end

for i = 1:nv
    Xl{i} = X{i}(:,1:nl);
	Xul{i} = X{i}(:,(nl+1):end);

	Us{i} = U{i}(:,1:nst);
	Uc{i} = U{i}(:,1+nst:end);

	obj{i}=[];
end

objY=[];
object=[];

Vint=V;
Uint=U;
for j=1:maxIter
    
   	%% update U
    for i = 1:nv
        U{i} = U{i}.*(max(X{i}*V{i}',eps)./max(U{i}*(V{i}*V{i}'),eps));
        if isUNorm %对列归一化
            %和为1，似乎更好
            U{i} = normalize(U{i},0,0);
        %   %平方和为1
        % 	U{i} = normalize(U{i},0,1);
        end
        Us{i} = U{i}(:,1:nst);
        Uc{i} = U{i}(:,1+nst:end);
    end
       
    %% update W
    %21范
%     A=inv(Vlab*Vlab'+gamma*D+1e-4*eye(nwf)); 
%     W = A*Vlab*Y';
    A=inv(Y*Y'+gamma*D+1e-4*eye(nwf)); 
    Wtemp = A*Y*Vlab';
    W=Wtemp';
    Wi = sqrt(sum(Wtemp.*Wtemp,2)+eps);
    d = 0.5./(Wi);
    D = diag(d);
    clear Wi d  
    
%     Wi = sqrt(sum(W.*W,2)+eps);
%     d = 0.5./(Wi);
%     D = diag(d);
%     clear Wi d    
    %F范
%     A=inv(Vlab*Vlab'+gamma*eye(nwf)); 
%     W = A*Vlab*Y'; 
    
    %% update V
    
    if typeB ==1        
        Wz = (abs(W)+W)/2;
        Wf = (abs(W)-W)/2;
        WW = W*W';
        WWz = (abs(WW)+WW)/2;
        WWf = (abs(WW)-WW)/2;
        temp = (WWz*Vlab+Wf*Y);
        Bz = beta*(WWz*Vlab+Wf*Y);
        Bf = beta*(WWf*Vlab+Wz*Y);
%          Bz = beta*(Vlab+Wf*Y);
%          Bf = beta*(Vlab+Wz*Y);
    else
        %W不参与迭代
        B = -beta*W*Y;
        Bz = (abs(B)+B)/2;
        Bf = (abs(B)-B)/2;    
    end
    
%     B = beta*W*(W'*Vlab-Y);
%     Bz = (abs(B)+B)/2;
%     Bf = (abs(B)-B)/2;

%     WWV = beta*W*W'*Vlab;
%     WY = beta*W*Y;
%     WWVz = (abs(WWV)+WWV)/2;
%     WWVf = (abs(WWV)-WWV)/2;
%     WYz = (abs(WY)+WY)/2;
%     WYf = (abs(WY)-WY)/2;
%     Bz = WWVz+WYf;
%     Bf = WWVf+WYz;

    
    newVlab=[];
    sumUXl=zeros(nct,nl);
    sumUUVl=zeros(nct,nl);
    sumUXul=zeros(nct,nul);
    sumUUVul=zeros(nct,nul);
    for i = 1:nv
        % 更新Vs
        Bzs = Bz(1:nst,:);
        Bz(1:nst,:)=[];
        Bfs = Bf(1:nst,:);
        Bf(1:nst,:)=[];
        
        if isSqrt
            Vsl{i} = Vsl{i}.*sqrt(max(alpha(i)*Us{i}'*Xl{i}+Bfs,eps)...
                ./max(alpha(i)*Us{i}'*U{i}*Vl{i}+Bzs,eps));
%             Vsl{i} = Vsl{i}.*sqrt(max(alpha(i)*Us{i}'*Xl{i}+Bzs,eps)...
%                 ./max(alpha(i)*Us{i}'*U{i}*Vl{i}+Bfs,eps));
            Vsul{i} = Vsul{i}.*sqrt(max(alpha(i)*Us{i}'*Xul{i},eps)...
                ./max(alpha(i)*Us{i}'*U{i}*Vul{i},eps));
        else
            Vsl{i} = Vsl{i}.*(max(alpha(i)*Us{i}'*Xl{i}+Bfs,eps)...
                ./max(alpha(i)*Us{i}'*U{i}*Vl{i}+Bzs,eps));
            Vsul{i} = Vsul{i}.*(max(alpha(i)*Us{i}'*Xul{i},eps)...
                ./max(alpha(i)*Us{i}'*U{i}*Vul{i},eps)); 
        end
        
        Vs{i} = [Vsl{i},Vsul{i}];
        
        newVlab=[newVlab;Vsl{i}];
        
        % 计算更新Vc相关值
        sumUXl=sumUXl+alpha(i)*Uc{i}'*Xl{i};
        sumUUVl=sumUUVl+alpha(i)*Uc{i}'*U{i}*Vl{i};
        sumUXul=sumUXul+alpha(i)*Uc{i}'*Xul{i};
        sumUUVul=sumUUVul+alpha(i)*Uc{i}'*U{i}*Vul{i};
    end
    
    % 更新Vc
    Bzc = Bz;
    Bfc = Bf;
    
     if isSqrt
        Vcl = Vcl.*sqrt(max(sumUXl+Bfc,eps)./max(sumUUVl+Bzc,eps));
%          Vcl = Vcl.*sqrt(max(sumUXl+Bzc,eps)./max(sumUUVl+Bfc,eps));
         Vcul = Vcul.*sqrt(max(sumUXul,eps)./max(sumUUVul,eps));
     else
        Vcl = Vcl.*(max(sumUXl+Bfc,eps)./max(sumUUVl+Bzc,eps));
         Vcul = Vcul.*(max(sumUXul,eps)./max(sumUUVul,eps));
     end
    
    Vc = [Vcl,Vcul]; 
    
    Vlab=[newVlab;Vcl];
        
    %% 计算目标函数   
    objRec(j) = 0;
    for i =1:nv
        V{i}=[Vs{i};Vc];
        Vl{i}=[Vsl{i};Vcl];
        Vul{i}=[Vsul{i};Vcul];
        obj{i}(j) = norm(X{i}-U{i}*V{i},'fro')^2;
        objRec(j) = objRec(j) + alpha(i)*obj{i}(j);
    end
%     objY(j) = norm((W'*Vlab-Y),'fro')^2+gamma*trace(W'*D*W);
   objY(j) = norm((W*Y-Vlab),'fro')^2 +gamma*trace(W*D*W');
    object(j) =objRec(j)+beta*objY(j);
    
%     fprintf('epoch %3i  objRec %10.4f   objY %10.4i   object %10.4i\n', ...
%               j, objRec(j), objY(j),object(j)); 
  
    if mod(j,5)==0 || j==maxIter
        isStop = isIterStop(object, epsilon, j, maxIter, dis, minIter);
        if isStop
            break;            
        end
    end
end



Vr=[];
for i =1:nv
    Vr = [Vr;Vs{i}];
end
Vr = [Vr;Vc];

numIter = j;
tElapsed=toc(tStart);
end