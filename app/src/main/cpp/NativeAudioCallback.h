#pragma once

#include <oboe/Oboe.h>
#include "NativeAudio.h"
#include "AudioNet.h"

class CNativeAudio_Callback : public oboe::AudioStreamCallback
{
    public:
        CAudioNet* _audioNet;

        CNativeAudio_Callback(CAudioNet* audioNet)
        {
            _audioNet = audioNet;
        }

        oboe::DataCallbackResult onAudioReady(oboe::AudioStream *audioStream, void *audioData, int32_t numFrames)
        {
            auto *outputData = static_cast<char *>(audioData);
            if(_audioNet)
                _audioNet->Render(numFrames * audioStream->getBytesPerFrame(), outputData);

            return oboe::DataCallbackResult::Continue;
        }
};
