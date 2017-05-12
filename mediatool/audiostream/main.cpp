#include <string>
#include <iostream>
#include <cstdio>
#include "ortp/ortp.h"
#include "ortp/payloadtype.h"
#include "mediastreamer2/mediastream.h"


static MSSndCard *get_sound_card(MSSndCardManager *manager, const char* card_name) {
    MSSndCard *play = ms_snd_card_manager_get_card(manager, card_name);
    if (play == NULL) {
        const MSList *list = ms_snd_card_manager_get_list(manager);
        char * cards = ms_strdup("");
        while (list) {
            MSSndCard *card = (MSSndCard*)list->data;
            cards = ms_strcat_printf(cards, "- %s\n", ms_snd_card_get_string_id(card));
            list = list->next;
        }
        ms_fatal("Specified card '%s' but could not find it. Available cards are:\n%s", card_name, cards);
        ms_free(cards);
    }
    return play;
}

void StartAudio(const std::string&  strLocalIP,int iLocalPort,const std::string& strRmoteIP,int iRemotePort)
{
    ortp_init();
    ortp_set_log_level_mask(ORTP_DEBUG | ORTP_MESSAGE | ORTP_WARNING | ORTP_ERROR | ORTP_FATAL);

    ms_init();

    RtpProfile *profile = rtp_profile_clone_full(&av_profile);
    OrtpEvQueue *q = ortp_ev_queue_new();

    ms_filter_enable_statistics(TRUE);
    ms_filter_reset_statistics();

    //signal(SIGINT, stop_handler);
    PayloadType *pt = rtp_profile_get_payload(profile, 8);
    if (pt == NULL) {
        ms_error("No payload defined with number %i.\n", 8);
        exit(-1);
    }

    payload_type_unset_flag(pt, PAYLOAD_TYPE_RTCP_FEEDBACK_ENABLED);

    MSSndCardManager *manager = ms_snd_card_manager_get();
    const MSList* pCardList = ms_snd_card_manager_get_list(manager);
   
    MSSndCard *pPreCard = (MSSndCard*)pCardList->data;
    ms_message("======>card id:%s,card name:%s",pPreCard->id,pPreCard->name);

    MSSndCard *pCard = (MSSndCard*)pCardList->next->data;
    ms_message("======>card id:%s,card name:%s",pCard->id,pCard->name);

    AudioStream *audio = audio_stream_new(iLocalPort, iLocalPort + 1, ms_is_ipv6(strLocalIP.c_str()));
    audio_stream_enable_automatic_gain_control(audio, FALSE);
    audio_stream_enable_noise_gate(audio, FALSE);
    audio_stream_enable_echo_limiter(audio, ELInactive);
    audio_stream_enable_adaptive_bitrate_control(audio, FALSE);

    ms_message("Starting audio stream.\n");

    audio_stream_start_full(audio, profile, strRmoteIP.c_str(), iRemotePort, strRmoteIP.c_str(), iRemotePort + 1, 8, 50, NULL, NULL, pCard, pCard, FALSE);
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        std::cout<<"invalid args"<<std::endl;
        return -1;
    }

    std::cout << "======>local ip is:" << argv[1] << std::endl;
    std::cout << "======>remote ip is:"<< argv[2]  << std::endl;
    StartAudio(argv[1] , 1551, argv[2], 1551);
    while (true)
    {
        Sleep(1000);
    }
}
