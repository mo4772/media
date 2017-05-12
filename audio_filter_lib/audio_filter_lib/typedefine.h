#ifndef TYPEDEF_H
#define TYPEDEF_H

#include "agc/include/gain_control.h"
#include "ns/include/noise_suppression_x.h"
#include "aec/include/echo_cancellation.h"
#include "aec/aec_core.h"
#include "vad/include/webrtc_vad.h"
#include "other/signal_processing_library.h"
#include "other/speex_resampler.h"

    typedef struct _FilterStateZam{
        double x[3];
        double y[3];
        double a[3];
        double b[3];
    }FilterStateZam;

    typedef struct _Filter_Audio
    {
        NsxHandle *noise_sup_x;
        VadInst   *Vad_handle;
        void *gain_control, *echo_cancellation;
        uint32_t fs;

        WebRtcSpl_State48khzTo16khz state_in, state_in_echo;
        WebRtcSpl_State16khzTo48khz state_out;
        int32_t tmp_mem[496];

        int16_t msInSndCardBuf;

        FilterStateZam hpfa;
        FilterStateZam hpfb;
        FilterStateZam lpfa;
        FilterStateZam lpfb;

        SpeexResamplerState *downsampler;
        SpeexResamplerState *downsampler_echo;
        SpeexResamplerState *upsampler;

        int32_t split_filter_state_1[6];
        int32_t split_filter_state_2[6];
        int32_t split_filter_state_3[6];
        int32_t split_filter_state_4[6];

        int echo_enabled;
        int gain_enabled;
        int noise_enabled;
        int vad_enabled;
        int lowpass_enabled;
    }Filter_Audio;

#endif