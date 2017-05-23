function isStop = isIterStop(obj, epsilon, iter, maxIter, isFig, minIter)

isStop = 0;

if ~exist('minIter','var')
    minIter = 5;
end
if ~exist('isFig','var')
    isFig = 0;
end
if ~exist('maxIter','var')
    maxIter = 50;
end

if length(obj)>=minIter
    if iter>=maxIter
        s=sprintf('max iteration! \n # of iterations is %0.0d. \n The final residual is %0.4d.',iter,obj(end));
        isStop = 1;
    end        
    if obj(end)<epsilon
        s=sprintf('O-value lower than the threshold! \n # of iterations is %0.0d. \n The final residual is %0.4d.',iter,obj(end));
        isStop = 1;
    end
    if obj(end-1)-obj(end)<epsilon
        s=sprintf('D-value lower than the threshold! \n # of iterations is %0.0d. \n The final residual is %0.4d.',iter,obj(end));
        isStop = 1;
    end
end

% if isFig && isStop
%     disp(s);
%     figure;
%     plot(obj(2:end));
%     drawnow;
% end
        


end

