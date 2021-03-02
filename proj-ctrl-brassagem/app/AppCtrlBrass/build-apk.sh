cd '/home/r2d2/Documents/ifsc/9P/pi 3/código/proj-ctrl-brassagem/app/AppCtrlBrass/android'
ANDROID_PATH=/home/r2d2/Documents/ifsc/9P/pi\ 3/código/proj-ctrl-brassagem/app/AppCtrlBrass/android
BUILD_PATH=./app/build/outputs/bundle/release

./gradlew bundleRelease
java -jar ~/Downloads/bundletool-all-1.4.0.jar build-apks --bundle=$BUILD_PATH/app-release.aab --output=$BUILD_PATH/apk/app.apks --ks=./app/my-upload-key.keystore --ks-key-alias=my-key-alias --mode=universal
