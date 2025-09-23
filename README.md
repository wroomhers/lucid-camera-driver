# LYEC Camera Driver

ROS-based camera driver for Lucid cameras. This package is developed to control Lucid (formerly Point Grey / FLIR) cameras in ROS environment using Arena SDK.

## Table of Contents

- [Features](#features)
- [Requirements](#requirements)
- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Launch Files](#launch-files)
- [Parameters](#parameters)
- [Topics](#topics)
- [Troubleshooting](#troubleshooting)

## Features

- **Lucid Arena SDK support**: Compatible with latest Arena SDK
- **Automatic brightness control**: Smart exposure adjustment based on AOI (Area of Interest)
- **Manual camera control**: Manual control of parameters like exposure, gain, fps
- **Multi-camera support**: Simultaneous use of multiple cameras
- **Camera calibration**: OpenCV compatible camera calibration support
- **ROS integration**: Standard ROS image messages and services
- **Real-time performance**: Low latency image streaming

## Requirements

### System Requirements
- Ubuntu 18.04+ / ROS Melodic+ veya Ubuntu 20.04+ / ROS Noetic
- OpenCV 3.2+
- CMake 3.5+
- C++11 uyumlu derleyici

### ROS Dependencies
```bash
sudo apt-get install ros-$ROS_DISTRO-camera-info-manager \
                     ros-$ROS_DISTRO-image-transport \
                     ros-$ROS_DISTRO-cv-bridge \
                     ros-$ROS_DISTRO-sensor-msgs \
                     ros-$ROS_DISTRO-diagnostic-updater
```

### Arena SDK
Download and install Arena SDK from Lucid Vision Labs:
1. Download Arena SDK from [Lucid Vision Labs](https://thinklucid.com/downloads-hub/) website
2. Install the SDK on your system
3. Set up required environment variables

## Installation

### 1. Create Workspace
```bash
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
```

### 2. Clone Package
```bash
git clone https://github.com/wroomhers/lucid-camera-driver
```

### 3. Install Dependencies
```bash
cd ~/catkin_ws
rosdep install --from-paths src --ignore-src -r -y
```

### 4. Build
```bash
catkin_make
source devel/setup.bash
```

## Usage

### Launch Single Camera
```bash
roslaunch lyec_camera mono.launch
```

### Launch Multi-Camera
```bash
roslaunch lyec_camera multi_cam.launch
```

### Stereo Camera System
```bash
# Left camera
roslaunch lyec_camera left.launch

# Right camera (in another terminal)
roslaunch lyec_camera right.launch
```

## Configuration

Camera settings are configured in `config/config.yaml` file:

```yaml
# Image settings
image_width: 1024
image_height: 768
binning_x: 2
binning_y: 2

# FPS settings
ayarlanabilir_fps: true
fps: 30

# Camera name
camera_name: tis_front_cam

# Auto brightness
auto_brightnes: false

# Calibration file
ost_file: /path/to/calibration/ost.yaml

# AOI (Area of Interest) settings
AOI: 
  AOI_On: false
  AOI_x: 472
  AOI_y: 610
  AOI_genislik: 600
  AOI_yukseklik: 200
  AOI_Rectangle: false

# Manual brightness control
manual_brightnes: 
  exposure: 100
  ISO: 100
```

### AOI (Area of Interest) Feature

The AOI feature performs automatic exposure adjustment based on brightness in a specific region of the image:

- `AOI_On`: Enables/disables AOI feature
- `AOI_x`, `AOI_y`: Center coordinates of AOI region
- `AOI_genislik`, `AOI_yukseklik`: Dimensions of AOI region
- `AOI_Rectangle`: Shows AOI region on the image

## Launch Files

### mono.launch
Basic launch file for single camera:
```xml
<launch>
  <arg name="config_file" value="$(find lyec_camera)/config/config.yaml"/>
  <arg name="ost_file" value="$(find lyec_camera)/intrinsics/ost.yaml"/>
  <arg name="cam_name" value="front_right_cam"/>
  <node name="lyec_camera_node" pkg="lyec_camera" type="arena_camera_node" output="screen">
    <param name="config_file" value="$(arg config_file)"/>
    <param name="cam_name" value="$(arg cam_name)"/>
    <param name="ost_file" value="$(arg ost_file)"/>
  </node>
</launch>
```

### Customization
You can customize launch files according to your needs:
```bash
roslaunch lyec_camera mono.launch cam_name:=my_camera config_file:=/path/to/my_config.yaml
```

## Parameters

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `config_file` | string | config.yaml | Configuration file path |
| `cam_name` | string | - | Camera name |
| `ost_file` | string | - | Calibration file path |
| `fps` | int | 30 | Target FPS value |
| `auto_brightness` | bool | false | Auto brightness control |

## Topics

### Published Topics
- `/camera/image_raw` (sensor_msgs/Image): Raw camera image
- `/camera/camera_info` (sensor_msgs/CameraInfo): Camera calibration information

### Subscribed Topics
This package does not subscribe to any topics.

## Services

- `/camera/set_brightness`: Brightness adjustment
- `/camera/set_exposure`: Exposure adjustment
- `/camera/set_binning`: Binning adjustment

## Camera Calibration

Use OpenCV calibration files for camera calibration:

1. Update `intrinsics/ost.yaml` file with your camera calibration data
2. Set the `ost_file` parameter in launch file

## Performance Tips

1. **FPS Optimization**: Avoid unnecessarily high FPS values
2. **AOI Usage**: Use AOI to monitor only important regions
3. **Binning**: Use binning if low resolution is sufficient
4. **USB Bandwidth**: Use USB 3.0 connection

## Troubleshooting

### Camera Not Found
```bash
# Check Arena SDK installation
ls /opt/arena_sdk/

# Check USB connection
lsusb | grep -i lucid
```

### Low FPS
- Check USB bandwidth
- Reduce image size
- Use binning

### No Image Received
- Check camera configuration
- Check ROS topics: `rostopic list`
- Inspect node logs: `rosnode info lyec_camera_node`

### Error Logs
```bash
# View ROS logs
rqt_console

# Monitor terminal output
roslaunch lyec_camera mono.launch --screen
```

## License

This project is distributed under the BSD license. See LICENSE file for details.

## Contributing

1. Fork this repository
2. Create feature branch (`git checkout -b feature/amazing-feature`)
3. Commit your changes (`git commit -m 'Add amazing feature'`)
4. Push to the branch (`git push origin feature/amazing-feature`)
5. Create Pull Request

## Contact

**Developer**: wroomhers
**Email**: lutfuucar8@gmail.com

## Version History

- **v0.0.0**: Initial release
  - Basic camera driver
  - AOI-based automatic exposure
  - Multi-camera support
