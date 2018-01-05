# Raspbery Pi専用カメラで顔認識する

## 環境セットアップ

* Raspberry Pi 3(Zeroでも検証する予定)
* 専用カメラ

以下URLに従って、Raspbianをインストールする  
http://karaage.hatenadiary.jp/entry/2015/07/15/080000

OpenCVをインストールする  
`$ sudo apt-get install libopencv-dev`

専用カメラをOpenCVから操作するためのライブラリをインストールする  
https://github.com/robidouille/robidouille/tree/master/raspicam_cv

上記手順後、以下コマンドでファイルを移動させる  
`sudo cp /home/pi/git/robidouille/raspicam_cv/libraspicamcv.so /usr/lib`  
`sudo cp /home/pi/git/robidouille/raspicam_cv/RaspiCamCV.h /usr/include`

以下を参考に専用カメラを有効にする。  
http://www.neko.ne.jp/~freewing/raspberry_pi/raspberry_pi_3_camera_setup/
