#include "PoseEstimation.h"

PoseEstimation* PoseEstimation::instance = NULL;

PoseEstimation::PoseEstimation() { }

PoseEstimation& PoseEstimation::Instance()
{
	if (!instance)
	{
		instance = new PoseEstimation();
	}

	return *instance;
}

void PoseEstimation::EstimateHeadPose(cv::Mat image, std::vector<cv::Point3d> modelPoints, std::vector<cv::Point2d> imagePoints,
									cv::Mat &rotationVector, cv::Mat &translationVector)
{
	// Camera internals
	double focalLength = image.cols; // Approximate focal length.
	cv::Point2d center = cv::Point2d(image.cols / 2, image.rows / 2);
	cv::Mat cameraMatrix = (cv::Mat_<double>(3, 3) << focalLength, 0, center.x, 0, focalLength, center.y, 0, 0, 1);
	cv::Mat distCoeffs = cv::Mat::zeros(4, 1, cv::DataType<double>::type); // Assuming no lens distortion

	// Solve for pose
	cv::solvePnP(modelPoints, imagePoints, cameraMatrix, distCoeffs, rotationVector, translationVector);
}