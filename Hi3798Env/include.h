#ifndef _INCLUDE_H__
#define _INCLUDE_H__

//sys 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <time.h> 
#include <termios.h>

#include <dirent.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <assert.h>
#include <fcntl.h>
#include <regex.h>
#include <linux/fb.h>
#include <net/if.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <sys/times.h>

//hi3798dmo include
#include "hi3798mv2dmo/include/hi_unf_common.h"
#include "hi3798mv2dmo/include/hi_unf_avplay.h"
#include "hi3798mv2dmo/include/hi_unf_sound.h"
#include "hi3798mv2dmo/include/hi_unf_disp.h"
#include "hi3798mv2dmo/include/hi_unf_vo.h"
#include "hi3798mv2dmo/include/hi_unf_demux.h"
#include "hi3798mv2dmo/include/hi_unf_hdmi.h"
#include "hi3798mv2dmo/include/hi_unf_edid.h"
#include "hi3798mv2dmo/include/hi_unf_frontend.h"
#include "hi3798mv2dmo/include/hi_unf_pvr.h"
#include "hi3798mv2dmo/include/hi_type.h"
#include "hi3798mv2dmo/include/hi_unf_ai.h"
#include "hi3798mv2dmo/include/hi_common.h"
#include "hi3798mv2dmo/include/hi_audio_codec.h"
#include "hi3798mv2dmo/include/hi_error_mpi.h"
#include "hi3798mv2dmo/include/hi_unf_mce.h"
#include "hi3798mv2dmo/include/hi_unf_pwm.h"
#include "hi3798mv2dmo/include/hi_unf_pdm.h"
#include "hi3798mv2dmo/include/hi_debug.h"
#include "hi3798mv2dmo/include/jpeglib.h"
#include "hi3798mv2dmo/include/hi_unf_so.h"
#include "hi3798mv2dmo/include/hi_go.h"
#include "hi3798mv2dmo/include/hi_unf_ir.h"
#include "hi3798mv2dmo/include/hifb.h"
#include "hi3798mv2dmo/include/hi_svr_player.h"
#include "hi3798mv2dmo/include/hi_svr_format.h"
#include "hi3798mv2dmo/include/hi_svr_metadata.h"
#include "hi3798mv2dmo/include/hi_svr_charset.h"
#include "hi3798mv2dmo/include/hi_svr_charset_norm.h"
#include "hi3798mv2dmo/include/hi_svr_codec.h"
#include "hi3798mv2dmo/include/hi_svr_player_errno.h"
#include "hi3798mv2dmo/include/hi_svr_assa.h"
#include "hi3798mv2dmo/include/hi_unf_wdg.h"

//msp_api
#include "msp_api/hi_jpeg_api.h"
//sample common 
#include "common/hi_adp_search.h"
#include "common/hi_adp_hdmi.h"
#include "common/hi_adp_data.h"
#include "common/hi_adp_mpi.h"
#include "common/hi_adp_demux.h"
#include "common/hi_filter.h"
#include "common/hi_adp_tuner.h"
#include "common/hi_adp_pvr.h"
#include "common/hi_adp.h"
#include "common/hi_adp_boardcfg.h"

#include "hi_ipplay/hi_ipplay.h"
#include "hi_fb/hi_fb.h"
#include "hi_player/hi_player.h"
#include "hi_image/hi_image.h"
#include "hi_gpio/hi_gpio.h"
#include "hi_osd/hi_osd.h"
#include "hi_pwm/hi_pwm.h"



#include "socket_unix/socket_unix.h"
#include "ini_file/ini_file.h"
#include "uart485/uart485.h"

#endif
