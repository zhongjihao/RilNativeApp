本工程不支持NDK编译，需要在Android源码环境根目录下执行如下 \
make RilService -j16 \
然后 执行 \
adb root \
adb remount \
adb push 生成的RilService.apk          /system/app/RilService/ \
adb push 生成的libgameserver.so        /system/lib/ \
adb push 生成的libril_native.so        /system/lib/ \
adb reboot

本工程分为两大部分： \
 一、电话模块Ril服务端部分，见jni/rild目录 \
 二、棋牌游戏服务端部分，见jni/gameserver目录

   
