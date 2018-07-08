x1=0; y1=0; z1=0; z1gora=60;
x2=0; y2=28; z2=0; z2gora=60;
x3=24; y3=14; z3=0; z3gora=60;
e = 30; % liczba krokow ekstrudera
t = 200; % temperatura ktora ma trzymac regulator

r=23;

xc=12; yc=14; zc=30;

figure(1)

kol1 = line ([x1 x1],[y1 y1],[z1 z1gora]);
kol2 = line ([x2 x2],[y2 y2], [z2 z2gora]);
kol3 = line ([x3 x3],[y3 y3],[z3 z3gora]);
view(30,30)
axis ([0 60 0 60 0 60])
axis square
wozek1 = zc + sqrt(r^2-(x1-xc)^2-(y1-yc)^2);
wozek2 = zc + sqrt(r^2-(x2-xc)^2-(y2-yc)^2);
wozek3 = zc + sqrt(r^2-(x3-xc)^2-(y3-yc)^2);

ramie1 = line([x1 xc], [y1 yc], [wozek1 zc]);
ramie2 = line([x2 xc], [y2 yc], [wozek2 zc]);
ramie3 = line([x3 xc], [y3 yc], [wozek3 zc]);
for ti=0:0.1:2*pi %kolo

    xc= 10 + 2*sin(ti);
    yc= 10 + 2*cos(ti);
    zc= 10;
    %kinematyka odwrotna
    wozek1 = zc + sqrt(r^2-(x1-xc)^2-(y1-yc)^2);
    wozek2 = zc + sqrt(r^2-(x2-xc)^2-(y2-yc)^2);
    wozek3 = zc + sqrt(r^2-(x3-xc)^2-(y3-yc)^2);

    set(ramie1, 'xData',[x1 xc], 'yData', [y1 yc], 'zData', [wozek1 zc]);
    set(ramie2, 'xData',[x2 xc], 'yData', [y2 yc], 'zData', [wozek2 zc]);
    set(ramie3, 'xData',[x3 xc], 'yData', [y3 yc], 'zData', [wozek3 zc]);

    drawnow;
    if(ti>0)
        kroki1=round(200*(wozek1-stare1));
        kroki2=round(200*(wozek2-stare2));
        kroki3=round(200*(wozek3-stare3));

        fprintf(s,'x %d y %d z %d e %d t %d\n', [kroki1 kroki2 kroki3 e t]); % to sie wysyla do drukarki kiedy jest program odpalony
        while(s.BytesAvailable == 0)
        end
        while(s.BytesAvailable)
        fscanf(s);
        end
    end
    stare1=wozek1;
    stare2=wozek2;
    stare3=wozek3;
end
