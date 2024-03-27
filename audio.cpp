#include "audio.h"


using namespace std;

using namespace SystemConf;

// int main() {
//     SystemVolume systemVolume;
//     try {
//
//         cout << "初始化 CoreAudioAPI " << endl;
//         systemVolume.init();
//
//         int volume = systemVolume.getVolume();
//         cout << "获取当前音量为:" << volume << endl;
//
//         cout << "设置音量为：80" << endl;
//         systemVolume.setVolume(80);
//
//         volume = systemVolume.getVolume();
//         cout << "获取当前音量为:" << volume << endl;
//
//         cout << "关闭服务" << endl;
//         systemVolume.close();
//     }catch (string e) {
//         cout << e << endl;
//     }catch (...) {
//         cout << "出现异常，关闭服务 释放资源" << endl;
//         systemVolume.close();
//     }
//     return 0;
// }

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p) \
if ((p)) {            \
delete (p);         \
(p) = NULL;         \
}
#endif

void MyAudioStreamVolume::getCount() {
    hr = CoInitialize(0);
    hr = CoCreateInstance(__uuidof(MMDeviceEnumerator), NULL, CLSCTX_ALL, __uuidof(IMMDeviceEnumerator),
                          (void **) &pDeviceEnumerator);
    if (FAILED(hr)) throw "InitException:pDeviceEnumerator is NULL;";


    hr = pDeviceEnumerator->GetDefaultAudioEndpoint(eRender, eMultimedia, &pDevice);
    if (FAILED(hr)) throw "InitException:pDevice is NULL";

    // Activate IAudioEndpointVolume
    hr = pDevice->Activate(__uuidof(IAudioEndpointVolume), CLSCTX_ALL, NULL, (void **) &pAudioEndpointVolume);
    if (FAILED(hr)) throw "pDevice->Active";


    /// 以下是WSA API部分代码
    // Activate IAudioClient
    hr = pDevice->Activate(__uuidof(IAudioClient), CLSCTX_ALL, NULL, (void **) &pAudioClient);
    if (FAILED(hr)) throw "pDevice->Active";

    // Get the device format.
    hr = pAudioClient->GetMixFormat(&pwfx);
    if (FAILED(hr)) throw "pAudioClient->GetMixFormat";

    hr = pAudioClient->Initialize(AUDCLNT_SHAREMODE_SHARED,
                                  AUDCLNT_STREAMFLAGS_EVENTCALLBACK,
                                  hnsRequestedDuration,
                                  hnsRequestedDuration,
                                  pwfx,
                                  NULL);
    if (FAILED(hr)) throw "pAudioClient->Initialize";


    // GetService IAudioStreamVolume
    hr = pAudioClient->GetService(__uuidof(IAudioStreamVolume), (void **) &pVolume);
    if (FAILED(hr)) throw " pAudioClient->GetService";


    UINT32 dwCount;
    pVolume->GetChannelCount(&dwCount);
    cout << "Channel count:" << dwCount << endl;

    /// TODO: 这种方法读取音量会出错
    float level;
    pVolume->GetAllVolumes(dwCount - 1, &level);
    cout << "GetAllVolumes: " << (int) round(level * 100.0) << endl;

    pVolume->GetChannelVolume(1, &level);
    cout << "GetChannelVolume: " << (int) round(level * 100.0) << endl;

    //
    hr = pAudioClient->GetService(__uuidof(ISimpleAudioVolume ),(void**)&pSimpleAudioVolume);
    if (FAILED(hr)) throw " pAudioClient->GetService";
    float pfLevel;
    pSimpleAudioVolume->GetMasterVolume(&pfLevel);          // 检测的是应用程序自己的
    cout << "GetMasterVolume: " << (int)round(pfLevel * 100.0) << endl;


    // Activate IAudioSessionManager
    hr = pDevice->Activate(__uuidof(IAudioSessionManager),CLSCTX_ALL, NULL, (void **) &pSessionMan);
    if (FAILED(hr)) throw " test";
    /// TODO: 获取音频会话控制器 疑似这里AudioSessionGuid设置为NULL，分配给默认会话
    // hr = pSessionMan->GetAudioSessionControl(nullptr,AUDCLNT_STREAMFLAGS_EVENTCALLBACK, &pSessionControl);
    // if (FAILED(hr)) throw " test";


    // // 获取进程ID 也是获取自己进程的
    // hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void **) &pSessionControl2);
    // if (FAILED(hr)) throw " test";

    // hr = pSessionControl2->IsSystemSoundsSession(); // 0 true  // 1 false
    // if (FAILED(hr)) throw " test";

    DWORD pid;
    // hr = pSessionControl2->GetProcessId(&pid);
    // HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    // WCHAR wszProcessName[MAX_PATH];
    // DWORD nChars = GetProcessImageFileNameW(hProcess, wszProcessName, MAX_PATH);
    // printf("Name: %ws\n", wszProcessName);
    // cout << "Process ID: " << pid << endl;


    // gemini    获取进程名称路径    也是获取自己的
    hr = pDevice->Activate(__uuidof(IAudioSessionManager2),CLSCTX_ALL, NULL, (void **) &pSessionMan2);
    if (FAILED(hr)) throw " test";
    hr = pSessionMan2->GetSessionEnumerator(&pSessionEnum);
    if (FAILED(hr)) throw " test";
    int count;
    hr = pSessionEnum->GetCount(&count);
    if (FAILED(hr)) throw " test";
    cout << "Session count: " << count << endl;

    // hr = pSessionEnum->GetSession(count - 1, &pSessionControl);
    // if (FAILED(hr)) throw " test";
    // hr = pSessionControl2->GetProcessId(&pid);
    // hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    // nChars = GetProcessImageFileNameW(hProcess, wszProcessName, MAX_PATH);
    // printf("Name: %ws\n", wszProcessName);
    LPWSTR pswSession = nullptr;
    for (int index = 0; index < count; index++) {
        // Get the <n>th session.
        // 获取音频会话对应的控制器
        hr = pSessionEnum->GetSession(index, &pSessionControl);

        // Get the extended session control interface pointer.
        hr = pSessionControl->QueryInterface(__uuidof(IAudioSessionControl2), (void **) &pSessionControl2);
        if (FAILED(hr)) throw " test";

        // 获取进程ID
        hr = pSessionControl2->GetProcessId(&pid);
        if (FAILED(hr)) throw " test";

        // 打开ID对应的进程
        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
        WCHAR wszProcessName[MAX_PATH];

        // 获取名称
        DWORD nChars = GetProcessImageFileNameW(hProcess, wszProcessName, MAX_PATH);
        printf("index: %d Name: %ws\n",index, wszProcessName);
        // cout << "Process ID: " << pid << endl;

        /// TODO: 所以说其实是不能那个通过获取进程名字来判断会话的
        hr = pSessionControl->SetDisplayName(L"hello",nullptr);

        hr = pSessionControl->GetDisplayName(&pswSession);

        wprintf_s(L"Session Name: %s\n", pswSession);
    }


    // GetService IAudioSessionControl
    // hr = pAudioClient->GetService(__uuidof(IAudioSessionControl), (void **) &pSessionControl);
    // if (FAILED(hr)) throw " test";
    // hr = pSessionEnum->GetSession(0, &pSessionControl);
    // if (FAILED(hr)) throw " test";
    // LPWSTR displayName;
    // hr = pSessionControl->GetIconPath(&displayName);
    // if (FAILED(hr)) throw " test";
    // cout << "Session display name: " << &displayName << endl;


    // 以下是EndpointVolume API 部分
    hr = pAudioEndpointVolume->GetMasterVolumeLevelScalar(&level);
    if (FAILED(hr)) throw "getVolume() throw Exception";
    cout << "Level: " << (int) round(level * 100.0) << endl;

    hr = pAudioEndpointVolume->GetChannelCount(&dwCount);
    cout << "Channel count:" << dwCount << endl;


}

/// TODO：能够读取应用的音量，识别出应用

int main() {
    MyAudioStreamVolume audio;
    audio.getCount();
    getchar();
    // audio.getCount();
    return 0;
}
