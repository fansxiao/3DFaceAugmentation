width = 640;
height = 480;
model_name = '3DModels/Faces/Model_2/3DScan.obj';
features_filename = 'features.txt';

%% STEP 1 - RENDER 3D REFERENCE MODEL, DETECT SPECIFIC FACIAL FEATURES, GET THEIR 3D COORDINATES ON THE 3D MODEL.

% % 1.1 Render.
[refDepth, refRendered, refUnproject, outA, outR, outT] = renderer(width, height, model_name, 0, 0, 0, -90, 0, 0, 'zxy');
%[refDepth, refRendered, refUnproject, outA, outR, outT]=renderer(width, height, model_name);

% Display the results.
figure, imshow(refRendered);

% View the 3D points of the model.
%figure, plot3(unproject(:,:,1),unproject(:,:,2),unproject(:,:,3),'.');

% % 1.2 Get facial features on the rendered image.
% Read keypoints from file.
if exist(features_filename, 'file') == 2
    delimiterIn = ' ';
    headerlinesIn = 1;
    kps = importdata(features_filename, delimiterIn, headerlinesIn);
    pts_2D = uint32(kps.data());
    %disp(pts_2D)

    %hold on;
    %plot(pts_2D(:,1), pts_2D(:,2), '.');
    
    pts_3D=zeros(size(pts_2D, 1), 3);
    
    % Get 3D coordinates of facial features, using the unproject map from image 2D to model 3D.
    for j=1:size(pts_2D, 1)
        pts_3D(j, :) = refUnproject(pts_2D(j, 2), pts_2D(j, 1), 1:3);
    end   
    disp(pts_3D);
    
    %hold on;
    %plot3(pts_3D(:, 1), pts_3D(:, 2), pts_3D(:, 3), '.');
    
    addpath('calib/')
    [outA, R_new, T_new] = doCalib(width, height, pts_2D, pts_3D, outA, [], []);
end

