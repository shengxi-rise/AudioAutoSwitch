#pragma once

#ifndef _SystemVolume_h_

#include <windows.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <audioclient.h>

#define _SystemVolume_h_
namespace SystemConf {

    class SystemVolume {
    private:
        HRESULT hr;
        IMMDeviceEnumerator* pDeviceEnumerator = 0;
        IMMDevice* pDevice = 0;
        IAudioEndpointVolume* pAudioEndpointVolume = 0;
        IAudioClient* pAudioClient = 0;
    public:
        /**初始化服务*/
        void init();
        /**关闭服务 释放资源*/
        void close();
        /**设置音量*/
        void setVolume(int volume);
        /**获取系统音量*/
        int getVolume();
        /**静音*/
        void Mute();
        /**解除静音*/
        void UnMute();
    };

}
#endif
