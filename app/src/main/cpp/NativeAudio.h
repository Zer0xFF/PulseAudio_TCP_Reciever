#pragma once

#include <oboe/Oboe.h>

class CNativeAudio
{
public:
    CNativeAudio();
    ~CNativeAudio();

    void SetupAudioStream(oboe::AudioStreamCallback*);
    void CloseAudioStream();
    bool isValidStream();

private:
    oboe::AudioStream *_stream = nullptr;
    bool _isValidStream = false;


};
