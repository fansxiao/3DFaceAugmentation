#include "FaceAugmentation.h"

FaceAugmentation* FaceAugmentation::instance = NULL;

FaceAugmentation::FaceAugmentation() { }

FaceAugmentation& FaceAugmentation::Instance()
{
	if (!instance)
	{
		instance = new FaceAugmentation();
	}

	return *instance;
}

void FaceAugmentation::ExtractFacialFeatures(BYTE* pixels, FeatureExtraction::Features &features, int width, int height,
	FeatureExtraction::DescriptorType descriptorType, FeatureExtraction::DetectorType detectorType, SeetaFaceDetection::Face face)
{

}

void FaceAugmentation::ExtractFacialFeatures(BYTE* pixels, FeatureExtraction::Features &features, int width, int height,
	FeatureExtraction::DescriptorType descriptorType, FeatureExtraction::DetectorType detectorType, bool cropFace)
{
	cv::Mat image(height, width, CV_8UC3, pixels);

	if (!image.empty())
	{
		int faceCount;
		long long duration;

		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces(image.data, image.cols, image.rows, false, false, false, faceCount, duration);

		if (faceCount > 0)
		{
			cv::Mat procImage;
			
			if (cropFace)
				procImage = SeetaFaceDetection::Instance().CropFace(pixels, width, height, faces[0]);
			else
				procImage = image.clone();

			if (!procImage.empty())
			{
				FeatureExtraction::Features allFeatures;
				FeatureExtraction::Instance().ExtractFeatures(procImage, allFeatures, descriptorType, detectorType);
				cv::Mat facialDescriptors;
				for (int i = 0; i < allFeatures.keypointsSize; i++)
				{
					if (allFeatures.keypoints[i].pt.x >= faces[0].FaceCoordinate.left()
						&& allFeatures.keypoints[i].pt.x <= faces[0].FaceCoordinate.right()
						&& allFeatures.keypoints[i].pt.y >= faces[0].FaceCoordinate.top()
						&& allFeatures.keypoints[i].pt.y <= faces[0].FaceCoordinate.bottom())
					{
						features.keypoints.push_back(allFeatures.keypoints[i]);
						features.keypointsSize++;
						facialDescriptors.push_back(allFeatures.descriptors.row(i));
					}
				}

				features.descriptors = facialDescriptors;
				features.descriptorsWidth = facialDescriptors.cols;
				features.descriptorsHeight = facialDescriptors.rows;
			}
		}
	}
}

void FaceAugmentation::Create2D3DCorrespondences(std::vector<cv::DMatch> matches, std::vector<cv::KeyPoint> imageKeypoints,
	std::vector<cv::Point3d> model3Dpoints, std::vector<cv::Point2d> &imagePoints, std::vector<cv::Point3d> &modelPoints)
{
	for (int i = 0; i < matches.size(); i++)
	{
		imagePoints.push_back(cv::Point2d(imageKeypoints.at((matches.at(i).queryIdx)).pt.x,
			imageKeypoints.at((matches.at(i).queryIdx)).pt.y));

		modelPoints.push_back(cv::Point3d(model3Dpoints.at((matches.at(i).trainIdx)).x,
			model3Dpoints.at((matches.at(i).trainIdx)).y,
			model3Dpoints.at((matches.at(i).trainIdx)).z));
	}
}