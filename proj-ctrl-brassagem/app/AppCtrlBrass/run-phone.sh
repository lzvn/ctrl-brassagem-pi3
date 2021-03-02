#echo 'SUBSYSTEM=="usb", ATTR{idVendor}=="04e8", MODE="0666", GROUP="plugdev"' | sudo tee /etc/udev/rules.d/51-android-usb.rules
#adb devices
export ANDROID_SDK_ROOT=/home/r2d2/Android/Sdk/
terminator -e 'export ANDROID_SDK_ROOT=/home/r2d2/Android/Sdk/ && npx react-native run-android'
npm start
