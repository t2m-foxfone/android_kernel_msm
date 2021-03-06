/* drivers/input/touchscreen/gt9xx.h
 * 
 * 2010 - 2013 Goodix Technology.
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be a reference 
 * to you, when you are integrating the GOODiX's CTP IC into your system, 
 * but WITHOUT ANY WARRANTY; without even the implied warranty of 
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
 * General Public License for more details.
 * 
 */

#ifndef _GOODIX_GT9XX_H_
#define _GOODIX_GT9XX_H_

#include <linux/kernel.h>
#include <linux/hrtimer.h>
#include <linux/i2c.h>
#include <linux/irq.h>
#include <linux/input.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/delay.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/proc_fs.h>
#include <linux/regulator/consumer.h>
#include <linux/firmware.h>
#include <linux/debugfs.h>
#include <asm/uaccess.h>

#if defined(CONFIG_FB)
#include <linux/notifier.h>
#include <linux/fb.h>
#elif defined(CONFIG_HAS_EARLYSUSPEND)
#include <linux/earlysuspend.h>
#define GOODIX_SUSPEND_LEVEL 1
#endif
#define  GT_VIRTUAL_KEY 0
struct goodix_ts_platform_data {
	int irq_gpio;
	u32 irq_gpio_flags;
	int reset_gpio;
	u32 reset_gpio_flags;
	u32 family_id;
	u32 x_max;
	u32 y_max;
	u32 x_min;
	u32 y_min;
	u32 panel_minx;
	u32 panel_miny;
	u32 panel_maxx;
	u32 panel_maxy;
	bool no_force_update;
	bool i2c_pull_up;
	int gtp_cfg_len;
	u8 *config_data;
#if GT_VIRTUAL_KEY
	//const char *name;
	int disp_maxx;
	int disp_maxy;
	int pan_maxx;
	int pan_maxy;
	int *keycodes;
	int num_keys;
	int y_offset;
#endif
};
struct goodix_ts_data {
	spinlock_t irq_lock;
	struct i2c_client *client;
	struct input_dev  *input_dev;
	struct goodix_ts_platform_data *pdata;
	struct hrtimer timer;
	struct workqueue_struct *goodix_wq;
	struct work_struct	work;
	s32 irq_is_disabled;
	s32 use_irq;
	u16 abs_x_max;
	u16 abs_y_max;
	u8  max_touch_num;
	u8  int_trigger_type;
	u8  green_wake_mode;
	u8  chip_type;
	u8 *config_data;
	u8  enter_update;
	u8  gtp_is_suspend;
	u8  gtp_rawdiff_mode;
	u8  gtp_cfg_len;
	u8  fixed_cfg;
	u8  esd_running;
	u8  fw_error;
	struct regulator *avdd;
	struct regulator *vdd;
	struct regulator *vcc_i2c;
#if defined(CONFIG_FB)
	struct notifier_block fb_notif;
#elif defined(CONFIG_HAS_EARLYSUSPEND)
	struct early_suspend early_suspend;
#endif
};

extern u16 show_len;
extern u16 total_len;
//***************************PART1:ON/OFF define*******************************
#define GTP_CUSTOM_CFG			1
#define GTP_CHANGE_X2Y        0
#define GTP_DRIVER_SEND_CFG   1
#define GTP_HAVE_TOUCH_KEY    1
#define GTP_POWER_CTRL_SLEEP  0
#define GTP_ICS_SLOT_REPORT   1 
#define VER_READ 				1//add the attribute for reading the version of firmware 2013-12-03-Richard  

/* auto updated by .bin file as default */
#define GTP_AUTO_UPDATE			0
/* auto updated by head_fw_array in gt9xx_firmware.h,
 * function together with GTP_AUTO_UPDATE */
#define GTP_HEADER_FW_UPDATE	0
                               
#define GTP_CREATE_WR_NODE		1
#define GTP_ESD_PROTECT			1// open the flag for ESD testing ---Richard---2013/11/26
#define GTP_WITH_PEN			0

#define GTP_SLIDE_WAKEUP     0
#define GTP_DBL_CLK_WAKEUP    0     // double-click wakeup, function together with GTP_SLIDE_WAKEUP

#define GTP_DEBUG_ON          0 
#define GTP_DEBUG_ARRAY_ON    0 
#define GTP_DEBUG_FUNC_ON     0 

//*************************** PART2:TODO define **********************************
// STEP_1(REQUIRED): Define Configuration Information Group(s)
// Sensor_ID Map:
/* sensor_opt1 sensor_opt2 Sensor_ID
    GND         GND         0 
    VDDIO       GND         1 
    NC          GND         2 
    GND         NC/300K     3 
    VDDIO       NC/300K     4 
    NC          NC/300K     5 
*/
// TODO: define your own default or for Sensor_ID == 0 config here. 
// The predefined one is just a sample config, which is not suitable for your tp in most cases.

#define CTP_CFG_GROUP1 {\
  0x43,0xE0,0x01,0x56,0x03,0x05,0x34,0x11,0x02,0xC8,\
  0x1E,0x0A,0x50,0x3C,0x03,0x05,0x00,0x00,0x00,0x00,\
  0x12,0x11,0x00,0x17,0x19,0x1D,0x14,0x89,0x08,0x0B,\
  0x3C,0x00,0xD3,0x07,0x00,0x00,0x00,0x22,0x03,0x1D,\
  0x3C,0x1D,0x0E,0x00,0x00,0x1E,0x14,0x10,0x40,0x00,\
  0x00,0x1E,0x5A,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,\
  0xD7,0x21,0x00,0xB1,0x29,0x00,0x90,0x34,0x00,0x77,\
  0x41,0x00,0x65,0x51,0x00,0x65,0x30,0x00,0x00,0x00,\
  0xF0,0x4A,0x3A,0xA0,0x00,0x27,0x00,0x00,0x00,0x00,\
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
  0x00,0x00,0x0A,0x0C,0x08,0x06,0x04,0x02,0x0E,0x10,\
  0x12,0x14,0x16,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,\
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
  0x00,0x00,0x21,0x20,0x1F,0x1E,0x1D,0x1C,0x18,0x16,\
  0x10,0x0F,0x00,0x02,0x04,0x08,0x0A,0x06,0x12,0xFF,\
  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,\
  0x00,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0x2F,0xF3,0xFF,\
  0xFF,0xFF,0xFF,0xFF,0x9D,0x01\
}
//updata 20140430

/*
#define CTP_CFG_GROUP1 {\
0x43,0xE0,0x01,0x56,0x03,0x05,0x34,0x11,0x02,0xC8,\
0x1E,0x0A,0x91,0x73,0x03,0x05,0x00,0x00,0x00,0x00,\
0x12,0x11,0x00,0x17,0x19,0x1D,0x14,0x89,0x08,0x0B,\
0x3C,0x00,0xD3,0x07,0x00,0x00,0x00,0x22,0x03,0x1D,\
0x3C,0x1D,0x0E,0x00,0x00,0x1E,0x14,0x10,0x40,0x00,\
0x00,0x1E,0x5A,0x94,0xC5,0x02,0x07,0x00,0x00,0x04,\
0xD7,0x21,0x00,0xB1,0x29,0x00,0x90,0x34,0x00,0x77,\
0x41,0x00,0x65,0x51,0x00,0x65,0x30,0x00,0x00,0x00,\
0xF0,0x50,0x40,0x80,0x00,0x27,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x0A,0x0C,0x08,0x06,0x04,0x02,0x0E,0x10,\
0x12,0x14,0x16,0xFF,0xFF,0xFF,0x00,0x00,0x00,0x00,\
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,\
0x00,0x00,0x21,0x20,0x1F,0x1E,0x1D,0x1C,0x18,0x16,\
0x10,0x0F,0x00,0x02,0x04,0x08,0x0A,0x06,0x12,0xFF,\
0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0x00,0x00,\
0x00,0xFF,0x3F,0xFF,0xFF,0xFF,0xFF,0x2F,0xF3,0xFF,\
0xFF,0xFF,0xFF,0xFF,0x39,0x01\
}
// init_data is only used for esd certification ,so the sensitivity is very low.
*/

    
// TODO: define your config for Sensor_ID == 1 here, if needed
#define CTP_CFG_GROUP2 {\
    }
// TODO: define your config for Sensor_ID == 2 here, if needed
#define CTP_CFG_GROUP3 {\
    }

// TODO: define your config for Sensor_ID == 3 here, if needed
#define CTP_CFG_GROUP4 {\
    }

// TODO: define your config for Sensor_ID == 4 here, if needed
#define CTP_CFG_GROUP5 {\
    }

// TODO: define your config for Sensor_ID == 5 here, if needed
#define CTP_CFG_GROUP6 {\
    }

// STEP_2(REQUIRED): Customize your I/O ports & I/O operations
#define GTP_RST_PORT   0 
#define GTP_INT_PORT   1 
#define GTP_INT_IRQ    gpio_to_irq(GTP_INT_PORT) 
//#define GTP_INT_CFG     S3C_GPIO_SFN(0xF)

#define GTP_GPIO_AS_INPUT(pin)     gpio_direction_input(pin)    
#define GTP_GPIO_AS_INT(pin)        gpio_direction_input(pin)   
#define GTP_GPIO_GET_VALUE(pin)         gpio_get_value(pin)
#define GTP_GPIO_OUTPUT(pin,level)      gpio_direction_output(pin,level)
#define GTP_GPIO_REQUEST(pin, label)    gpio_request(pin, label)
#define GTP_GPIO_FREE(pin)              gpio_free(pin)
#define GTP_IRQ_TAB                     {IRQ_TYPE_EDGE_RISING, IRQ_TYPE_EDGE_FALLING, IRQ_TYPE_LEVEL_LOW, IRQ_TYPE_LEVEL_HIGH}

/* STEP_3(optional): Specify your special config info if needed */
#define GTP_IRQ_TAB_RISING	0
#define GTP_IRQ_TAB_FALLING	1
#if GTP_CUSTOM_CFG
  #define GTP_MAX_HEIGHT   854
  #define GTP_MAX_WIDTH    480
  #define GTP_INT_TRIGGER  0    //0:Rising 1:Falling
#else
  #define GTP_MAX_HEIGHT   4096
  #define GTP_MAX_WIDTH    4096
#define GTP_INT_TRIGGER		GTP_IRQ_TAB_FALLING
#endif
#define GTP_MAX_TOUCH         5
#define GTP_ESD_CHECK_CIRCLE  1000      /* jiffy: ms *///richard

// STEP_4(optional): If keys are available and reported as keys, config your key info here                             
#if GTP_HAVE_TOUCH_KEY
    #define GTP_KEY_TAB  {KEY_HOME, KEY_HOME, KEY_HOME}
#endif

//***************************PART3:OTHER define*********************************
#define GTP_DRIVER_VERSION    "V1.8<2013/06/08>"
#define GTP_I2C_NAME          "Goodix-TS"
#define GTP_POLL_TIME         10     // jiffy: ms
#define GTP_ADDR_LENGTH       2
#define GTP_CONFIG_MIN_LENGTH 186
#define GTP_CONFIG_MAX_LENGTH 240
#define FAIL                  0
#define SUCCESS               1
#define SWITCH_OFF            0
#define SWITCH_ON             1

// Registers define
#define GTP_READ_COOR_ADDR    0x814E
#define GTP_REG_SLEEP         0x8040
#define GTP_REG_SENSOR_ID     0x814A
#define GTP_REG_CONFIG_DATA   0x8047
#define GTP_REG_VERSION       0x8140

#define RESOLUTION_LOC        3
#define TRIGGER_LOC           8

#define CFG_GROUP_LEN(p_cfg_grp)  (sizeof(p_cfg_grp) / sizeof(p_cfg_grp[0]))
// Log define

//#define GT915_DEBUG
#ifdef GT915_DEBUG
#define GTP_INFO(fmt,arg...)           printk("<<-GTP-INFO->> "fmt"\n",##arg)
#define GTP_ERROR(fmt,arg...)          printk("<<-GTP-ERROR->> "fmt"\n",##arg)
#define GTP_DEBUG(fmt,arg...)          do{\
                                         if(GTP_DEBUG_ON)\
                                         printk("<<-GTP-DEBUG->> [%d]"fmt"\n",__LINE__, ##arg);\
                                       }while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{\
                                         s32 i;\
                                         u8* a = array;\
                                         if(GTP_DEBUG_ARRAY_ON)\
                                         {\
                                            printk("<<-GTP-DEBUG-ARRAY->>\n");\
                                            for (i = 0; i < (num); i++)\
                                            {\
                                                printk("%02x   ", (a)[i]);\
                                                if ((i + 1 ) %10 == 0)\
                                                {\
                                                    printk("\n");\
                                                }\
                                            }\
                                            printk("\n");\
                                        }\
                                       }while(0)
#define GTP_DEBUG_FUNC()               do{\
                                         if(GTP_DEBUG_FUNC_ON)\
                                         printk("<<-GTP-FUNC->> Func:%s@Line:%d\n",__func__,__LINE__);\
                                       }while(0)
#else
#define GTP_INFO(fmt,arg...)           do{}while(0)
#define GTP_ERROR(fmt,arg...)         do{}while(0)
#define GTP_DEBUG(fmt,arg...)        do{}while(0)
#define GTP_DEBUG_ARRAY(array, num)    do{}while(0)
#define GTP_DEBUG_FUNC()  do{}while(0)
#endif
#define GTP_SWAP(x, y)                 do{\
                                         typeof(x) z = x;\
                                         x = y;\
                                         y = z;\
                                       }while (0)

//*****************************End of Part III********************************

void gtp_esd_switch(struct i2c_client *client, int on);

#if GTP_CREATE_WR_NODE
extern s32 init_wr_node(struct i2c_client *client);
extern void uninit_wr_node(void);
#endif

#if GTP_AUTO_UPDATE
extern u8 gup_init_update_proc(struct goodix_ts_data *ts);
#endif
#endif /* _GOODIX_GT9XX_H_ */
