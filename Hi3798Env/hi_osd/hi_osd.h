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

// 菜单界面参数大小
#define MENU_Y_OFFSET 0              
#define MENU_X_OFFSET 0   

#define MENU_WIDTH 1000                    // 菜单界面宽
#define MENU_HEIGHT 290                   // 菜单界面高
#define MENU1_WIDTH 200                    // 1级菜单界面宽
#define MENU1_HEIGHT 240                 // 1级菜单界面高
#define MENU2_WIDTH 300                    // 2级菜单界面宽

#define MENU_STATUS_HEIGHT (50)      // 菜单栏高度
#define MENU_CHILD_VALU_HEIGHT 50 // 每级子菜单高度

#define MENU_CHILD_VALU_LINE_X0 (MENU_X_OFFSET+MENU1_WIDTH+10) // 每级子菜单割线的x起点
#define MENU_CHILD_VALU_LINE_X1 (990) // 每级子菜单割线的x终点

#define MENU_CHILD_VALU_LINE_Y1 (100+MENU_Y_OFFSET) //100 // 子菜单第1条割线的y坐标
#define MENU_CHILD_VALU_LINE_Y2 (MENU_CHILD_VALU_LINE_Y1+MENU_STATUS_HEIGHT)//150 // 子菜单第2条割线的y坐标
#define MENU_CHILD_VALU_LINE_Y3 (MENU_CHILD_VALU_LINE_Y2+MENU_STATUS_HEIGHT)//200 // 子菜单第3条割线的y坐标
#define MENU_CHILD_VALU_LINE_Y4 (MENU_CHILD_VALU_LINE_Y3+MENU_STATUS_HEIGHT)//250 // 子菜单第4条割线的y坐标

#define MENU_TEXT_X (10+MENU_X_OFFSET)                            // ""Menu" 文字显示坐标x
#define MENU_TEXT_Y (0+MENU_Y_OFFSET)//0                             // ""Menu" 文字显示坐标y

#define MENU1_TEXT_X 35                            // ""Menu1" 文字显示坐标x
#define MENU1_TEXT_Y (10+MENU_Y_OFFSET)//10                             // ""Menu1" 文字显示坐标y

#define MENU2_TEXT_X 35                            // ""Menu2" 文字显示坐标x
#define MENU2_TEXT_Y (10+MENU_Y_OFFSET)//10                               // ""Menu2" 文字显示坐标y

#define MENU3_TEXT_X 35                            // ""Menu3" 文字显示坐标x
#define MENU3_TEXT_X_OFFSET 20                            // ""Menu3" 文字显示坐标x
#define MENU3_TEXT_Y (10+MENU_Y_OFFSET)//10                               // ""Menu3" 文字显示坐标y


#define MENU3_VALU_SPACE 53                   // 数值间距
#define MENU3_VALU_WIDTH 100                // 数值间距

#define FONT_EN         40                      // 英文字体大小
#define FONT_CH         30                      // 中文字体大小

// 菜单参数
#define MENU_CNT 3                             // 一级主菜单总数
#define CHILD_CNT_MAX 10                // 最大子菜单数

#define MENU_DISPLAY 0                    // 一级主菜单：参数设置项
#define MENU_NET 1                            // 一级主菜单：网络设置项
#define MENU_COLORTEST 2               // 一级主菜单：显示设置

#define BRIGHTNESS_VALU_INDEX 0       // 参数设置项 子菜单： 亮度菜单索引
#define CONTRAST_VALU_INDEX 1           // 参数设置项 子菜单： 对比度菜单索引
#define IP_VALU_INDEX 0                         // 网络设置项 子菜单： IP设置项索引
#define CYCLEMODE_VALU_INDEX 0                      // 灰阶测试项 子菜单： 循环测试项索引
#define CYCLETIME_VALU_INDEX 1                      // 灰阶测试项 子菜单： 间隔时间项索引
#define CYCLEINDEX_VALU_INDEX 2                    // 灰阶测试项 子菜单： 单独循环项索引

#define BRIGHTNESS_VALU_CHILD_INDEX 0      // 亮度菜单值索引
#define CONTRAST_VALU_CHILD_INDEX 0          // 对比菜单值索引
#define IP_VALU_CHILD_INDEX 0                        // IP设置项索引

#define CYCLEMODE_VALU_CHILD_INDEX 0        // 灰阶测试项 子菜单： 循环测试项索引
#define CYCLETIME_VALU_CHILD_INDEX 0         // 灰阶测试项 子菜单： 间隔时间项索引
#define CYCLEINDEX_VALU_CHILD_INDEX 0       // 灰阶测试项 子菜单： 单独循环项索引

// 参数设置项
#define BRIGHTNESS_SET 1
#define CONTRAST_SET 2
#define IPADDR_SET 3
#define CYCLE_SET 4
#define CYCLETIME_SET 5
#define CYCLEINDEX_SET 6

// 按键值
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
