image = imread('./Data/GEv730_LinearArray/1FocalPointTop/N0000002.png');

limit = size(image,2);
offset = 350;

for (i = 1:1:limit-offset)
    avgRows = mean(image(:,i:1:i+offset),2);
    h = plot(avgRows);
    hold on;
    topOfGraph = get(gca,'YLim');
    topOfGraph = topOfGraph(2);
    plot([349, 349],[topOfGraph, 0],'-r');
    plot([382, 382],[topOfGraph, 0],'-r');
    hold off;
    fname = num2str(i);
    fname = strcat(fname,'.png');
    saveas(h,fname);
end
