#include "FeatureExtraction.h"

FeatureExtraction* FeatureExtraction::instance = NULL;

FeatureExtraction::FeatureExtraction() { }

FeatureExtraction& FeatureExtraction::Instance()
{
	if (!instance)
	{
		instance = new FeatureExtraction();
	}

	return *instance;
}

FeatureExtraction::Feature FeatureExtraction::ExtractFeatures(BYTE* pixels, int width, int height, DescriptorType descriptorType, DetectorType detectorType)
{
	cv::Mat image(height, width, CV_8UC3, pixels);

	std::vector<cv::KeyPoint> keypoints = DetectFeatures(pixels, width, height, detectorType);

	if (keypoints.size() > 0)
	{
		cv::Ptr<cv::DescriptorExtractor> extractor;
		cv::Mat descriptors;

		switch (descriptorType)
		{
		case FeatureExtraction::DescriptorType::AKAZE:
		{
			extractor = cv::AKAZE::create();
			break;
		}
		case FeatureExtraction::DescriptorType::BRIEF:
		{
			extractor = cv::xfeatures2d::BriefDescriptorExtractor::create();
			break;
		}
		case FeatureExtraction::DescriptorType::BRISK:
		{
			extractor = cv::BRISK::create();
			break;
		}
		case FeatureExtraction::DescriptorType::DAISY:
		{
			extractor = cv::xfeatures2d::DAISY::create();
			break;
		}
		case FeatureExtraction::DescriptorType::FREAK:
		{
			extractor = cv::xfeatures2d::FREAK::create();
			break;
		}
		case FeatureExtraction::DescriptorType::KAZE:
		{
			extractor = cv::KAZE::create();
			break;
		}
		case FeatureExtraction::DescriptorType::LATCH:
		{
			extractor = cv::xfeatures2d::LATCH::create();
			break;
		}
		case FeatureExtraction::DescriptorType::LUCID:
		{
			extractor = cv::xfeatures2d::LUCID::create();
			break;
		}
		case FeatureExtraction::DescriptorType::ORB:
		{
			extractor = cv::ORB::create();
			break;
		}
		case FeatureExtraction::DescriptorType::SIFT:
		{
			extractor = cv::xfeatures2d::SIFT::create();
			break;
		}
		case FeatureExtraction::DescriptorType::SURF:
		{
			extractor = cv::xfeatures2d::SURF::create();
			break;
		}
		default:
			extractor = cv::ORB::create();
			break;
		}

		extractor->compute(image, keypoints, descriptors);

		Feature feature = {
			keypoints,
			descriptors,
			keypoints.size(),
			descriptors.cols,
			descriptors.rows
		};

		return feature;
	}
	return {};
}


std::vector<cv::KeyPoint> FeatureExtraction::DetectFeatures(BYTE* pixels, int width, int height, FeatureExtraction::DetectorType detectorType)
{
	cv::Mat image(height, width, CV_8UC3, pixels);
	std::vector<cv::KeyPoint> keypoints;

	cv::Ptr<cv::FeatureDetector> detector;

	switch (detectorType) 
	{
	case FeatureExtraction::DetectorType::AGAST:
	{
		detector = cv::AgastFeatureDetector::create();
		break;
	}
	case FeatureExtraction::DetectorType::AKAZE:
	{
		detector = cv::AKAZE::create();
		break;
	}
	case FeatureExtraction::DetectorType::BRISK:
	{
		detector = cv::BRISK::create();
		break;
	}
	case FeatureExtraction::DetectorType::FAST:
	{
		detector = cv::FastFeatureDetector::create();
		break;
	}
	case FeatureExtraction::DetectorType::GFFT:
	{
		detector = cv::GFTTDetector::create();
		break;
	}
	case FeatureExtraction::DetectorType::KAZE:
	{
		detector = cv::KAZE::create();
		break;
	}
	case FeatureExtraction::DetectorType::MSDD:
	{
		detector = cv::xfeatures2d::MSDDetector::create();
		break;
	}
	case FeatureExtraction::DetectorType::MSER:
	{
		detector = cv::MSER::create();
		break;
	}
	case FeatureExtraction::DetectorType::ORB:
	{
		detector = cv::ORB::create();
		break;
	}
	case FeatureExtraction::DetectorType::SBD:
	{
		detector = cv::SimpleBlobDetector::create();
		break;
	}
	case FeatureExtraction::DetectorType::SIFT:
	{
		detector = cv::xfeatures2d::SIFT::create();
		break;
	}
	case FeatureExtraction::DetectorType::STAR:
	{
		detector = cv::xfeatures2d::StarDetector::create();
		break;
	}
	case FeatureExtraction::DetectorType::SURF:
	{
		detector = cv::xfeatures2d::SURF::create();
		break;
	}
	default:
		detector = cv::ORB::create();
		break;
	}

	detector->detect(image, keypoints);

	return keypoints;
}