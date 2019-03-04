#include <jni.h>
#include <oboe/Oboe.h>
#include "NativeAudio.h"
#include "AudioNet.h"
#include "NativeAudioCallback.h"

CAudioNet *audioNet;
CNativeAudio *nativeAudio;
CNativeAudio_Callback *audioCallback;

extern "C" JNIEXPORT jint JNICALL Java_net_madnation_pulseaudio_NativeLibs_Start(JNIEnv *env, jobject obj /*unused*/, jstring hostString, jint port)
{
    auto dirPath = env->GetStringUTFChars(hostString, 0);
    audioNet = new CAudioNet(dirPath, port, true, 5);
    if(int res = audioNet->Connect() != 0)
    {
        audioNet->Close();
        delete audioNet;
        audioNet = nullptr;
        return res;
    }
    audioNet->Start();

    audioCallback = new CNativeAudio_Callback(audioNet);

    nativeAudio = new CNativeAudio();
    nativeAudio->SetupAudioStream(static_cast<oboe::AudioStreamCallback *>(audioCallback));
    if(!nativeAudio->isValidStream())
    {
        nativeAudio->CloseAudioStream();
        delete nativeAudio;
        nativeAudio = nullptr;
       return -3;
    }
    return 0;
}

extern "C" JNIEXPORT void JNICALL Java_net_madnation_pulseaudio_NativeLibs_Stop(JNIEnv *env, jobject obj /*unused*/)
{

    if (nativeAudio)
        nativeAudio->CloseAudioStream();
    delete nativeAudio;
    nativeAudio = nullptr;

    if (audioNet)
        audioNet->Close();
    delete audioNet;
    audioNet = nullptr;

}
