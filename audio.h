#pragma once
#include "SystemVolume.h"
#include <iostream>
#include "SystemVolume.h"
#include <audiopolicy.h>
#include <psapi.h>
#include <mfapi.h>
#include <atlstr.h>

#ifndef SRC_DEMO_H  
#define SRC_DEMO_H  
#define REFTIMES_PER_SEC  10000000

class MyAudioStreamVolume {
private:
	HRESULT hr;
	IAudioClient *pAudioClient = 0;
	IMMDeviceEnumerator *pDeviceEnumerator = 0;
	IMMDevice *pDevice = 0;

	IAudioEndpointVolume *pAudioEndpointVolume = 0;
	IAudioStreamVolume *pVolume = 0;
	ISimpleAudioVolume *pSimpleAudioVolume = 0;


	WAVEFORMATEX *pwfx = NULL;

	REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;

	UINT32 nFrames = 0;

	IAudioSessionManager *pSessionMan = nullptr;
	IAudioSessionManager2 *pSessionMan2 = nullptr;
	IAudioSessionEnumerator *pSessionEnum = nullptr;
	IAudioSessionControl *pSessionControl = nullptr;
	IAudioSessionControl2 *pSessionControl2 = nullptr;


public:
	void getCount();
};
#endif