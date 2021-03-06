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

void FeatureExtraction::ExtractFeatures(cv::Mat &image, Features &features, DescriptorType descriptorType, DetectorType detectorType)
{
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	cv::Ptr<cv::DescriptorExtractor> descriptor;
	cv::Ptr<cv::FeatureDetector> detector;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

	SetFeatureDetector(detector, detectorType);
	SetFeatureDescriptor(descriptor, descriptorType);

	detector->detect(grayImage, keypoints, cv::Mat());
	descriptor->compute(grayImage, keypoints, descriptors);
	//descriptor->detectAndCompute(grayImage, cv::Mat(), keypoints, descriptors);

	features.keypoints = keypoints;
	features.keypointsSize = keypoints.size();
	features.descriptors = descriptors;
	features.descriptorsWidth = descriptors.cols;
	features.descriptorsHeight = descriptors.rows;
}

void FeatureExtraction::ExtractFeatures(BYTE* pixels, int width, int height, Features &features, DescriptorType descriptorType, DetectorType detectorType)
{
	cv::Mat image(height, width, CV_8UC3, pixels);
	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	cv::Ptr<cv::DescriptorExtractor> descriptor;
	cv::Ptr<cv::FeatureDetector> detector;
	std::vector<cv::KeyPoint> keypoints;
	cv::Mat descriptors;

	SetFeatureDetector(detector, detectorType);
	SetFeatureDescriptor(descriptor, descriptorType);

	detector->detect(grayImage, keypoints, cv::Mat());
	descriptor->compute(grayImage, keypoints, descriptors);
	//extractor->detectAndCompute(grayImage, cv::Mat(), keypoints, descriptors);

	features.keypoints = keypoints;
	features.keypointsSize = keypoints.size();
	features.descriptors = descriptors;
	features.descriptorsWidth = descriptors.cols;
	features.descriptorsHeight = descriptors.rows;
}

void FeatureExtraction::Match(cv::Mat descriptors1, cv::Mat descriptors2, std::vector<cv::DMatch> &matches, DescriptorMatcher descriptorMatcher)
{
	if (descriptorMatcher == FeatureExtraction::DescriptorMatcher::FLANN_BASED)
	{
		cv::FlannBasedMatcher matcher;
		matcher.match(descriptors1, descriptors2, matches);
	}
	else
	{
		cv::BFMatcher matcher;
		matcher.create(cv::NORM_HAMMING, false);
		matcher.match(descriptors1, descriptors2, matches);
	}
}

void FeatureExtraction::SetFeatureDescriptor(cv::Ptr<cv::DescriptorExtractor> &extractor, DescriptorType descriptorType)
{
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
}

void FeatureExtraction::SetFeatureDetector(cv::Ptr<cv::FeatureDetector> &detector, DetectorType detectorType)
{
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
}