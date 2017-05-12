#include <string>
#include <iostream>
#include <cstdio>
#include "ortp/ortp.h"
#include "ortp/payloadtype.h"
#include "mediastreamer2/mediastream.h"

//extern void libmsx264_init();

void StartVideo(const std::string&  strLocalIP,int iLocalPort,const std::string& strRmoteIP,int iRemotePort)
{
    ortp_init();
    ortp_set_log_level_mask(ORTP_DEBUG | ORTP_MESSAGE | ORTP_WARNING | ORTP_ERROR | ORTP_FATAL);

    ms_init();
    //libmsx264_init();

    rtp_profile_set_payload(&av_profile,96,&payload_type_h264);
    RtpProfile *profile = rtp_profile_clone_full(&av_profile);
    OrtpEvQueue *q = ortp_ev_queue_new();

    ms_filter_enable_statistics(TRUE);
    ms_filter_reset_statistics();

    //signal(SIGINT, stop_handler);
    PayloadType *pt = rtp_profile_get_payload(profile, 96);
    if (pt == NULL) {
        ms_error("No payload defined with number %i.\n", 96);
        exit(-1);
    }

    payload_type_unset_flag(pt, PAYLOAD_TYPE_RTCP_FEEDBACK_ENABLED);

    MSWebCamManager *manager = ms_web_cam_manager_get();

    VideoStream *video = video_stream_new(iLocalPort,iLocalPort + 1,ms_is_ipv6(strLocalIP.c_str()));
    video->dir = MediaStreamSendOnly;
    ms_message("Starting video stream.\n");

    MSMediaStreamIO iodef;
    iodef.input.type = MSResourceCamera;
    iodef.input.camera = ms_web_cam_manager_get_default_cam(manager);
    iodef.input.session = video->rtp_io_session;
    iodef.input.resource_arg = ms_web_cam_manager_get_default_cam(manager);
    iodef.output.type = MSResourceRtp;
    video_stream_start_from_io(video,profile,strRmoteIP.c_str(),iRemotePort,strRmoteIP.c_str(),iRemotePort,96,&iodef);
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
    StartVideo(argv[1],1561,argv[2],1561);
    while (true)
    {
        Sleep(1000);
    }
}
