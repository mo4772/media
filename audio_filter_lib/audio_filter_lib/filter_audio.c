
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "agc/include/gain_control.h"
#include "ns/include/noise_suppression_x.h"
#include "aec/include/echo_cancellation.h"
#include "aec/aec_core.h"
#include "vad/include/webrtc_vad.h"
#include "other/signal_processing_library.h"
#include "other/speex_resampler.h"
#include "zam/filters.h"



/*typedef struct {
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
} Filter_Audio;*/

//#define _FILTER_AUDIO
#include "filter_audio.h"


void kill_filter_audio(Filter_Audio *f_a)
{
    if (!f_a) {
        return;
    }

    WebRtcNsx_Free(f_a->noise_sup_x);
    WebRtcAgc_Free(f_a->gain_control);
    WebRtcAec_Free(f_a->echo_cancellation);
    WebRtcVad_Free(f_a->Vad_handle);
    speex_resampler_destroy(f_a->upsampler);
    speex_resampler_destroy(f_a->downsampler);
    speex_resampler_destroy(f_a->downsampler_echo);
    free(f_a);
}

Filter_Audio * new_filter_audio(uint32_t fs)
{
    Filter_Audio *f_a = NULL;
    unsigned int lowpass_filter_frequency = 12000;
    WebRtcAgc_config_t gain_config;
    AecConfig echo_config;
    int vad_mode = 1;  //Aggressiveness mode (0, 1, 2, or 3).
    int quality = 4;

    if (fs == 0) {
        return NULL;
    }

    f_a = (Filter_Audio *)calloc(sizeof(Filter_Audio), 1);
    if (!f_a) {
        return NULL;
    }

    f_a->fs = fs;

    /*if (fs != 16000)
        fs = 32000;*/

    //init_highpass_filter_zam(&f_a->hpfa, 100, (float) f_a->fs);
    //init_highpass_filter_zam(&f_a->hpfb, 100, (float) f_a->fs);

    
    /*if (f_a->fs > (lowpass_filter_frequency * 2)) {
        init_lowpass_filter_zam(&f_a->lpfa, lowpass_filter_frequency, (float) f_a->fs);
        init_lowpass_filter_zam(&f_a->lpfb, lowpass_filter_frequency, (float) f_a->fs);
        f_a->lowpass_enabled = 1;
    }*/

    if (WebRtcAgc_Create(&f_a->gain_control) == -1) {
        free(f_a);
        return NULL;
    }

    if (WebRtcNsx_Create(&f_a->noise_sup_x) == -1) {
        WebRtcAgc_Free(f_a->gain_control);
        free(f_a);
        return NULL;
    }

    if (WebRtcAec_Create(&f_a->echo_cancellation) == -1) {
        WebRtcAgc_Free(f_a->gain_control);
        WebRtcNsx_Free(f_a->noise_sup_x);
        free(f_a);
        return NULL;
    }

    if (WebRtcVad_Create(&f_a->Vad_handle) == -1){
        WebRtcAec_Free(f_a->echo_cancellation);
        WebRtcAgc_Free(f_a->gain_control);
        WebRtcNsx_Free(f_a->noise_sup_x);
        free(f_a);
        return NULL;
    }

    WebRtcAec_enable_delay_correction(WebRtcAec_aec_core(f_a->echo_cancellation), kAecTrue);
    WebRtcAec_enable_reported_delay(WebRtcAec_aec_core(f_a->echo_cancellation), kAecTrue);
    
    

    gain_config.targetLevelDbfs = 1;
    gain_config.compressionGaindB = 20;
    gain_config.limiterEnable = kAgcTrue;

    if (WebRtcAgc_Init(f_a->gain_control, 0, 255, kAgcModeAdaptiveDigital, fs) == -1 || WebRtcAgc_set_config(f_a->gain_control, gain_config) == -1) {
        kill_filter_audio(f_a);
        return NULL;
    }


    if (WebRtcNsx_Init(f_a->noise_sup_x, fs) == -1 || WebRtcNsx_set_policy(f_a->noise_sup_x, 2) == -1) {
        kill_filter_audio(f_a);
        return NULL;
    }

    echo_config.nlpMode = kAecNlpAggressive;
    echo_config.skewMode = kAecFalse;
    echo_config.metricsMode = kAecFalse;
    echo_config.delay_logging = kAecFalse;

    if (WebRtcAec_Init(f_a->echo_cancellation, fs, f_a->fs) == -1 || WebRtcAec_set_config(f_a->echo_cancellation, echo_config) == -1) {
        kill_filter_audio(f_a);
        return NULL;
    }

    if (WebRtcVad_Init(f_a->Vad_handle) == -1 || WebRtcVad_set_mode(f_a->Vad_handle,vad_mode) == -1){
        kill_filter_audio(f_a);
        return NULL;
    }

    f_a->echo_enabled = 1;
    f_a->gain_enabled = 1;
    f_a->noise_enabled = 1;
    f_a->vad_enabled = 1;

    
    //if (f_a->fs != 16000) {
    //    f_a->downsampler = speex_resampler_init(1, f_a->fs, 32000, quality, 0);
    //    f_a->upsampler = speex_resampler_init(1, 32000, f_a->fs, quality, 0);

    //     /* quality doesn't need to be high for this one. */
    //    quality = 0;
    //    f_a->downsampler_echo = speex_resampler_init(1, f_a->fs, 16000, quality, 0);

    //    if (!f_a->upsampler || !f_a->downsampler || !f_a->downsampler_echo) {
    //        kill_filter_audio(f_a);
    //        return NULL;
    //    }
    //}


    return f_a;
}

int enable_disable_filters(Filter_Audio *f_a, int echo, int noise, int gain, int vad)
{
    if (!f_a) {
        return -1;
    }

    f_a->echo_enabled = echo;
    f_a->gain_enabled = gain;
    f_a->noise_enabled = noise;
    f_a->vad_enabled = vad;
    return 0;
}

static void downsample_audio_echo_in(Filter_Audio *f_a, int16_t *out, const int16_t *in)
{
    uint32_t inlen = f_a->fs / 100;
    uint32_t outlen = inlen;
    speex_resampler_process_int(f_a->downsampler_echo, 0, in, &inlen, out, &outlen);
}


static void downsample_audio(Filter_Audio *f_a, int16_t *out_l, int16_t *out_h, const int16_t *in, uint32_t in_length)
{
    int16_t temp[320];
    uint32_t out_len = 320;
    if (f_a->fs != 32000) {
        speex_resampler_process_int(f_a->downsampler, 0, in, &in_length, temp, &out_len);
        WebRtcSpl_AnalysisQMF(temp, out_len, out_l, out_h,
                              f_a->split_filter_state_1, f_a->split_filter_state_2);
    } else {
        WebRtcSpl_AnalysisQMF(in, out_len, out_l, out_h,
                              f_a->split_filter_state_1, f_a->split_filter_state_2);
    }
}

static void upsample_audio(Filter_Audio *f_a, int16_t *out, uint32_t out_len, const int16_t *in_l, const int16_t *in_h, uint32_t in_length)
{
    int16_t temp[320];
    if (f_a->fs != 32000) {
        WebRtcSpl_SynthesisQMF(in_l, in_h, in_length, temp,
                               f_a->split_filter_state_3, f_a->split_filter_state_4);
        in_length *= 2;
        speex_resampler_process_int(f_a->upsampler, 0, temp, &in_length, out, &out_len);
    } else {
        WebRtcSpl_SynthesisQMF(in_l, in_h, in_length, out,
                               f_a->split_filter_state_3, f_a->split_filter_state_4);
    }
}


int pass_audio_output(Filter_Audio *f_a, const int16_t *data, unsigned int samples)
{
    unsigned int nsx_samples = f_a->fs / 100;
    unsigned int resampled_samples = 0;
    unsigned int temp_samples = samples;
    if (!f_a || (!f_a->echo_enabled && !f_a->gain_enabled)) {
        return -1;
    }

    if (!samples || (samples % nsx_samples) != 0) {
        return -1;
    }

    while (temp_samples) {
        
        float *d_f = (float*)malloc(sizeof(float)*nsx_samples);
        memset(d_f, 0, sizeof(float)*nsx_samples);
        S16ToFloatS16(data + (samples - temp_samples), nsx_samples, d_f);

        if (WebRtcAec_BufferFarend(f_a->echo_cancellation, d_f, nsx_samples) == -1) {
            return -1;
        }

        temp_samples -= nsx_samples;
        if (d_f)
        {
            free(d_f);
            d_f = NULL;
        }
    }

    return 0;
}

/* Tell the echo canceller how much time in ms it takes for audio to be played and recorded back after. */
int set_echo_delay_ms(Filter_Audio *f_a, int16_t msInSndCardBuf)
{
    if (!f_a) {
        return -1;
    }

    f_a->msInSndCardBuf = msInSndCardBuf;

    return 0;
}

int filter_audio(Filter_Audio *f_a, int16_t *data, unsigned int samples)
{
    //每次处理十毫秒的数据
    unsigned int nsx_samples = f_a->fs / 100;
    unsigned int resampled_samples = 0;
    unsigned int temp_samples = samples;

    unsigned int smp = f_a->fs / 100;
    int novoice = 1;

    if (!f_a) {
        return -1;
    }

    
    if (!samples || (samples % nsx_samples) != 0) {
        return -1;
    }

    /*if (f_a->fs != 16000) {
        samples = (samples / nsx_samples) * 160;
        nsx_samples = 160;
        resample = 1;
    }*/

    while (temp_samples) {
        //int16_t d_l[nsx_samples];
        int16_t *d_l = (int16_t*)malloc(sizeof(int16_t)*nsx_samples);
        int16_t *d_h = NULL;
        int16_t *temp = NULL;
        float *d_f_l = NULL;
        float *d_f_h = NULL;

        memset(d_l, 0, sizeof(int16_t)*nsx_samples);
        //int16_t temp[nsx_samples];
        temp = (int16_t*)malloc(sizeof(int16_t)*nsx_samples);
        memset(temp, 0, sizeof(int16_t)*nsx_samples);
        //memset(temp, 0, nsx_samples*sizeof(float));
        //memcpy(d_l, data + (samples - temp_samples), sizeof(d_l));
        memcpy(d_l, data + (samples - temp_samples), sizeof(int16_t)*nsx_samples);

        if(f_a->vad_enabled){
            if(WebRtcVad_Process(f_a->Vad_handle, f_a->fs, d_l, nsx_samples) == 1){
                novoice = 0;
            }
        } else {
            novoice = 0;
        }

        if (f_a->gain_enabled) {
            int tmp = 0;
            int32_t inMicLevel = 0, outMicLevel;

            if (WebRtcAgc_VirtualMic(f_a->gain_control, d_l, d_h, nsx_samples, inMicLevel, &outMicLevel) == -1)
                return -1;

            tmp = 0;
        }

        //float d_f_l[nsx_samples];
        d_f_l = (float*)malloc(sizeof(float)*nsx_samples);
        memset(d_f_l, 0, sizeof(float)*nsx_samples);

        S16ToFloatS16(d_l, nsx_samples, d_f_l);

        ////float d_f_h[nsx_samples];
        //d_f_h = (float*)malloc(sizeof(float)*nsx_samples);
        //memset(d_f_h, 0, nsx_samples*sizeof(float));

        /*f_a->echo_cancellation = 0;*/
        if (f_a->echo_enabled) {
            if (WebRtcAec_Process(f_a->echo_cancellation, d_f_l, NULL, d_f_l, NULL, nsx_samples, f_a->msInSndCardBuf, 0) == -1) {
                return -1;
            }

            FloatS16ToS16(d_f_l, nsx_samples, d_l);
        }

        if (f_a->noise_enabled) {
            if (WebRtcNsx_Process(f_a->noise_sup_x, d_l, d_h, d_l, d_h) == -1) {
                return -1;
            }
        }

        if (f_a->gain_enabled) {
            int32_t inMicLevel = 128, outMicLevel;
            uint8_t saturationWarning;
            int tmp = 0;

            if (WebRtcAgc_Process(f_a->gain_control, d_l, d_h, nsx_samples, d_l, d_h, inMicLevel, &outMicLevel, 1, &saturationWarning) == -1) {
                return -1;
            }

            tmp = 0;
        }

        /*S16ToFloat(d_l, nsx_samples, d_f_l);

        run_filter_zam(&f_a->hpfa, d_f_l, nsx_samples);
        run_filter_zam(&f_a->hpfb, d_f_l, nsx_samples);

        if (f_a->lowpass_enabled) {
            run_filter_zam(&f_a->lpfa, d_f_l, nsx_samples);
            run_filter_zam(&f_a->lpfb, d_f_l, nsx_samples);
        }

        run_saturator_zam(d_f_l, nsx_samples);*/

        //FloatToS16(d_f_l, nsx_samples, d_l);
        memcpy(data + (samples - temp_samples), d_l, sizeof(int16_t)*nsx_samples);

        temp_samples -= nsx_samples;

        if (d_l)
        {
            free(d_l);
            d_l = NULL;
        }

        if (d_h)
        {
            free(d_h);
            d_h = NULL;
            temp = NULL;
        }

        if (temp)
        {
            free(temp);
            temp = NULL;
        }

        if (d_f_l)
        {
            free(d_f_l);
            d_f_l = NULL;
        }

        if (d_f_h)
        {
            free(d_f_h);
            d_f_h = NULL;
        }
    }

    return !novoice;
}
