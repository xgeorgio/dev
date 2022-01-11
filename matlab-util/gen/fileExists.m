function flag = fileExists( strPath, strFile )

strPath=sprintf('%s/*.*',strPath);
DL=dir(strPath);
flag=0;
for k=1:length(DL)
    if (strcmp(DL(k).name,strFile)==1)
        flag=1;  break;
    end
end

end
