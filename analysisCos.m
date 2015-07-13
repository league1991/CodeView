clear all;
doc292;


[nDoc, nTerm] = size(docTermMat);

% original result

% raw result
l = sqrt(sum(docTermMat .* docTermMat, 2));
norTermMat = docTermMat ./ repmat(l,[1,nTerm]);
oriCos = norTermMat * norTermMat';
for i=1:nDoc
    oriCos(i,i)=0;
end

% svd result
[U,S,V] = svd(docTermMat,'econ');

for i=15:nDoc
    S(i,i)=0;
end
US = U * S;
l = sqrt(sum(US .* US, 2));
norUS = US ./ repmat(l,[1,nDoc]);
approCos = norUS * norUS';
for i=1:nDoc
    approCos(i,i)=0;
end
figure;
imagesc(approCos);

imagesc([simMat, oriCos, approCos, approCos-simMat])
