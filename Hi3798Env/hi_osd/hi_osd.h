#ifndef __SAMPLE_OSD_H
#define __SAMPLE_OSD_H

#if 0
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <pthread.h>

#include <assert.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

#include "hi_unf_ir.h"
//#include "hi_adp.h"

#include "hi_go.h"
//#include "Hi_go_decoder.h"
//#include "sample_displayInit.h"
#include "hi_adp_mpi.h"

#include "ini_file.h"
#endif

#define OSD_FONT_NAME "/root/fonts/osdfonts/simhei.ttf" 
#define OSD_INI "/root/config/osd.ini"
#define NET_INI "/etc/net.conf"

// �˵����������С
#define MENU_Y_OFFSET 0              
#define MENU_X_OFFSET 0   

#define MENU_WIDTH 1000                    // �˵������
#define MENU_HEIGHT 290                   // �˵������
#define MENU1_WIDTH 200                    // 1���˵������
#define MENU1_HEIGHT 240                 // 1���˵������
#define MENU2_WIDTH 300                    // 2���˵������

#define MENU_STATUS_HEIGHT (50)      // �˵����߶�
#define MENU_CHILD_VALU_HEIGHT 50 // ÿ���Ӳ˵��߶�

#define MENU_CHILD_VALU_LINE_X0 (MENU_X_OFFSET+MENU1_WIDTH+10) // ÿ���Ӳ˵����ߵ�x���
#define MENU_CHILD_VALU_LINE_X1 (990) // ÿ���Ӳ˵����ߵ�x�յ�

#define MENU_CHILD_VALU_LINE_Y1 (100+MENU_Y_OFFSET) //100 // �Ӳ˵���1�����ߵ�y����
#define MENU_CHILD_VALU_LINE_Y2 (MENU_CHILD_VALU_LINE_Y1+MENU_STATUS_HEIGHT)//150 // �Ӳ˵���2�����ߵ�y����
#define MENU_CHILD_VALU_LINE_Y3 (MENU_CHILD_VALU_LINE_Y2+MENU_STATUS_HEIGHT)//200 // �Ӳ˵���3�����ߵ�y����
#define MENU_CHILD_VALU_LINE_Y4 (MENU_CHILD_VALU_LINE_Y3+MENU_STATUS_HEIGHT)//250 // �Ӳ˵���4�����ߵ�y����

#define MENU_TEXT_X (10+MENU_X_OFFSET)                            // ""Menu" ������ʾ����x
#define MENU_TEXT_Y (0+MENU_Y_OFFSET)//0                             // ""Menu" ������ʾ����y

#define MENU1_TEXT_X 35                            // ""Menu1" ������ʾ����x
#define MENU1_TEXT_Y (10+MENU_Y_OFFSET)//10                             // ""Menu1" ������ʾ����y

#define MENU2_TEXT_X 35                            // ""Menu2" ������ʾ����x
#define MENU2_TEXT_Y (10+MENU_Y_OFFSET)//10                               // ""Menu2" ������ʾ����y

#define MENU3_TEXT_X 35                            // ""Menu3" ������ʾ����x
#define MENU3_TEXT_X_OFFSET 20                            // ""Menu3" ������ʾ����x
#define MENU3_TEXT_Y (10+MENU_Y_OFFSET)//10                               // ""Menu3" ������ʾ����y


#define MENU3_VALU_SPACE 53                   // ��ֵ���
#define MENU3_VALU_WIDTH 100                // ��ֵ���

#define FONT_EN         40                      // Ӣ�������С
#define FONT_CH         30                      // ���������С

// �˵�����
#define MENU_CNT 3                             // һ�����˵�����
#define CHILD_CNT_MAX 10                // ����Ӳ˵���

#define MENU_DISPLAY 0                    // һ�����˵�������������
#define MENU_NET 1                            // һ�����˵�������������
#define MENU_COLORTEST 2               // һ�����˵�����ʾ����

#define BRIGHTNESS_VALU_INDEX 0       // ���������� �Ӳ˵��� ���Ȳ˵�����
#define CONTRAST_VALU_INDEX 1           // ���������� �Ӳ˵��� �ԱȶȲ˵�����
#define IP_VALU_INDEX 0                         // ���������� �Ӳ˵��� IP����������
#define CYCLEMODE_VALU_INDEX 0                      // �ҽײ����� �Ӳ˵��� ѭ������������
#define CYCLETIME_VALU_INDEX 1                      // �ҽײ����� �Ӳ˵��� ���ʱ��������
#define CYCLEINDEX_VALU_INDEX 2                    // �ҽײ����� �Ӳ˵��� ����ѭ��������

#define BRIGHTNESS_VALU_CHILD_INDEX 0      // ���Ȳ˵�ֵ����
#define CONTRAST_VALU_CHILD_INDEX 0          // �ԱȲ˵�ֵ����
#define IP_VALU_CHILD_INDEX 0                        // IP����������

#define CYCLEMODE_VALU_CHILD_INDEX 0        // �ҽײ����� �Ӳ˵��� ѭ������������
#define CYCLETIME_VALU_CHILD_INDEX 0         // �ҽײ����� �Ӳ˵��� ���ʱ��������
#define CYCLEINDEX_VALU_CHILD_INDEX 0       // �ҽײ����� �Ӳ˵��� ����ѭ��������

// ����������
#define BRIGHTNESS_SET 1
#define CONTRAST_SET 2
#define IPADDR_SET 3
#define CYCLE_SET 4
#define CYCLETIME_SET 5
#define CYCLEINDEX_SET 6

// ����ֵ
#define KEY_0  0
#define KEY_1  1
#define KEY_2  2
#define KEY_3  3
#define KEY_4  4
#define KEY_5  5
#define KEY_6  6
#define KEY_7  7
#define KEY_8  8
#define KEY_9  9
#define KEY_POWER  10  //power 
#define KEY_MENU  11  //menu
#define KEY_EXIT  12  //exit
#define KEY_OK 13  //ok
#define KEY_UP  14  //up   
#define KEY_DOWN  15  //down
#define KEY_LEFT  16  //left
#define KEY_RIGHT  17  //right

#define KEY_ENTER_CNT_MAX 15

#define ON 1
#define OFF 0

int osdTest(void);
void back_light_power(int onoff);
//void Hi_SetPwm(unsigned char valu);
void back_light_set_valu(unsigned char valu);

void set_cycle_chg(unsigned char valu);
unsigned char get_cycle_chg(void);

unsigned char get_cycle_mode(void);
unsigned char get_cycle_time(void);
unsigned char get_cycle_index(void);

#endif
