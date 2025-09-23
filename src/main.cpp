#include "stdafx.h"
#include <lyec-var.h>


void tulfu_aoi(Arena::IDevice* pDevice, cv::Mat yesim)
{
	// TULFUU
	cropped_image = yesim(cv::Range(aoi.y_sol, aoi.y_sag), cv::Range(aoi.x_ust, aoi.x_alt));
	cvtColor(cropped_image, hsv, cv::COLOR_RGB2HSV);

	avg = cv::mean(hsv)[2];
	per = (avg.val[0] / 255) * 100;
	if (launch.config["AOI"]["AOI_Rectangle"].as<bool>())
		cv::rectangle(yesim, cv::Point(aoi.x - (aoi.genislik / 2), aoi.y - (aoi.yukseklik / 2)), cv::Point(aoi.x + (aoi.genislik / 2), aoi.y + (aoi.yukseklik / 2)), (255, 0, 0), 4, cv::LINE_8);
	// cvtColor(yesim, yesim, cv::COLOR_BGR2RGB);
	tahmini_max_pozlama = (fps > 10 ? ((1.0 / fps) * 1000000) * 0.98 : 99863) ;
	amaclanan_parlaklik = std::abs((((per / 3) - 32) * (tahmini_max_pozlama - 32) * 8) / ((200 - 1)));
	filterelenmis_parlaklik += 0.1 * (amaclanan_parlaklik - filterelenmis_parlaklik);
	if (filterelenmis_parlaklik < 33)
	{
		filterelenmis_parlaklik = 33;
		ROS_INFO("Gardas senin pozlama cok kucuktu ben buyuttum");
	}
	
	else if (filterelenmis_parlaklik > tahmini_max_pozlama)
	{
		gain_degeri++;
		Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "Gain", gain_degeri);
		filterelenmis_parlaklik = tahmini_max_pozlama;
		ROS_INFO("Gardas senin pozlama cok buyuktu ben kuculttum");
	}
	else if (filterelenmis_parlaklik < (tahmini_max_pozlama / 2)){
		if(gain_degeri > 0){
			gain_degeri--;
			Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "Gain", gain_degeri);
		}
	}
	for (int i = 10; i > 0; i--) {
        onceki_parlakliklar[i] = onceki_parlakliklar[i - 1];
    }

    // Add the element at the beginning of the array
    onceki_parlakliklar[0] = filterelenmis_parlaklik;



	//onceki_parlakliklar.push_front(filterelenmis_parlaklik);
	// zaman[0] = ros::Time::now().sec;
	// if (zaman[0] > zaman[1] + 1){
	// 	zaman[1] = zaman[0];
	// 	std::cout << "1 saniye gecti" << std::endl;
	// }
	if(std::abs(onceki_parlakliklar[0] - onceki_parlakliklar[10]) > 100){
		Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime", filterelenmis_parlaklik);
		std::cout << "PARLAKLIK ORANI :" << filterelenmis_parlaklik << std::endl;
	}
		// Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "Gain", 0.3);

	// cv::namedWindow("AOI Gorseli",cv::WINDOW_NORMAL);
	// cv::resizeWindow("AOI Gorseli", 480, 340);
	// cv::imshow("AOI Gorseli", yesim);
	//return filterelenmis_parlaklik;
	// std::cout << "Gorsel Yuksekligi: " << img_raw_msg_.height << " Gorsel Genisligi : " << img_raw_msg_.width << std::endl;
}
void GorselPaylas(cv_bridge::CvImage ros_image)
{
	try
	{
		/* code */
		sensor_msgs::ImagePtr msg = ros_image.toImageMsg();
		msg->header.frame_id = cihaz.kamera_adi;
		msg->header.stamp = ros::Time::now();
		pub.publish(msg);

		if (launch.ost_defined)
		{
			float d_matris[5];
			const auto &d_data = launch.ost["distortion_coefficients"]["data"];
			for (int i = 0; i < 5; ++i)
			{
				d_matris[i] = d_data[i].as<float>();
			}

			// Initialize and parse projection matrix
			float p_matris[12];
			const auto &p_data = launch.ost["projection_matrix"]["data"];
			for (int i = 0; i < 12; ++i)
			{
				p_matris[i] = p_data[i].as<float>();
			}

			// Initialize and parse camera matrix
			float k_matris[9];
			const auto &k_data = launch.ost["camera_matrix"]["data"];
			for (int i = 0; i < 9; ++i)
			{
				k_matris[i] = k_data[i].as<float>();
			}

			// Initialize and parse rectification matrix
			float r_matris[9];
			const auto &r_data = launch.ost["rectification_matrix"]["data"];
			for (int i = 0; i < 9; ++i)
			{
				r_matris[i] = r_data[i].as<float>();
			}

			sensor_msgs::CameraInfo camera_bilgi;
			camera_bilgi.width = cihaz.image_width;
			camera_bilgi.height = cihaz.image_height;
			camera_bilgi.distortion_model = dist_mode;
			// Assuming d_matris, k_matris, r_matris, and p_matris are arrays of floats
			std::vector<double> distortion_coeffs(std::begin(d_matris), std::end(d_matris));

			boost::array<double, 9> intrinsic_matrix;
			std::copy(std::begin(k_matris), std::end(k_matris), intrinsic_matrix.begin());

			boost::array<double, 9> rotation_matrix;
			std::copy(std::begin(r_matris), std::end(r_matris), rotation_matrix.begin());

			boost::array<double, 12> projection_matrix;
			std::copy(std::begin(p_matris), std::end(p_matris), projection_matrix.begin());

			// Assign the converted values to the camera_bilgi object
			camera_bilgi.D = distortion_coeffs;
			camera_bilgi.K = intrinsic_matrix;
			camera_bilgi.R = rotation_matrix;
			camera_bilgi.P = projection_matrix;
			camera_bilgi.binning_x = 1;
			camera_bilgi.binning_y = 1;

			camera_bilgi.header.frame_id = cihaz.kamera_adi;
			camera_bilgi.header.stamp = msg->header.stamp;

			pubc.publish(camera_bilgi);
		}
	}
	catch (const std::exception &e)
	{
		std::cerr << e.what() << '\n';
	}
}
void PrintInfo(Arena::IDevice* pDevice)
{


	// int64_t genislik = 1920;
	// int64_t yukseklik = 1280;
	// Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "Width") = genislik;
	// Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "Height") = yukseklik;


	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "BinningHorizontalMode", "Average");
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "BinningVerticalMode", "Average");
	Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "BinningHorizontal", cihaz.binning_y);
	Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "BinningVertical", cihaz.binning_x);
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "BinningVerticalMode", "Average");
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "SensorShutterMode", "Global");
	Arena::SetNodeValue<bool>(pDevice->GetNodeMap(), "AcquisitionFrameRateEnable", 1);
	Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "AcquisitionFrameRate", fps);
	Arena::SetNodeValue<int64_t>(pDevice->GetNodeMap(), "Width", cihaz.image_width);


	
	Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "ExposureAuto", "Off");
	bool AOI_On=launch.config["AOI"]["AOI_On"].as<bool>();
	bool Oto_Parlaklik=launch.config["auto_brightnes"];	
	if ((!AOI_On) && (!Oto_Parlaklik)){
		int AOI_Off_exposure = launch.config["manual_brightnes"]["exposure"].as<int>();
		int AOI_Off_ISO = launch.config["manual_brightnes"]["ISO"].as<int>();
		Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "ExposureTime", AOI_Off_exposure);
		Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "Gain", AOI_Off_ISO);

	}
}

// cv::Mat Gorsel_Duzelt(cv::Mat gorsel){
//     cv::Mat cameraMatrix(3, 3, CV_64F);
//     for (int i = 0; i < 3; ++i) {
//         for (int j = 0; j < 3; ++j) {
//             cameraMatrix.at<double>(i, j) = ost["camera_matrix"]["data"][i * 3 + j].as<double>();
//         }
//     }
// 	cv::Mat distortionCoeffs(1, 5, CV_64F);
//     for (int i = 0; i < 5; ++i) {
//         distortionCoeffs.at<double>(i) = ost["distortion_coefficients"]["data"][i].as<double>();
//     }
// 	cv::Mat undistorted;
// 	cv::undistort(gorsel, undistorted, cameraMatrix, distortionCoeffs);
// 	return undistorted;
// }

void AcquireImages(Arena::IDevice* pDevice)
{
PrintInfo(pDevice);

    std::cout << "Devamli Goruntu Modu\n";
    Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", "Continuous");
    Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetTLStreamNodeMap(), "StreamBufferHandlingMode", "NewestOnly");
    Arena::SetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamAutoNegotiatePacketSize", true);
    Arena::SetNodeValue<bool>(pDevice->GetTLStreamNodeMap(), "StreamPacketResendEnable", true);
	Arena::SetNodeValue<double>(pDevice->GetNodeMap(), "Gain", 0);

    pDevice->StartStream();

    while (ros::ok()) {
        pImage = pDevice->GetImage(TIMEOUT);

        if (pImage != nullptr) {
            width = pImage->GetWidth();
            height = pImage->GetHeight();
            yesim = cv::Mat((int)height, (int)width, CV_8UC1, (void*)pImage->GetData());
            cv::cvtColor(yesim, RGB, cv::COLOR_BayerRG2RGB);
            //Rect_RGB = Gorsel_Duzelt(RGB);
			cv::Mat Rect_RGB = RGB;
            tulfu_aoi(pDevice, Rect_RGB);

            cv_bridge::CvImage ros_image;
            ros_image.encoding = sensor_msgs::image_encodings::BGR8;
            ros_image.image = Rect_RGB;


			boost::thread acquirerThread(&GorselPaylas, ros_image);
		   	acquirerThread.join();




            pDevice->RequeueBuffer(pImage);
        }
    }

    std::cout << "Dahasisi beni asar babacim\n";
    pDevice->StopStream();
    Arena::SetNodeValue<GenICam::gcstring>(pDevice->GetNodeMap(), "AcquisitionMode", "Continuous");
}

int main(int argc, char** argv)
{
    ros::init(argc, argv, "LYEC_Driver");
    ros::NodeHandle nh_;
    ros::NodeHandle nh_c;


	std::string string_var;
	std::string ost_var;
    // Parametrenin tam yolunu kontrol et

        ros::param::get("/lyec_camera_node/config_file", string_var);
		ros::param::get("/lyec_camera_node/cam_name", cihaz.kamera_adi);

		launch.config = YAML::LoadFile(string_var);
		pub = nh_.advertise<sensor_msgs::Image>(("lucid/" + cihaz.kamera_adi + "/image_raw"), 1);
    	pubc = nh_c.advertise<sensor_msgs::CameraInfo>(("lucid/" + cihaz.kamera_adi+"/camera_info"), 1);
 		try  {
			ros::param::get("/lyec_camera_node/ost_file", ost_var);
			launch.ost = YAML::LoadFile(ost_var);
			dist_mode = launch.ost["distortion_model"].as<std::string>();
			ROS_INFO_ONCE("OST dosyani buldum aga");
			launch.ost_defined = 1;
		}
		catch(const std::exception& e){
			ROS_ERROR("OST dosyan bizi bozdu aga");
		}


		std::cout << "bak hele bak:" << string_var << std::endl;

		cihaz.image_height = launch.config["image_height"].as<int>();
		cihaz.image_width = launch.config["image_width"].as<int>();
		// std::string kamera_adi = config["camera_name"].as<std::string>();

		cihaz.binning_x = launch.config["binning_x"].as<int>();
		cihaz.binning_y = launch.config["binning_y"].as<int>();

		ayarlanabilir_fps = launch.config["ayarlanabilir_fps"].as<bool>();

		fps = launch.config["fps"].as<int>();
		aoi.x = launch.config["AOI"]["AOI_x"].as<int>(),
		aoi.y = launch.config["AOI"]["AOI_y"].as<int>(),
		aoi.genislik = launch.config["AOI"]["AOI_genislik"].as<int>(),
		aoi.yukseklik = launch.config["AOI"]["AOI_yukseklik"].as<int>();

		aoi.y_sol = aoi.y - (aoi.yukseklik / 2),
		aoi.y_sag = aoi.y + (aoi.yukseklik / 2),
		aoi.x_ust = aoi.x - (aoi.genislik / 2),
		aoi.x_alt = aoi.x + (aoi.genislik / 2);

	// Parametrenin var olup olmadığını kontrol etmeden önce, ROS parametre listesini yazdır
    std::vector<std::string> params;
    ros::param::getParamNames(params);
    ROS_INFO("Mevcut parametreler:");
    for (const auto& param : params) {
        ROS_INFO("%s", param.c_str());
    }
    try {
        pSystem = Arena::OpenSystem();
        pSystem->UpdateDevices(100);
        deviceInfos = pSystem->GetDevices();

        if (deviceInfos.empty()) {
            std::cout << "\nKamera Bağlı Değil\nEnter Bas Baba\n";
            std::getchar();
            return 0;
        }
		//std::string device_user_id_to_open = "front_fight_cam";
      for (it = deviceInfos.begin(); it != deviceInfos.end(); ++it)
      {
		std::string device_user_id_found(it->UserDefinedName());
		// std::cout << "BAK KAMERALAR: " << device_user_id_found << std::endl;
			if (device_user_id_found == cihaz.kamera_adi){
				std::cout <<  "helal pampa" << std::endl;;
		        pDevice = pSystem->CreateDevice(*it);
				pNodeMap = pDevice->GetNodeMap();
           		pExposureTimeNode = pNodeMap->GetNode("ExposureTime");
           		getImageTimeout_ms = static_cast<int64_t>(pExposureTimeNode->GetMax() / 1000 * 2);
		
           		boost::thread acquirerThread(&AcquireImages, pDevice);
		   		acquirerThread.join();
           		std::cout << "\nROS Gitti\n";
		
           		pSystem->DestroyDevice(pDevice);
           		Arena::CloseSystem(pSystem);

			}
			std::cout <<  device_user_id_found << std::endl;
	  }
		
        
    } catch (GenICam::GenericException& ge) {
        std::cout << "\nKütüphane Hatası: " << ge.what() << "\n";
    } catch (std::exception& ex) {
        std::cout << "\nÇalışmıyor Çünkü: " << ex.what() << "\n";
    } catch (...) {
        std::cout << "\nHata Oluştu\n";
    }

    std::cout << "Enter Bas Baba\n";
    std::getchar();

    return 0;
}
