%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%    function: val=f_zccount( V )               %%
%%    ----------------------------               %%
%%    V = original radial data vector (norm)     %%
%%                                               %%
%%    output: zero-crossings of vector data      %%
%%                                               %%
%%    Author: Harris Georgiou (c) 2001           %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------------------------------------------------------------------------------------

function val=f_zccount(V)
m=mean(V);
c=0;
for i=2:length(V),
    zcv=(V(i-1)-m)*(V(i)-m);
    if (zcv<=0), c=c+1; end;
end;
val=c;
