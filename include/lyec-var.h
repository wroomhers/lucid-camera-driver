// include/lyec-var.h
#ifndef LYEC_VAR_H
#define LYEC_VAR_H

#include "stdafx.h"
#include "ArenaApi.h"
#include <opencv2/opencv.hpp>
#include "opencv2/photo.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include <arena_camera/arena_camera_node.h>
#include <yaml-cpp/yaml.h>
#include <ros/ros.h>
#include <string.h>
#include <lyec-var.h>

#include "std_msgs/String.h" 
#include <sensor_msgs/Image.h>
#include <sensor_msgs/CameraInfo.h>
#include <sstream> 
#include <boost/thread.hpp>
#include <thread> // İş parçacığı (thread) kütüphanesi eklendi

#define TAB1 "  "
#define TAB2 "    "


#define TIMEOUT 2000

#define PIXEL_FORMAT BGR8
cv::Mat yesim;
cv::Mat img;
cv::Mat cropped_image;
cv::Mat hsv;
cv::Mat bgr;

int gencadamsinlazimoluyor = 0;
Arena::IImage *murat;
ros::Publisher pub;
ros::Publisher pubc;
 //YAML::Node config = YAML::LoadFile("/home/wroomhers/lyec-driver/src/lyec-camera-driver/coasdnfig/config.yaml");
 //YAML::Node ost = YAML::LoadFile(config["ost_file"].as<std::string>());

Arena::IDevice* pDevice = nullptr;
Arena::IImage* pImage = nullptr;
const uint8_t* pData = nullptr;
GenApi::INodeMap *pNodeMap = nullptr;
GenApi::CFloatPtr pExposureTimeNode;
int64_t getImageTimeout_ms;
int onceki_parlakliklar[10];
int zaman[2];

float gain_degeri = 0.0;
// float d_matris[]  = ost["distortion_coefficients"]["data"].as<float>();
// float p_matris[12]  = ost["projection_matrix"]["data"].as<float>();
// float k_matris[9]  = ost["camera_matrix"]["data"].as<float>();
// float r_matris[9] = ost["rectification_matrix"]["data"].as<float>();
    
class launch_param
{
public:
    YAML::Node config;
    YAML::Node ost;
    bool ost_defined = 0;

};
class cams
{
public:
    Arena::ISystem* pSystem = nullptr;

    int64_t image_height;
    int64_t image_width;
    // std::string kamera_adi = config["camera_name"].as<std::string>();
    std::string kamera_adi;
    int64_t binning_x;
    int64_t binning_y;
};
class AOI_Param
{
public:
    int x,
        y,
        genislik,
        yukseklik;
    int y_sol,
	    y_sag,
	    x_ust,
	    x_alt;
};

cams cihaz;
AOI_Param aoi;
std::string dist_mode;
launch_param launch;

bool ayarlanabilir_fps;

int fps;





float filterelenmis_parlaklik = 0;
// void sure_bakani(int ilk_zaman, int son_zaman, char kimim_ben){
// 	int gecen_sure = son_zaman - ilk_zaman;
// 	std::cout << kimim_ben << "'den gecen sure: " << gecen_sure << std::endl; 
// }
cv::Scalar avg;
double per;






int tahmini_max_pozlama;
float amaclanan_parlaklik;


size_t width;
size_t height;
cv::Mat RGB;
cv::Mat Rect_RGB;

std::vector<Arena::DeviceInfo>::iterator it;
std::vector<Arena::DeviceInfo> deviceInfos;

#endif // LYEC_VAR_H