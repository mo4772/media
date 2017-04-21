#include <string>
#include <iostream>
#include <cstdio>
#include "ortp/ortp.h"
#include "ortp/payloadtype.h"
#include "mediastreamer2/mediastream.h"

//UTF-8到GB2312的转换
char* U2G(const char* utf8)
{
    int len = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, wstr, len);
    len = WideCharToMultiByte(CP_ACP, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_ACP, 0, wstr, -1, str, len, NULL, NULL);
    if(wstr) delete[] wstr;
    return str;
}

//GB2312到UTF-8的转换
char* G2U(const char* gb2312)
{
    int len = MultiByteToWideChar(CP_ACP, 0, gb2312, -1, NULL, 0);
    wchar_t* wstr = new wchar_t[len+1];
    memset(wstr, 0, len+1);
    MultiByteToWideChar(CP_ACP, 0, gb2312, -1, wstr, len);
    len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
    char* str = new char[len+1];
    memset(str, 0, len+1);
    WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
    if(wstr) delete[] wstr;
    return str;
}

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

int main()
{
    ms_init();
    MSSndCardManager *manager = ms_snd_card_manager_get();
    const MSList* pCardList = ms_snd_card_manager_get_list(manager);
    while (pCardList != NULL)
    {
        MSSndCard *pCard = (MSSndCard*)pCardList->data;
        std::cout<<"snd card id=====>"<<U2G(pCard->id)<<",name=====>"<<U2G(pCard->name)<<std::endl;
        pCardList = pCardList->next;
    }


}
