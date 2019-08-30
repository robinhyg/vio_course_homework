
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <thread>
#include <iomanip>

#include <cv.h>
#include <opencv2/opencv.hpp>
#include <highgui.h>
#include <eigen3/Eigen/Dense>
#include "System.h"

using namespace std;
using namespace cv;
using namespace Eigen;

const int nDelayTimes = 2;
string sData_path = "/home/dataset/EuRoC/MH-05/mav0/";
string sConfig_path = "../config/";

std::shared_ptr<System> pSystem;

void PubImuData()
{
	/************************************ 修改部分 start ******************************************/
	string sImu_data_file = "./imu_pose.txt";
	cout << "1 PubImuData start sImu_data_file: " << sImu_data_file << endl;
	ifstream fsImu;
	fsImu.open(sImu_data_file.c_str());
	if (!fsImu.is_open())
	{
		cerr << "Failed to open imu file! " << sImu_data_file << endl;
		return;
	}

	std::string sImu_line;
	double dStampNSec = 0.0;
	double dDrop = 0.0;
	Vector3d vAcc;
	Vector3d vGyr;
	while (std::getline(fsImu, sImu_line) && !sImu_line.empty()) // read imu data
	{
		std::istringstream ssImuData(sImu_line);
		ssImuData >> dDrop >> dStampNSec >> dDrop >> dDrop >> dDrop >> dDrop >> dDrop >> dDrop >> dDrop >> vGyr.x() >> vGyr.y() >> vGyr.z() >> vAcc.x() >> vAcc.y() >> vAcc.z();
		// cout << "Imu t: " << fixed << dStampNSec << " gyr: " << vGyr.transpose() << " acc: " << vAcc.transpose() << endl;
		pSystem->PubImuData(dStampNSec, vGyr, vAcc);
		usleep(5000*nDelayTimes);
	}
	fsImu.close();
	/************************************ 修改部分 end ******************************************/
}

void PubImageData()
{
	/************************************ 修改部分 start ******************************************/
	string sImage_file = "./cam_pose.txt";

	cout << "1 PubImageData start sImage_file: " << sImage_file << endl;

	ifstream fsImage;
	fsImage.open(sImage_file.c_str());
	if (!fsImage.is_open())
	{
		cerr << "Failed to open image file! " << sImage_file << endl;
		return;
	}

	std::string sImage_line;
	double dDrop = 0.0;
	double dStampNSec;
	string sImgFileName;

	ifstream fsFeatures;
	std::string sFeatures_line;
	double dPu, dPv;
	int p_id;
	
	// cv::namedWindow("SOURCE IMAGE", CV_WINDOW_AUTOSIZE);
	while (std::getline(fsImage, sImage_line) && !sImage_line.empty())
	{
		std::istringstream ssImgData(sImage_line);
		ssImgData >> sImgFileName >> dStampNSec
				 >> dDrop >> dDrop >> dDrop >> dDrop
				 >> dDrop >> dDrop >> dDrop
				 >> dDrop >> dDrop >> dDrop
				 >> dDrop >> dDrop >> dDrop;
		// cout << "Image t : " << fixed << dStampNSec << " Name: " << sImgFileName << endl;
		string imagePath = "./keyframe/all_points_" + sImgFileName + ".txt";

		// cout << "imagePath : " << imagePath << endl;

		fsFeatures.open(imagePath.c_str());
		if (!fsFeatures.is_open())
		{
			cerr << "Failed to open features file! " << imagePath << endl;
			return;
		}

		shared_ptr<IMG_MSG> feature_points(new IMG_MSG());

		while (std::getline(fsFeatures, sFeatures_line) && !sFeatures_line.empty())
		{
			std::istringstream ssFeaturesData(sFeatures_line);
			ssFeaturesData >> p_id >> dDrop >> dDrop >> dDrop >> dDrop >> dPu >> dPv;

			feature_points->header = dStampNSec;
			feature_points->points.push_back(Vector3d(dPu, dPv, 1.0));
			feature_points->id_of_point.push_back(p_id);
			feature_points->u_of_point.push_back(dPu);
			feature_points->v_of_point.push_back(dPv);
			feature_points->velocity_x_of_point.push_back(0.0);
			feature_points->velocity_y_of_point.push_back(0.0);
		}

		// cout << "frame_t: " << feature_points->header << endl;
		// unsigned int nn = feature_points->points.size();
		// for(unsigned int z=0; z<nn; z++)
		// {
		// 	cout << "id: " << feature_points->id_of_point[z] 
		// 		<< " u: " << feature_points->u_of_point[z] 
		// 		<< " v: " <<feature_points->v_of_point[z] 
		// 		<< endl;
		// }

		// cout << "Features t: " << fixed << dStampNSec << "id: " << p_id << " u: " << dPu << " v: " << dPv << endl;
		
		// Mat img = imread(imagePath.c_str(), 0);
		// if (img.empty())
		// {
		// 	cerr << "image is empty! path: " << imagePath << endl;
		// 	return;
		// }
		pSystem->PubImageData(dStampNSec, feature_points);

		fsFeatures.close(); 

		// cv::imshow("SOURCE IMAGE", img);
		// cv::waitKey(0);
		usleep(50000*nDelayTimes);
	}
	fsImage.close();
	/************************************ 修改部分 end ******************************************/
}

#ifdef __APPLE__
// support for MacOS
void DrawIMGandGLinMainThrd(){
	string sImage_file = sConfig_path + "MH_05_cam0.txt";

	cout << "1 PubImageData start sImage_file: " << sImage_file << endl;

	ifstream fsImage;
	fsImage.open(sImage_file.c_str());
	if (!fsImage.is_open())
	{
		cerr << "Failed to open image file! " << sImage_file << endl;
		return;
	}

	std::string sImage_line;
	double dStampNSec;
	string sImgFileName;

	pSystem->InitDrawGL();
	while (std::getline(fsImage, sImage_line) && !sImage_line.empty())
	{
		std::istringstream ssImuData(sImage_line);
		ssImuData >> dStampNSec >> sImgFileName;
		// cout << "Image t : " << fixed << dStampNSec << " Name: " << sImgFileName << endl;
		string imagePath = sData_path + "cam0/data/" + sImgFileName;

		Mat img = imread(imagePath.c_str(), 0);
		if (img.empty())
		{
			cerr << "image is empty! path: " << imagePath << endl;
			return;
		}
		//pSystem->PubImageData(dStampNSec / 1e9, img);
		cv::Mat show_img;
		cv::cvtColor(img, show_img, CV_GRAY2RGB);
		if (SHOW_TRACK)
		{
			for (unsigned int j = 0; j < pSystem->trackerData[0].cur_pts.size(); j++)
			{
				double len = min(1.0, 1.0 *  pSystem->trackerData[0].track_cnt[j] / WINDOW_SIZE);
				cv::circle(show_img,  pSystem->trackerData[0].cur_pts[j], 2, cv::Scalar(255 * (1 - len), 0, 255 * len), 2);
			}

			cv::namedWindow("IMAGE", CV_WINDOW_AUTOSIZE);
			cv::imshow("IMAGE", show_img);
		  // cv::waitKey(1);
		}

		pSystem->DrawGLFrame();
		usleep(50000*nDelayTimes);
	}
	fsImage.close();

} 
#endif

int main(int argc, char **argv)
{
	if(argc != 3)
	{
		cerr << "./run_euroc PATH_TO_FOLDER/MH-05/mav0 PATH_TO_CONFIG/config \n" 
			<< "For example: ./run_euroc /home/stevencui/dataset/EuRoC/MH-05/mav0/ ../config/"<< endl;
		return -1;
	}
	sData_path = argv[1];
	sConfig_path = argv[2];

	pSystem.reset(new System(sConfig_path));
	
	std::thread thd_BackEnd(&System::ProcessBackEnd, pSystem);
		
	// sleep(5);
	std::thread thd_PubImuData(PubImuData);

	std::thread thd_PubImageData(PubImageData);

#ifdef __linux__	
	std::thread thd_Draw(&System::Draw, pSystem);
#elif __APPLE__
	DrawIMGandGLinMainThrd();
#endif

	thd_PubImuData.join();
	thd_PubImageData.join();

	// thd_BackEnd.join();
	thd_Draw.join();

	cout << "main end... see you ..." << endl;

	return 0;
}
