%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%%    function: val=f_rough( V )                 %%
%%    --------------------------                 %%
%%    V = original radial data vector (norm)     %%
%%                                               %%
%%    output: roughness of vector data           %%
%%                                               %%
%%    Author: Harris Georgiou (c) 2001           %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%


%--------------------------------------------------------------------------------------

function val=f_rough(V, L)
%L=16;
N=length(V);
rs=0;
for i=2:L:length(V),
    ss=0;
    for k=1:L,
        ss=ss+abs(V(i-1)-V(i));
    end;
    rs=rs+ss;
end;
val=rs*(L/N);
