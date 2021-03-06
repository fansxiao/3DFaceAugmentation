/**
* 
* This is a test for face detection task using SeetaFace detection API.
*
* @author João Paulo Lima (jpsml@cin.ufpe.br)
* @author João Otávio Brandão (jobal@cin.ufpe.br)
* @since 2016-10-26
*/

#include "SeetaFaceDetection.h"
//#include <opencv2/opencv.hpp>

int main()
{
	cv::Mat image = cv::imread("../../../Dataset/Images/Faces/0000006.jpg");
	int faceCount;
	long long duration;
	SeetaFaceDetection::Face* faces = SeetaFaceDetection::Instance().DetectFaces(image.data, image.cols, image.rows, false, false, false, faceCount, duration);

	std::cout << "faceCount: " << faceCount << std::endl << std::endl;

	for (int i = 0; i < faceCount; ++i)
	{
		std::cout << "face " << (i + 1) << std::endl;
		std::cout << "FaceCoordinate: " << faces[i].FaceCoordinate << std::endl;
		std::cout << "openEyesCount: " << faces[i].openEyesCount << std::endl;
		std::cout << "closedEyesCount: " << faces[i].closedEyesCount << std::endl;
		std::cout << "smilesCount: " << faces[i].smilesCount << std::endl;
		std::cout << "faceScore:" << faces[i].score << std::endl;
		std::cout << std::endl;

		cv::Mat imageCopy(image.rows, image.cols, image.channels());
		image.copyTo(imageCopy);

		cv::Rect boundingBox = cv::Rect(faces[i].FaceCoordinate.left(), faces[i].FaceCoordinate.top(), faces[i].FaceCoordinate.width(), faces[i].FaceCoordinate.height());
		cv::rectangle(imageCopy, boundingBox, cv::Scalar(0, 255, 0), 1, 8, 0);
	 
		cv::Mat croppedImg = SeetaFaceDetection::Instance().CropFace(image.data, image.cols, image.rows, faces[i]);

		cv::imshow("face detected" + (i + 1), imageCopy);
		cv::imshow("cropped" + (i + 1), croppedImg);
		cv::waitKey(0);
		cv::destroyAllWindows();
	}
	
    return 0;
}