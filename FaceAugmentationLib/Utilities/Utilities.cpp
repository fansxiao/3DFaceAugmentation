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

FeatureExtraction::Feature Utility::ExtractFacialFeatures(BYTE* pixels, int width, int height, bool crop, FeatureExtraction::DescriptorType descriptorType, FeatureExtraction::DetectorType detectorType, std::string feat_file_path)
{
	cv::Mat image(height, width, CV_8UC3, pixels);

	if (!image.empty())
	{
		int faceCount;
		long long duration;

		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces(image.data, image.cols, image.rows, false, false, false, faceCount, duration);

		if (faceCount > 0)
		{
			cv::Mat proc_image;

			if (crop)
				proc_image = SeetaFaceDetection::Instance().CropFace(pixels, width, height, faces[0]);
			else
				proc_image = image.clone();

			if (!proc_image.empty())
			{
				FeatureExtraction::Feature feature = FeatureExtraction::Instance().ExtractFeatures(proc_image.data, proc_image.cols, proc_image.rows, descriptorType, detectorType);

				if (feat_file_path != "")
				{
					CreateNewFile(feat_file_path);
					SaveFeatures(feature, feat_file_path);
				}

				return feature;
			}
		}
	}

	return {};
}

void Utility::CreateNewFile(std::string file_path)
{
	std::fstream fs;

	fs.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);

	if (!fs)
		fs.open(file_path, std::fstream::in | std::fstream::out | std::fstream::trunc);
	fs.close();
}

void Utility::SaveFeatures(FeatureExtraction::Feature feature, std::string file_path)
{
	std::fstream fs;
	fs.open(file_path, std::fstream::in | std::fstream::out | std::fstream::app);

	if (fs.is_open())
	{
		/*fs << "keypoints " << feature.keypointsSize << std::endl;
		fs << "x y size angle response octave class_id" << std::endl;*/

		fs << "x y" << std::endl;

		for (int i = 0; i < feature.keypointsSize; i++)
		{
			/*fs << "" << feature.keypoints[i].pt.x << " " << feature.keypoints[i].pt.y << " " << feature.keypoints[i].angle << " "
				<< feature.keypoints[i].response << " " << feature.keypoints[i].octave << " " << feature.keypoints[i].class_id << std::endl;*/
			fs << "" << feature.keypoints[i].pt.x << " " << feature.keypoints[i].pt.y << std::endl;
		}
	}
	fs.close();
}

void Utility::CropAndSaveImage(BYTE* pixels, int width, int height, std::string crop_image_path)
{
	cv::Mat image(height, width, CV_8UC3, pixels);

	if (!image.empty())
	{
		int faceCount;
		long long duration;

		SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces(image.data, image.cols, image.rows, false, false, false, faceCount, duration);

		if (faceCount > 0)
		{
			cv::Mat crop_image = SeetaFaceDetection::Instance().CropFace(pixels, width, height, faces[0]);
			cv::imwrite(crop_image_path, crop_image);
		}
	}
}