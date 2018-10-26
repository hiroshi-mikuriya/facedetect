# PiCameraで顔認識する

## 動作環境

* Raspberry Pi 3(ZeroでもOK)
* PiCamera
* SDカード16GB

## Raspbianをインストールする  
http://karaage.hatenadiary.jp/entry/2015/07/15/080000

たぶんapt-getを更新しておいたほうがよい。  
`sudo apt-get update`  
`sudo apt-get upgrade`  
パスワードは raspberry です。

## OpenCVをインストールする  
`$ sudo apt-get install libopencv-dev`

## カメラを有効にする

* OpenCVのVideoCaptureからカメラ画像取得可能にする  
`sudo modprobe bcm2835-v4l2`
* PiCameraを使用可能にする。（カメラがないとRaspbianが起動しなくなる）  
`sudo raspi-config`
  * 5 Interfacing Options
  * P1 Camera
  * Yes
  * OK
  * Finish
