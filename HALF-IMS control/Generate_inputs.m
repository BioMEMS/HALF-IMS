% © The Regents of the University of California, Davis campus, 2019. This
% material is available as open source for research and personal use under
% a Creative Commons Attribution-NonCommercial-NoDerivatives 4.0
% International Public License
% (https://creativecommons.org/licenses/by-nc-nd/4.0/).

clc
clear

VS_start=0;
VS_end=35;
VS_step=.001;
VS=VS_start:VS_step:VS_end;
VStb=[VS fliplr(VS)];
num_S=length(VS);
num_Stb=length(VStb);



VL_start=0;
VL_end=9;
VL_step=.1;
VL=VL_start:VL_step:VL_end;
VLtb=[VL fliplr(VL)];
num_L=length(VL);
num_Ltb=length(VLtb);

sl=16/6;
ss=8001;

VS_range=[linspace(-4,4,ss)]';
VL_range=[linspace(-2,2,ss)]';

round=.001;

V_inputs=zeros(num_S*num_L,2);

V=input('sweep VS step VL (1) or sweep VL step VS (2)')
R=input('straight (1) or there and back (2)')
s=input('slope on (1) or slope off (2)')

if V==1 & R==1 & s==2
    
    
    
    for j=1:num_L
        V_inputs((j-1)*num_S+1:(j)*num_S,1) = [VS]';
        V_inputs((j-1)*num_S+1:(j)*num_S,2) = -[VL(j)]';
        
    end
    
elseif V==1 & R==2 & s==2
    for j=1:num_L
        V_inputs((j-1)*num_S+1:(j)*num_S,1) = [VS]';
        V_inputs((j-1)*num_S+1:(j)*num_S,2) = -[VL(j)]';
        
    end
    for j=2:2:num_L
        V_inputs((j-1)*num_S+1:(j)*num_S,1) = [fliplr(VS)]';
        
    end
    
elseif V==2 & R==1 & s==2
    for j=1:num_S
        V_inputs((j-1)*num_L+1:(j)*num_L,1) = [VS(j)]';
        V_inputs((j-1)*num_L+1:(j)*num_L,2) = -[VL]';
        
    end
    
    
elseif V==2 && R==2 & s==2
    for j=1:num_S
        V_inputs((j-1)*num_L+1:(j)*num_L,1) = [VS(j)]';
        V_inputs((j-1)*num_L+1:(j)*num_L,2) = [VL]';
        
    end
    
    for        j=2:2:num_S
        V_inputs((j-1)*num_L+1:(j)*num_L,2) = -[fliplr(VL)]';
        
    end
    
elseif V==1 && R==1 & s==1
%     VL=[7.5:.01:10]';
    VS=zeros(length(VL)*ss,2);
    for k=1:length(VL)
        
        V_inputs((k-1)*ss+1:k*ss,1)=[VL(k)]';
        
        V_inputs((k-1)*ss+1:k*ss,2)=[floor(VL(k)*sl/round)*round]';
        V_inputs((k-1)*ss+1:k*ss,2)=-(V_inputs((k-1)*ss+1:k*ss,2)+VS_range);
    end
    
    
elseif V==2 && R==1 & s==1
    % V=2
%     VS=[20:.1:29]';
    VL=zeros(length(VS)*ss,2);
    for k=1:length(VS)
        
        V_inputs((k-1)*ss+1:k*ss,2)=-[VS(k)]';
        
        V_inputs((k-1)*ss+1:k*ss,1)=[floor(VS(k)/sl/round)*round]';
        V_inputs((k-1)*ss+1:k*ss,1)=V_inputs((k-1)*ss+1:k*ss,1)+VL_range;
    end
    
    
elseif V==1 && R==2 & s==1
    % VL=[7.5:.1:10]';
    VS=zeros(length(VL)*ss,2);
    for k=1:length(VL)
        
        V_inputs((k-1)*ss+1:k*ss,1)=[VL(k)]';
        
        V_inputs((k-1)*ss+1:k*ss,2)=[floor(VL(k)*sl/round)*round]';
        V_inputs((k-1)*ss+1:k*ss,2)=V_inputs((k-1)*ss+1:k*ss,2)+VS_range;
    end
    V_inputs2=V_inputs;
    for        j=2:2:length(VL)
        V_inputs2((j-1)*ss+1:j*ss,2) = -flipud(V_inputs((j-1)*ss+1:j*ss,2));
        
    end
    V_inputs=V_inputs2;
    
elseif V==2 && R==2 & s==1
    % V=2
    % VS=[20:.1:29]';
    VL=zeros(length(VS)*ss,2);
    for k=1:length(VS)
        
        V_inputs((k-1)*ss+1:k*ss,2)=-[VS(k)]';
        
        V_inputs((k-1)*ss+1:k*ss,1)=[floor(VS(k)/sl/round)*round]';
        V_inputs((k-1)*ss+1:k*ss,1)=V_inputs((k-1)*ss+1:k*ss,1)+VL_range;
    end
    V_inputs2=V_inputs;
    for        j=2:2:length(VS)
        V_inputs2((j-1)*ss+1:j*ss,1) = flipud(V_inputs((j-1)*ss+1:j*ss,1));
        
    end
    V_inputs=V_inputs2;
end


    V_inputs1=[V_inputs(:,2), V_inputs(:,1)];


dlmwrite('inputs.csv',V_inputs1, 'precision', '%i','delimiter', ',')

