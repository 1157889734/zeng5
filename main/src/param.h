#ifndef __PARAM_H__
#define __PARAM_H__

#define LCD_ONLINE 0x01
#define LCD_SLEEP 0x03

#define VERSION_MAX 1
#define VERSION_MIN 6

int read_ip(char *ip);
int read_mac(char *mac);
int read_light();
int read_pwm();
int read_contrast();
int read_dev_id();
int read_run_state();
int read_work_mode();
int read_version(char *version);

int set_drmd_version(int max, int min);
int set_osd_version(int max, int min);

void set_osd_485st(unsigned char valu);
unsigned char get_osd_485st(void);

void set_osd_CycleMode(unsigned char valu);
unsigned char get_osd_CycleMode(void);

void set_osd_CycleTime(unsigned char valu);
unsigned char get_osd_CycleTime(void);

void set_osd_CycleIndex(unsigned char valu);
unsigned char get_osd_CycleIndex(void);

int read_off_line_time(void);
int read_out_line_time(void);

void write_off_line_time(int  valu);
void write_out_line_time(int  valu);

unsigned long getlocalip (void);
int exec_cmd_popen(char *cmd);

#endif
