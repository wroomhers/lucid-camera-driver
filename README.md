# LYEC Camera Driver

Lucid kameralar için ROS tabanlı kamera sürücüsü. Bu paket Arena SDK kullanarak Lucid (eski adıyla Point Grey / FLIR) kameralarını ROS ortamında kontrol etmek için geliştirilmiştir.

## İçindekiler

- [Özellikler](#özellikler)
- [Gereksinimler](#gereksinimler)
- [Kurulum](#kurulum)
- [Kullanım](#kullanım)
- [Konfigürasyon](#konfigürasyon)
- [Launch Dosyaları](#launch-dosyaları)
- [Parametreler](#parametreler)
- [Topics](#topics)
- [Sorun Giderme](#sorun-giderme)

## Özellikler

- **Lucid Arena SDK desteği**: En son Arena SDK ile uyumlu
- **Otomatik parlaklık kontrolü**: AOI (Area of Interest) tabanlı akıllı pozlama ayarı
- **Manuel kamera kontrolü**: Pozlama, gain, fps gibi parametrelerin manuel kontrolü
- **Çoklu kamera desteği**: Birden fazla kameranın eşzamanlı kullanımı
- **Kamera kalibrasyonu**: OpenCV uyumlu kamera kalibrasyonu desteği
- **ROS entegrasyonu**: Standart ROS görüntü mesajları ve servisler
- **Gerçek zamanlı performans**: Düşük gecikme ile görüntü yayını

## Gereksinimler

### Sistem Gereksinimleri
- Ubuntu 18.04+ / ROS Melodic+ veya Ubuntu 20.04+ / ROS Noetic
- OpenCV 3.2+
- CMake 3.5+
- C++11 uyumlu derleyici

### ROS Bağımlılıkları
```bash
sudo apt-get install ros-$ROS_DISTRO-camera-info-manager \
                     ros-$ROS_DISTRO-image-transport \
                     ros-$ROS_DISTRO-cv-bridge \
                     ros-$ROS_DISTRO-sensor-msgs \
                     ros-$ROS_DISTRO-diagnostic-updater
```

### Arena SDK
Lucid Vision Labs'dan Arena SDK'yı indirip kurun:
1. [Lucid Vision Labs](https://thinklucid.com/downloads-hub/) web sitesinden Arena SDK'yı indirin
2. SDK'yı sisteminize kurun
3. Gerekli çevre değişkenlerini ayarlayın

## Kurulum

### 1. Workspace Oluşturma
```bash
mkdir -p ~/catkin_ws/src
cd ~/catkin_ws/src
```

### 2. Paketi Klonlama
```bash
git clone <repository-url> lyec_camera
```

### 3. Bağımlılıkları Kurma
```bash
cd ~/catkin_ws
rosdep install --from-paths src --ignore-src -r -y
```

### 4. Derleme
```bash
catkin_make
source devel/setup.bash
```

## Kullanım

### Tek Kamera Başlatma
```bash
roslaunch lyec_camera mono.launch
```

### Çoklu Kamera Başlatma
```bash
roslaunch lyec_camera multi_cam.launch
```

### Stereo Kamera Sistemi
```bash
# Sol kamera
roslaunch lyec_camera left.launch

# Sağ kamera (başka terminalde)
roslaunch lyec_camera right.launch
```

## Konfigürasyon

Kamera ayarları `config/config.yaml` dosyasında yapılandırılır:

```yaml
# Görüntü ayarları
image_width: 1024
image_height: 768
binning_x: 2
binning_y: 2

# FPS ayarları
ayarlanabilir_fps: true
fps: 30

# Kamera adı
camera_name: tis_front_cam

# Otomatik parlaklık
auto_brightnes: false

# Kalibrasyon dosyası
ost_file: /path/to/calibration/ost.yaml

# AOI (Area of Interest) ayarları
AOI: 
  AOI_On: false
  AOI_x: 472
  AOI_y: 610
  AOI_genislik: 600
  AOI_yukseklik: 200
  AOI_Rectangle: false

# Manuel parlaklık kontrolü
manual_brightnes: 
  exposure: 100
  ISO: 100
```

### AOI (Area of Interest) Özelliği

AOI özelliği, görüntünün belirli bir bölgesindeki parlaklığa göre otomatik pozlama ayarı yapar:

- `AOI_On`: AOI özelliğini aktif/pasif yapar
- `AOI_x`, `AOI_y`: AOI bölgesinin merkez koordinatları
- `AOI_genislik`, `AOI_yukseklik`: AOI bölgesinin boyutları
- `AOI_Rectangle`: AOI bölgesini görüntü üzerinde gösterir

## Launch Dosyaları

### mono.launch
Tek kamera için temel launch dosyası:
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

### Özelleştirme
Launch dosyalarını ihtiyacınıza göre özelleştirebilirsiniz:
```bash
roslaunch lyec_camera mono.launch cam_name:=my_camera config_file:=/path/to/my_config.yaml
```

## Parametreler

| Parametre | Tip | Varsayılan | Açıklama |
|-----------|-----|------------|----------|
| `config_file` | string | config.yaml | Konfigürasyon dosyası yolu |
| `cam_name` | string | - | Kamera adı |
| `ost_file` | string | - | Kalibrasyon dosyası yolu |
| `fps` | int | 30 | Hedef FPS değeri |
| `auto_brightness` | bool | false | Otomatik parlaklık kontrolü |

## Topics

### Published Topics
- `/camera/image_raw` (sensor_msgs/Image): Ham kamera görüntüsü
- `/camera/camera_info` (sensor_msgs/CameraInfo): Kamera kalibrasyon bilgileri

### Subscribed Topics
Bu paket herhangi bir topic'e abone olmaz.

## Services

- `/camera/set_brightness`: Parlaklık ayarı
- `/camera/set_exposure`: Pozlama ayarı
- `/camera/set_binning`: Binning ayarı

## Kamera Kalibrasyonu

Kamera kalibrasyonu için OpenCV kalibrasyon dosyalarını kullanın:

1. `intrinsics/ost.yaml` dosyasını kamera kalibrasyon verilerinizle güncelleyin
2. Launch dosyasında `ost_file` parametresini ayarlayın

## Performans İpuçları

1. **FPS Optimizasyonu**: Gereksiz yüksek FPS değerlerinden kaçının
2. **AOI Kullanımı**: Sadece önemli bölgeyi izlemek için AOI kullanın
3. **Binning**: Düşük çözünürlük yeterli ise binning kullanın
4. **USB Bant Genişliği**: USB 3.0 bağlantısı kullanın

## Sorun Giderme

### Kamera Bulunamıyor
```bash
# Arena SDK kurulumunu kontrol edin
ls /opt/arena_sdk/

# USB bağlantısını kontrol edin
lsusb | grep -i lucid
```

### Düşük FPS
- USB bant genişliğini kontrol edin
- Görüntü boyutunu azaltın
- Binning kullanın

### Görüntü Gelmemiyor
- Kamera konfigürasyonunu kontrol edin
- ROS topic'lerini kontrol edin: `rostopic list`
- Node loglarını inceleyin: `rosnode info lyec_camera_node`

### Hata Logları
```bash
# ROS loglarını görüntüleme
rqt_console

# Terminal çıktısını izleme
roslaunch lyec_camera mono.launch --screen
```

## Lisans

Bu proje BSD lisansı altında dağıtılmaktadır. Detaylar için LICENSE dosyasına bakın.

## Katkıda Bulunma

1. Bu repository'yi fork edin
2. Feature branch oluşturun (`git checkout -b feature/amazing-feature`)
3. Değişikliklerinizi commit edin (`git commit -m 'Add amazing feature'`)
4. Branch'ınızı push edin (`git push origin feature/amazing-feature`)
5. Pull Request oluşturun

## İletişim

**Geliştirici**: Lutfu Aydin UCAR  
**E-posta**: lutfuucar8@gmail.com

## Versiyon Geçmişi

- **v0.0.0**: İlk sürüm
  - Temel kamera sürücüsü
  - AOI tabanlı otomatik pozlama
  - Çoklu kamera desteği

---

*Bu README dosyası sürekli güncellenmektedir. Son sürüm için repository'yi kontrol edin.*
