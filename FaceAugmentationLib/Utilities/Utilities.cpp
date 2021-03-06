#include "Utilities.h"

Utility* Utility::instance = NULL;

Utility::Utility() { }

Utility& Utility::Instance()
{
	if (!instance)
	{
		instance = new Utility();
	}

	return *instance;
}

UTILITIES_API void Utility::SaveKeypoints(std::vector<cv::KeyPoint> keypoints, std::string file)
{
	CreateNewFile(file);

	std::fstream fs;
	fs.open(file, std::fstream::in | std::fstream::out);
	
	if (fs.is_open())
	{
		fs << "x y" << std::endl;
	
		for (int i = 0; i < keypoints.size(); i++)
			fs << "" << keypoints[i].pt.x << " " << keypoints[i].pt.y << std::endl;
	}
	fs.close();
}

void Utility::CreateNewFile(std::string file)
{
	struct stat buffer;
	if ((stat(file.c_str(), &buffer) != 0))
	{
		std::fstream fs;
		fs.open(file, std::fstream::in | std::fstream::out | std::fstream::app);
		fs.close();
	}
}

void Utility::Read3DPoints(std::vector<cv::Point3d> &points, std::string file)
{
	std::ifstream ifs(file);

	if (ifs.is_open())
	{
		std::string line;

		while (getline(ifs, line))
		{
			char *lineChar = new char[line.length() + 1];
			strcpy(lineChar, line.c_str());
			std::vector<std::string> values = Split(lineChar, ' ');
			float x = std::stof(values.at(0));
			float y = std::stof(values.at(1));
			float z = std::stof(values.at(2));
			points.push_back(cv::Point3d(x, y, z));
		}
		ifs.close();
	}
}

void Utility::ReadCameraParams(PoseEstimation::CameraInternals &cameraInternals, std::string file)
{
	std::ifstream ifs(file);

	if (ifs.is_open())
	{
		std::string line;
		int lineCount = 0;
		std::vector<double> aValues;
		std::vector<double> rValues;

		while (getline(ifs, line))
		{
			char *lineChar = new char[line.length() + 1];
			strcpy(lineChar, line.c_str());
			if (lineCount < 3)
			{
				std::vector<std::string> values = Split(lineChar, ' ');
				aValues.push_back(std::stof(values.at(0)));
				aValues.push_back(std::stof(values.at(1)));
				aValues.push_back(std::stof(values.at(2)));
			}
			else if (lineCount == 3)
			{
				for (int i = 0; i < aValues.size(); i++)
				{
					cameraInternals.A = (cv::Mat_<double>(3, 3) << aValues.at(0), aValues.at(1), aValues.at(2)
						, aValues.at(3), aValues.at(4), aValues.at(5)
						, aValues.at(6), aValues.at(7), aValues.at(8));

				}
			}
			else if (lineCount > 3 && lineCount < 7)
			{
				std::vector<std::string> values = Split(lineChar, ' ');
				rValues.push_back(std::stof(values.at(0)));
				rValues.push_back(std::stof(values.at(1)));
				rValues.push_back(std::stof(values.at(2)));
			}
			else if (lineCount == 7)
			{
				for (int i = 0; i < rValues.size(); i++)
				{
					cameraInternals.R = (cv::Mat_<double>(3, 3) << rValues.at(0), rValues.at(1), rValues.at(2)
						, rValues.at(3), rValues.at(4), rValues.at(5)
						, rValues.at(6), rValues.at(7), rValues.at(8));

				}
			}
			else if (lineCount == 8)
			{
				std::vector<std::string> values = Split(lineChar, ' ');
				cameraInternals.T = (cv::Mat_<double>(3, 1) << std::stof(values.at(0)), std::stof(values.at(1)), std::stof(values.at(2)));
			}

			lineCount++;
		}
	}

	ifs.close();
}

std::vector<std::string> Utility::Split(const char *str, char c)
{
	std::vector<std::string> result;

	do
	{
		const char *begin = str;

		while (*str != c && *str)
			str++;

		result.push_back(std::string(begin, str));
	} while (0 != *str++);

	return result;
}

//FeatureExtraction::Features Utility::ExtractFacialFeatures(BYTE* pixels, int width, int height, bool crop, FeatureExtraction::DescriptorType descriptorType, FeatureExtraction::DetectorType detectorType, std::string featuresFilename)
//{
//	cv::Mat image(height, width, CV_8UC3, pixels);
//
//	if (!image.empty())
//	{
//		int faceCount;
//		long long duration;
//
//		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces(image.data, image.cols, image.rows, false, false, false, faceCount, duration);
//
//		if (faceCount > 0)
//		{
//			cv::Mat procImage;
//
//			if (crop)
//				procImage = SeetaFaceDetection::Instance().CropFace(pixels, width, height, faces[0]);
//			else
//				procImage = image.clone();
//
//			if (!procImage.empty())
//			{
//				FeatureExtraction::Features feature = FeatureExtraction::Instance().ExtractFeatures(procImage.data, procImage.cols, procImage.rows, descriptorType, detectorType);
//
//				if (featuresFilename != "")
//				{
//					CreateNewFile(featuresFilename);
//					SaveFeatures(feature, featuresFilename);
//				}
//
//				return feature;
//			}
//		}
//	}
//
//	return {};
//}
//
//FeatureExtraction::Features Utility::ExtractFacialFeatures(BYTE* pixels, int width, int height, FeatureExtraction::DescriptorType descriptorType, FeatureExtraction::DetectorType detectorType, std::string featuresFilename)
//{
//	cv::Mat image(height, width, CV_8UC3, pixels);
//
//	if (!image.empty())
//	{
//		int faceCount;
//		long long duration;
//
//		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces
//		(image.data, image.cols, image.rows, false, false, false, faceCount, duration);
//
//		if (faceCount > 0)
//		{
//			cv::Mat grayImage;
//			cv::cvtColor(image, grayImage, CV_BGR2GRAY);
//			FeatureExtraction::Features feature = FeatureExtraction::Instance().ExtractFeatures
//			(grayImage.data, grayImage.cols, grayImage.rows, descriptorType, detectorType);
//
//			FeatureExtraction::Features facialFeature = {};
//
//			for (int i = 0; i < feature.keypoints.size(); i++)
//			{
//				if (feature.keypoints[i].pt.x >= faces[0].FaceCoordinate.left() && feature.keypoints[i].pt.x <= faces[0].FaceCoordinate.right()
//					&& feature.keypoints[i].pt.y >= faces[0].FaceCoordinate.top() && feature.keypoints[i].pt.y <= faces[0].FaceCoordinate.bottom())
//				{
//					facialFeature.keypoints.push_back(feature.keypoints[i]);
//					facialFeature.keypointsSize++;
//				}					
//			}
//
//			if (featuresFilename != "")
//			{
//				CreateNewFile(featuresFilename);
//				SaveFeatures(facialFeature, featuresFilename);
//			}
//
//			return facialFeature;
//		}
//	}
//
//	return {};
//}
//
//FeatureExtraction::Features Utility::ExtractFacialFeatures(cv::Mat image, FeatureExtraction::DescriptorType descriptorType, FeatureExtraction::DetectorType detectorType, std::string featuresFilename)
//{
//	if (!image.empty())
//	{
//		int faceCount;
//		long long duration;
//
//		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces
//		(image.data, image.cols, image.rows, false, false, false, faceCount, duration);
//
//		if (faceCount > 0)
//		{
//			cv::Mat grayImage;
//			cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
//			cv::imshow("Gray", grayImage);
//			cv::waitKey(0);
//			FeatureExtraction::Features feature = FeatureExtraction::Instance().ExtractFeatures
//			(grayImage, descriptorType, detectorType);
//
//			FeatureExtraction::Features facialFeature = {};
//
//			for (int i = 0; i < feature.keypoints.size(); i++)
//			{
//				if (feature.keypoints[i].pt.x >= faces[0].FaceCoordinate.left() && feature.keypoints[i].pt.x <= faces[0].FaceCoordinate.right()
//					&& feature.keypoints[i].pt.y >= faces[0].FaceCoordinate.top() && feature.keypoints[i].pt.y <= faces[0].FaceCoordinate.bottom())
//				{
//					facialFeature.keypoints.push_back(feature.keypoints[i]);
//					facialFeature.keypointsSize++;
//				}
//			}
//
//			if (featuresFilename != "")
//			{
//				CreateNewFile(featuresFilename);
//				SaveFeatures(facialFeature, featuresFilename);
//			}
//
//			return facialFeature;
//		}
//	}
//
//	return {};
//}
//

//
//void Utility::SaveFeatures(FeatureExtraction::Features feature, std::string filename)
//{
//	std::fstream fs;
//	fs.open(filename, std::fstream::in | std::fstream::out);
//
//	if (fs.is_open())
//	{
//		fs << "x y" << std::endl;
//
//		for (int i = 0; i < feature.keypoints.size(); i++)
//			fs << "" << feature.keypoints[i].pt.x << " " << feature.keypoints[i].pt.y << std::endl;
//	}
//	fs.close();
//}
//
//void Utility::CropAndSaveImage(BYTE* pixels, int width, int height, std::string cropFilename)
//{
//	cv::Mat image(height, width, CV_8UC3, pixels);
//
//	if (!image.empty())
//	{
//		int faceCount;
//		long long duration;
//
//		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces(image.data, image.cols, image.rows, false, false, false, faceCount, duration);
//
//		if (faceCount > 0)
//		{
//			cv::Mat crop_image = SeetaFaceDetection::Instance().CropFace(pixels, width, height, faces[0]);
//			cv::imwrite(cropFilename, crop_image);
//		}
//	}
//}
//
//std::vector<cv::Point2d> Utility::ParseKeyPointsTo2DPoints(std::vector<cv::KeyPoint> keypoints)
//{
//	std::vector<cv::Point2d> pts2d;
//	for (int i = 0; i < keypoints.size(); i++)
//		pts2d.push_back(cv::Point2d(keypoints.at(i).pt.x, keypoints.at(i).pt.y));
//	return pts2d;
//}
//
//void Utility::DrawLines(std::vector<cv::Point2d> imagePoints, std::vector<cv::Point2d> projPoints, cv::Mat &outputImage)
//{
//	/*for (int i = 0; i < imagePoints.size(); i++)
//		cv::circle(outputImage, imagePoints[i], 3, cv::Scalar(0, 0, 255), -1);*/
//
//	if (projPoints.size() >= imagePoints.size())
//	{
//		for (int i = 0; i < projPoints.size(); i++)
//			cv::line(outputImage, imagePoints[i], projPoints[i], cv::Scalar(0, 255, 0), 2);
//	}
//}
//
//
