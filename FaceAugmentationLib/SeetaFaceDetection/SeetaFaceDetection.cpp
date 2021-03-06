/* Voxar Labs, Samsung Electronics
*
* Copyright(c) 2016 by Samsung Electronics
*
* This software is the confidential and proprietary information
* of Samsung Electronics, Inc. ("Confidential Information"). You
* shall not disclose such Confidential Information and shall use
* it only in accordance with the terms of the license agreement
* you entered into with Samsung.
*
* Description: Face detection using SeetaFace.
*
* @author João Paulo Lima (jpsml@cin.ufpe.br)
* @author João Otávio Brandão (jobal@cin.ufpe.br)
* @since 2016-10-26
*/

#include "SeetaFaceDetection.h"


SeetaFaceDetection* SeetaFaceDetection::instance = NULL;

SeetaFaceDetection::SeetaFaceDetection()
	: faceDetector("seeta_fd_frontal_v1.0.bin")
	, landmarkDetector("seeta_fa_v1.1.bin")
	, faceRecognizer("seeta_fr_v1.0.bin")
{
	faceDetector.SetMinFaceSize(40);
	faceDetector.SetScoreThresh(2.f);
	faceDetector.SetImagePyramidScaleFactor(0.8f);
	faceDetector.SetWindowStep(4, 4);

	openEyeCascade.load("haarcascade_eye_tree_eyeglasses.xml");
	smileCascade.load("haarcascade_smile.xml");
}

SeetaFaceDetection& SeetaFaceDetection::Instance()
{
	if (!instance)
	{
		instance = new SeetaFaceDetection();
	}

	return *instance;
}

SeetaFaceDetection::Face* SeetaFaceDetection::DetectFaces(BYTE* pixels, int width, int height, bool detectEyes, bool detectSmile, bool writeLog, int& facecount, long long& durationNs)
{
	cv::Mat img(height, width, CV_8UC3, pixels);

	cv::Mat imgGray;
	cv::cvtColor(img, imgGray, cv::COLOR_BGR2GRAY);

	seeta::ImageData imgData;
	imgData.data = imgGray.data;
	imgData.width = imgGray.cols;
	imgData.height = imgGray.rows;
	imgData.num_channels = 1;

	auto start = std::chrono::high_resolution_clock::now();

	std::vector<seeta::FaceInfo> facesDetected = faceDetector.Detect(imgData);

	facecount = facesDetected.size();

	Face* faces = (Face*)malloc(sizeof(Face)* facecount);

	for (int i = 0; i < facecount; i++)
	{
		faces[i].FaceCoordinate.set_left(facesDetected[i].bbox.x);
		faces[i].FaceCoordinate.set_top(facesDetected[i].bbox.y);
		faces[i].FaceCoordinate.set_right(facesDetected[i].bbox.x + facesDetected[i].bbox.width);
		faces[i].FaceCoordinate.set_bottom(facesDetected[i].bbox.y + facesDetected[i].bbox.height);
		faces[i].score = facesDetected[i].score;

		if (detectEyes)
		{
			seeta::FacialLandmark landmarks[5];
			landmarkDetector.PointDetectLandmarks(imgData, facesDetected[i], landmarks);

			cv::Rect leftEyeRect(landmarks[0].x - facesDetected[i].bbox.width / 6, landmarks[0].y - facesDetected[i].bbox.height / 6, facesDetected[i].bbox.width / 3, facesDetected[i].bbox.height / 3);
			cv::Rect rightEyeRect(landmarks[1].x - facesDetected[i].bbox.width / 6, landmarks[1].y - facesDetected[i].bbox.height / 6, facesDetected[i].bbox.width / 3, facesDetected[i].bbox.height / 3);

			cv::Mat leftEyeROI(imgGray, leftEyeRect);
			cv::Mat rightEyeROI(imgGray, rightEyeRect);

			cv::Mat leftEyeResized;
			cv::Mat rightEyeResized;

			double ratioEyes = 100.0 / leftEyeRect.height;

			cv::resize(leftEyeROI, leftEyeResized, cv::Size(), ratioEyes, ratioEyes);
			cv::resize(rightEyeROI, rightEyeResized, cv::Size(), ratioEyes, ratioEyes);

			std::vector<cv::Rect> openLeftEyes;
			std::vector<cv::Rect> openRightEyes;

			openEyeCascade.detectMultiScale(leftEyeResized, openLeftEyes, 1.1, 2, cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(0, 0));
			openEyeCascade.detectMultiScale(rightEyeResized, openRightEyes, 1.1, 2, cv::CASCADE_FIND_BIGGEST_OBJECT, cv::Size(0, 0));

			if (!openLeftEyes.empty())
			{
				cv::Rect leftEyeScaled((double)openLeftEyes[0].x / ratioEyes,
					(double)openLeftEyes[0].y / ratioEyes,
					(double)openLeftEyes[0].width / ratioEyes,
					(double)openLeftEyes[0].height / ratioEyes);

				leftEyeRect = cv::Rect(leftEyeRect.tl() + leftEyeScaled.tl(), leftEyeScaled.size());
			}

			if (!openRightEyes.empty())
			{
				cv::Rect rightEyeScaled((double)openRightEyes[0].x / ratioEyes,
					(double)openRightEyes[0].y / ratioEyes,
					(double)openRightEyes[0].width / ratioEyes,
					(double)openRightEyes[0].height / ratioEyes);

				rightEyeRect = cv::Rect(rightEyeRect.tl() + rightEyeScaled.tl(), rightEyeScaled.size());
			}

			if (!openLeftEyes.empty() || !openRightEyes.empty())
			{
				faces[i].openEyesCount = 1;
				faces[i].closedEyesCount = 0;

				cv::Rect boundingRect = leftEyeRect | rightEyeRect;

				faces[i].EyesCoordinate.set_left(boundingRect.x - facesDetected[i].bbox.x);
				faces[i].EyesCoordinate.set_top(boundingRect.y - facesDetected[i].bbox.y);
				faces[i].EyesCoordinate.set_right(boundingRect.x - facesDetected[i].bbox.x + boundingRect.width);
				faces[i].EyesCoordinate.set_bottom(boundingRect.y - facesDetected[i].bbox.y + boundingRect.height);
			}
			else
			{
				faces[i].openEyesCount = 0;
				faces[i].closedEyesCount = 1;

				faces[i].EyesCoordinate.set_left(0);
				faces[i].EyesCoordinate.set_top(0);
				faces[i].EyesCoordinate.set_right(0);
				faces[i].EyesCoordinate.set_bottom(0);
			}
		}

		if (detectSmile)
		{
			cv::Rect mouthRect(facesDetected[i].bbox.x, facesDetected[i].bbox.y + facesDetected[i].bbox.height / 2, facesDetected[i].bbox.width, facesDetected[i].bbox.height / 2);

			if ((cv::Rect(0, 0, imgGray.cols, imgGray.rows) & mouthRect) == mouthRect)
			{
				cv::Mat mouthROI(imgGray, mouthRect);

				cv::Mat mouthResized;

				double ratioMouth = 100.0 / mouthRect.height;

				cv::resize(mouthROI, mouthResized, cv::Size(), ratioMouth, ratioMouth);

				std::vector<cv::Rect> smiles;

				smileCascade.detectMultiScale(mouthResized, smiles, 1.7, 22, cv::CASCADE_SCALE_IMAGE | cv::CASCADE_FIND_BIGGEST_OBJECT);

				if (!smiles.empty())
				{
					faces[i].smilesCount = 1;

					cv::Rect smileScaled((double)smiles[0].x / ratioMouth,
						(double)smiles[0].y / ratioMouth,
						(double)smiles[0].width / ratioMouth,
						(double)smiles[0].height / ratioMouth);

					cv::Rect smileRect = cv::Rect(mouthRect.tl() + smileScaled.tl(), smileScaled.size());

					faces[i].SmileCoordinate.set_left(smileRect.x - facesDetected[i].bbox.x);
					faces[i].SmileCoordinate.set_top(smileRect.y - facesDetected[i].bbox.y);
					faces[i].SmileCoordinate.set_right(smileRect.x - facesDetected[i].bbox.x + smileRect.width);
					faces[i].SmileCoordinate.set_bottom(smileRect.y - facesDetected[i].bbox.y + smileRect.height);
				}
				else
				{
					faces[i].smilesCount = 0;

					faces[i].SmileCoordinate.set_left(0);
					faces[i].SmileCoordinate.set_top(0);
					faces[i].SmileCoordinate.set_right(0);
					faces[i].SmileCoordinate.set_bottom(0);
				}
			}
			else
			{
				faces[i].smilesCount = 0;

				faces[i].SmileCoordinate.set_left(0);
				faces[i].SmileCoordinate.set_top(0);
				faces[i].SmileCoordinate.set_right(0);
				faces[i].SmileCoordinate.set_bottom(0);
			}
		}
	}

	auto finish = std::chrono::high_resolution_clock::now();
	durationNs = std::chrono::duration_cast<std::chrono::nanoseconds>(finish - start).count();

	return faces;
}

cv::Mat SeetaFaceDetection::CropFace(BYTE* pixels, int width, int height, Face face)
{
	cv::Mat image(height, width, CV_8UC3, pixels);
	seeta::ImageData imageData(image.cols, image.rows, image.channels());
	imageData.data = image.data;

	cv::Mat grayImage;
	cv::cvtColor(image, grayImage, cv::COLOR_BGR2GRAY);
	seeta::ImageData grayImgData(grayImage.cols, grayImage.rows, grayImage.channels());
	grayImgData.data = grayImage.data;

	seeta::FaceInfo faceInfo;
	faceInfo.bbox.y = face.FaceCoordinate.top();
	faceInfo.bbox.x = face.FaceCoordinate.left();
	faceInfo.bbox.width = face.FaceCoordinate.width();
	faceInfo.bbox.height = face.FaceCoordinate.height();

	seeta::FacialLandmark landmarks[5];
	landmarkDetector.PointDetectLandmarks(grayImgData, faceInfo, landmarks);

	cv::Mat dstImage(faceRecognizer.crop_height(), faceRecognizer.crop_width(), CV_8UC(faceRecognizer.crop_channels()));
	seeta::ImageData dstImageData(dstImage.cols, dstImage.rows, dstImage.channels());
	dstImageData.data = dstImage.data;

	faceRecognizer.CropFace(imageData, landmarks, dstImageData);

	image.release();
	grayImage.release();

	return dstImage;
}