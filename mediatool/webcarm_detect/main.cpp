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

int main()
{
    ms_init();
    MSWebCamManager *manager = ms_web_cam_manager_get();
    const MSList* pWebCamList = ms_web_cam_manager_get_list(manager);
    while (pWebCamList != NULL)
    {
        MSWebCam *pWebCam = (MSWebCam*)pWebCamList->data;
        std::cout<<"web cam id=====>"<<U2G(pWebCam->id)<<"   name=====>"<<U2G(pWebCam->name)<<std::endl;
        pWebCamList = pWebCamList->next;
    }
}
