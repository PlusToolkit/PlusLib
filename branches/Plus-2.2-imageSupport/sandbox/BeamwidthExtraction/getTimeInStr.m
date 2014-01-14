function s = getTimeInStr()

    c = clock;
    s = date;
    s = strcat(s,'-');
    s = strcat(s,num2str(c(4)));
    s = strcat(s,'-');
    s = strcat(s,num2str(c(5)));
    s = strcat(s,'-');
    s = strcat(s,num2str(round(c(6))));

end

