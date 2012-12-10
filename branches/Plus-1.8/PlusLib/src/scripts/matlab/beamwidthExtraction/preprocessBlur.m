% source: http://stackoverflow.com/questions/2773606/gaussian-filter-in-matlab

function Blurred = preprocessBlur(Image,... % Image to be blurred
                                  sizes,... % [x y] size of blur filter
                                  sigma)  % Radial decay
                   
blur = fspecial('gaussian',sizes,sigma);
Blurred = imfilter(Image,blur,'replicate','same');

end

