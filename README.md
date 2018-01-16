# Raspbery Pi専用カメラで顔認識する

以下の環境セットアップを実施後、makeしてGO!!  

## 動作環境

* Raspberry Pi 3(ZeroでもOK)
* 専用カメラ

## Raspbianをインストールする  
http://karaage.hatenadiary.jp/entry/2015/07/15/080000

たぶんapt-getを更新しておいたほうがよい。  
`sudo apt-get update`  
`sudo apt-get upgrade`  
パスワードは raspberry です。

## OpenCVをインストールする  
`$ sudo apt-get install libopencv-dev`

## 専用カメラをOpenCVから操作するためのライブラリをインストールする  
https://github.com/robidouille/robidouille/tree/master/raspicam_cv

上記手順後、以下コマンドでファイルを移動させる  
`sudo cp /home/pi/git/robidouille/raspicam_cv/libraspicamcv.so /usr/lib`  
`sudo cp /home/pi/git/robidouille/raspicam_cv/RaspiCamCV.h /usr/include`

追記：
ライブラリのビルドは面倒なので、ビルド済みファイルを用意した。  
`git clone https://github.com/hiroshi-mikuriya/facedetect`  
`sudo mv facedetect/raspicamcv/RaspiCamCV.h /usr/include`  
`sudo mv facedetect/raspicamcv/libraspicamcv.so /usr/lib`


## カメラを有効にする
http://www.neko.ne.jp/~freewing/raspberry_pi/raspberry_pi_3_camera_setup/
* `sudo apt-get upgrade`
* `sudo raspi-config`
  * 5 Interfacing Options
  * P1 Camera
  * Yes
  * OK
  * Finish
