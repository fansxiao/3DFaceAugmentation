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
										CameraInternals cameraInternals, bool ransac, cv::Mat &rotationVector, cv::Mat &translationVector)
{
	if (!ransac)
		cv::solvePnP(modelPoints, imagePoints, cameraInternals.cameraMatrix, cameraInternals.distCoeffs, rotationVector, translationVector);
	else
		cv::solvePnPRansac(modelPoints, imagePoints, cameraInternals.cameraMatrix, cameraInternals.distCoeffs, rotationVector, translationVector);
}

void PoseEstimation::ProjectPoints(std::vector<cv::Point3d> modelPoints, CameraInternals cameraInternals,
									cv::Mat rotationVector, cv::Mat translationVector, std::vector<cv::Point2d> &projPoints)
{
	cv::projectPoints(modelPoints, rotationVector, translationVector, cameraInternals.cameraMatrix, cameraInternals.distCoeffs, projPoints);
}