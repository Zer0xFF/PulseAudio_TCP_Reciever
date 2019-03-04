#include "NativeAudio.h"


#if defined(__ANDROID__) && !defined(NDEBUG)

#include <android/log.h>

#define TAG "NativeAudio"


#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR,    TAG, __VA_ARGS__)
#define LOGW(...) __android_log_print(ANDROID_LOG_WARN,     TAG, __VA_ARGS__)
#define LOGI(...) __android_log_print(ANDROID_LOG_INFO,     TAG, __VA_ARGS__)
#define LOGD(...) __android_log_print(ANDROID_LOG_DEBUG,    TAG, __VA_ARGS__)

#else

#define LOGE(...) printf(__VA_ARGS__)
#define LOGI(...) printf(__VA_ARGS__)
#define LOGW(...) printf(__VA_ARGS__)
#define LOGD(...) printf(__VA_ARGS__)

#endif
CNativeAudio::CNativeAudio()
{
}

CNativeAudio::~CNativeAudio()
{
    CloseAudioStream();
}


void CNativeAudio::SetupAudioStream(oboe::AudioStreamCallback* audioCallback)
{
    oboe::AudioStreamBuilder builder;

    builder.setDirection(oboe::Direction::Output);
    builder.setPerformanceMode(oboe::PerformanceMode::LowLatency);
    builder.setSharingMode(oboe::SharingMode::Exclusive);
    builder.setFormat(oboe::AudioFormat::I16);
    builder.setChannelCount(oboe::ChannelCount::Stereo);
    builder.setUsage(oboe::Usage::Media);
    builder.setSampleRate(48000);


    builder.setCallback(audioCallback);


    oboe::Result result = builder.openStream(&_stream);

    if (result != oboe::Result::OK)
    {
        LOGE("Failed to create stream. Error: %s", oboe::convertToText(result));
    }
    else
    {
        _stream->requestStart();
    }
    _isValidStream = result == oboe::Result::OK;
}

bool CNativeAudio::isValidStream()
{
    return _isValidStream;
}

void CNativeAudio::CloseAudioStream()
{
    if(_stream != nullptr)
    {
        _stream->close();
    }
    _isValidStream = false;

}