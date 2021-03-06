/* Copyright (c) 2010-2012, Code Aurora Forum. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifdef CONFIG_SPI_QUP
#include <linux/spi/spi.h>
#endif
#include <linux/leds.h>
#include "msm_fb.h"
#include "mipi_dsi.h"
#include "mipi_novatek.h"
#include "mdp4.h"
#include <mach/gpio.h>

#define COLOR_ENHANCE
static struct mipi_dsi_panel_platform_data *mipi_novatek_pdata;

static struct dsi_buf novatek_tx_buf;
static struct dsi_buf novatek_rx_buf;
extern void mipi_set_tx_power_mode(int mode);

static int mipi_novatek_lcd_init(void);
static void lcd_panle_reset(void);

static char exit_sleep[2] = {0x11, 0x00}; /* DTYPE_DCS_WRITE */
static char display_on[2] = {0x29, 0x00}; /* DTYPE_DCS_WRITE */

#ifdef COLOR_ENHANCE

static char SSD2825_write_para01[] = {0x00,0xa0};
static char SSD2825_write_para02[] = {0xd6,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD};
static char SSD2825_write_para03[] = {0x00,0xB0};
static char SSD2825_write_para04[] = {0xd6,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD};
static char SSD2825_write_para05[] = {0x00,0xC0};
static char SSD2825_write_para06[] = {0xD6,0x89,0x11,0x89,0x89,0x11,0x89,0x89,0x11,0x89,0x89,0x11,0x89};
static char SSD2825_write_para07[] = {0x00,0xD0};
static char SSD2825_write_para08[] = {0xD6,0x89,0x11,0x89,0x89,0x11,0x89};
static char SSD2825_write_para09[] = {0x00,0xE0};
static char SSD2825_write_para10[] = {0xD6,0x44,0x11,0x44,0x44,0x11,0x44,0x44,0x11,0x44,0x44,0x11,0x44};
static char SSD2825_write_para11[] = {0x00,0xF0};
static char SSD2825_write_para12[] = {0xD6,0x44,0x11,0x44,0x44,0x11,0x44};
static char SSD2825_write_para13[] = {0x00,0x90};			  //Clever CMD1
static char SSD2825_write_para14[] = {0xd6,0x80};
static char SSD2825_write_para15[] = {0x00,0x00};			  //CE - High
static char SSD2825_write_para16[] = {0x55,0xB0};
static char SSD2825_write_para17[] = {0x00,0x81};			  //SHARPNESS Level - 9
static char SSD2825_write_para18[] = {0xD6,0x09,0xff};

#endif

#if 0// old lead auo otm1283 panel
static char OTM8018_para001[] = {0x00,0x00};                                                                              
static char OTM8018_para002[] = {0xff,0x12,0x83,0x01};	                                                                  
static char OTM8018_para003[] = {0x00,0x80};	                                                                            
static char OTM8018_para004[] = {0xff,0x12,0x83};                                                                         
static char OTM8018_para005[] = {0x00,0xa6};                                                                              
static char OTM8018_para006[] = {0xb3,0x0b};                                                                              
static char OTM8018_para007[] = {0x00,0x80};                                                                              
static char OTM8018_para008[] = {0xc0,0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10};                                      
static char OTM8018_para009[] = {0x00,0x90};             //Panel Timing Setting                                           
static char OTM8018_para010[] = {0xc0,0x00,0x5c,0x00,0x01,0x00,0x04};                                                     
static char OTM8018_para011[] = {0x00,0xa4};             //source pre.                                                    
static char OTM8018_para012[] = {0xc0,0x22};                                                                              
static char OTM8018_para013[] = {0x00,0xb3};             //Interval Scan Frame: 0 frame, column inversion                 
static char OTM8018_para014[] = {0xc0,0x00,0x50};                                                                         
static char OTM8018_para015[] = {0x00,0x81};             //frame rate:60Hz                                                
static char OTM8018_para016[] = {0xc1,0x55};                                                                              
static char OTM8018_para017[] = {0x00,0x90};             //clock delay for data latch                                     
static char OTM8018_para018[] = {0xc4,0x49};                                                                              
static char OTM8018_para019[] = {0x00,0xa0};             //dcdc setting                                                   
static char OTM8018_para020[] = {0xc4,0x05,0x10,0x04,0x02,0x05,0x15,0x11,0x05,0x10,0x05,0x02,0x05,0x15,0x11};             
static char OTM8018_para021[] = {0x00,0xb0};             //clamp voltage setting                                          
static char OTM8018_para022[] = {0xc4,0x00,0x00};                                                                         
static char OTM8018_para023[] = {0x00,0x91};             //VGH=15V, VGL=-12V, pump ratio:VGH=6x, VGL=-5x                  
static char OTM8018_para024[] = {0xc5,0x49,0x50};                                                                         
static char OTM8018_para025[] = {0x00,0x00};             //GVDD=4.504V, NGVDD=-4.504V                                     
static char OTM8018_para026[] = {0xd8,0xb6,0xb6};//{0xd8,0x9d,0x9d};                                                                         
static char OTM8018_para027[] = {0x00,0x00};             //VCOMDC=-0.1212 for gvdd=4.5                                    
static char OTM8018_para028[] = {0xd9,0x5e};                                                                              
static char OTM8018_para029[] = {0x00,0x81};             //source bias 0.75uA                                             
static char OTM8018_para030[] = {0xc4,0x82};                                                                              
static char OTM8018_para031[] = {0x00,0xb0};             //VDD_18V=1.6V, LVDSVDD=1.55V                                    
static char OTM8018_para032[] = {0xc5,0x04,0x78};//0xb8};                                                                         
static char OTM8018_para033[] = {0x00,0xbb};             //LVD voltage level setting                                      
static char OTM8018_para034[] = {0xc5,0x80};                                                                              
static char OTM8018_para035[] = {0x00,0x82};		//chopper                                                                 
static char OTM8018_para036[] = {0xC4,0x02};                                                                              
static char OTM8018_para037[] = {0x00,0x00};             //ID1                                                            
static char OTM8018_para038[] = {0xd0,0x40};                                                                              
static char OTM8018_para039[] = {0x00,0x00};             //ID2, ID3                                                       
static char OTM8018_para040[] = {0xd1,0x00,0x00};                                                                         
static char OTM8018_para041[] = {0x00,0x80};             //source blanking frame = black, defacult='30'                   
static char OTM8018_para042[] = {0xc4,0x00};                                                                              
static char OTM8018_para043[] = {0x00,0x98};             //vcom discharge=gnd:'10', '00'=disable                          
static char OTM8018_para044[] = {0xc5,0x10};                                                                              
static char OTM8018_para045[] = {0x00,0x81};                                                                              
static char OTM8018_para046[] = {0xf5,0x15};  // ibias off                                                                
static char OTM8018_para047[] = {0x00,0x83};                                                                              
static char OTM8018_para048[] = {0xf5,0x15};  // lvd off                                                                  
static char OTM8018_para049[] = {0x00,0x85};                                                                              
static char OTM8018_para050[] = {0xf5,0x15};  // gvdd off                                                                 
static char OTM8018_para051[] = {0x00,0x87};                                                                              
static char OTM8018_para052[] = {0xf5,0x15};  // lvdsvdd off                                                              
static char OTM8018_para053[] = {0x00,0x89};                                                                              
static char OTM8018_para054[] = {0xf5,0x15};  // nvdd_18 off                                                              
static char OTM8018_para055[] = {0x00,0x8b};                                                                              
static char OTM8018_para056[] = {0xf5,0x15};  // en_vcom off                                                              
static char OTM8018_para057[] = {0x00,0x95};                                                                              
static char OTM8018_para058[] = {0xf5,0x15};  // pump3 off                                                                
static char OTM8018_para059[] = {0x00,0x97};                                                                              
static char OTM8018_para060[] = {0xf5,0x15};  // pump4 off                                                                
static char OTM8018_para061[] = {0x00,0x99};                                                                              
static char OTM8018_para062[] = {0xf5,0x15};  // pump5 off                                                                
static char OTM8018_para063[] = {0x00,0xa1};                                                                              
static char OTM8018_para064[] = {0xf5,0x15};  // gamma off                                                                
static char OTM8018_para065[] = {0x00,0xa3};                                                                              
static char OTM8018_para066[] = {0xf5,0x15};  // sd ibias off                                                             
static char OTM8018_para067[] = {0x00,0xa5};                                                                              
static char OTM8018_para068[] = {0xf5,0x15};  // sdpch off                                                                
static char OTM8018_para069[] = {0x00,0xa7};                                                                              
static char OTM8018_para070[] = {0xf5,0x15};  // sdpch bias off                                                           
static char OTM8018_para071[] = {0x00,0xab};                                                                              
static char OTM8018_para072[] = {0xf5,0x18};  // ddc osc off                                                              
static char OTM8018_para073[] = {0x00,0xb1};             //VGLO, VGHO setting                                             
static char OTM8018_para074[] = {0xf5,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x15,0x08,0x15};                  
static char OTM8018_para075[] = {0x00,0xb4};             //VGLO1/2 Pull low setting                                       
static char OTM8018_para076[] = {0xc5,0xc0};				//d[7] vglo1 d[6] vglo2 => 0: pull vss, 1: pull vgl                   
static char OTM8018_para077[] = {0x00,0x90};             //Mode-3                                                         
static char OTM8018_para078[] = {0xf5,0x02,0x11,0x02,0x11};                                                               
static char OTM8018_para079[] = {0x00,0x90};             //2xVPNL, 1.5*=00, 2*=50, 3*=a0                                  
static char OTM8018_para080[] = {0xc5,0x50};                                                                              
static char OTM8018_para081[] = {0x00,0x94};             //Frequency                                                      
static char OTM8018_para082[] = {0xc5,0x66};                                                                              
static char OTM8018_para083[] = {0x00,0x80};             //panel timing state control                                     
static char OTM8018_para084[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};                            
static char OTM8018_para085[] = {0x00,0x90};             //panel timing state control                                     
static char OTM8018_para086[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0xff,0x00};        
static char OTM8018_para087[] = {0x00,0xa0};             //panel timing state control                                     
static char OTM8018_para088[] = {0xcb,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};        
static char OTM8018_para089[] = {0x00,0xb0};             //panel timing state control                                     
static char OTM8018_para090[] = {0xcb,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00};        
static char OTM8018_para091[] = {0x00,0xc0};             //panel timing state control                                     
static char OTM8018_para092[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x00,0x05,0x05,0x05,0x05,0x05};        
static char OTM8018_para093[] = {0x00,0xd0};             //panel timing state control                                     
static char OTM8018_para094[] = {0xcb,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05};        
static char OTM8018_para095[] = {0x00,0xe0};             //panel timing state control                                     
static char OTM8018_para096[] = {0xcb,0x05,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00};//10-0x05             
static char OTM8018_para097[] = {0x00,0xf0};             //panel timing state control                                     
static char OTM8018_para098[] = {0xcb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};                            
static char OTM8018_para099[] = {0x00,0x80};             //panel pad mapping control                                      
static char OTM8018_para100[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x07,0x00,0x0d,0x09,0x0f,0x0b,0x11};        
static char OTM8018_para101[] = {0x00,0x90};             //panel pad mapping control                                      
static char OTM8018_para102[] = {0xcc,0x15,0x13,0x17,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06};        
static char OTM8018_para103[] = {0x00,0xa0};             //panel pad mapping control                                      
static char OTM8018_para104[] = {0xcc,0x08,0x00,0x0e,0x0a,0x10,0x0c,0x12,0x16,0x14,0x18,0x02,0x04,0x00,0x00};             
static char OTM8018_para105[] = {0x00,0xb0};             //panel pad mapping control                                      
static char OTM8018_para106[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x02,0x00,0x0c,0x10,0x0a,0x0e,0x18};        
static char OTM8018_para107[] = {0x00,0xc0};             //panel pad mapping control                                      
static char OTM8018_para108[] = {0xcc,0x14,0x16,0x12,0x08,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03};        
static char OTM8018_para109[] = {0x00,0xd0};             //panel pad mapping control                                      
static char OTM8018_para110[] = {0xcc,0x01,0x00,0x0b,0x0f,0x09,0x0d,0x17,0x13,0x15,0x11,0x07,0x05,0x00,0x00};             
static char OTM8018_para111[] = {0x00,0x80};             //panel VST setting                                              
static char OTM8018_para112[] = {0xce,0x87,0x03,0x28,0x86,0x03,0x28,0x85,0x03,0x28,0x84,0x03,0x28};                       
static char OTM8018_para113[] = {0x00,0x90};             //panel VEND setting                                             
static char OTM8018_para114[] = {0xce,0x34,0xfc,0x28,0x34,0xfd,0x28,0x34,0xfe,0x28,0x34,0xff,0x28,0x00,0x00};             
static char OTM8018_para115[] = {0x00,0xa0};             //panel CLKA1/2 setting                                          
static char OTM8018_para116[] = {0xce,0x38,0x07,0x05,0x00,0x00,0x28,0x00,0x38,0x06,0x05,0x01,0x00,0x28,0x00};             
static char OTM8018_para117[] = {0x00,0xb0};             //panel CLKA3/4 setting                                          
static char OTM8018_para118[] = {0xce,0x38,0x05,0x05,0x02,0x00,0x28,0x00,0x38,0x04,0x05,0x03,0x00,0x28,0x00};             
static char OTM8018_para119[] = {0x00,0xc0};             //panel CLKb1/2 setting                                          
static char OTM8018_para120[] = {0xce,0x38,0x03,0x05,0x04,0x00,0x28,0x00,0x38,0x02,0x05,0x05,0x00,0x28,0x00};             
static char OTM8018_para121[] = {0x00,0xd0};             //panel CLKb3/4 setting                                          
static char OTM8018_para122[] = {0xce,0x38,0x01,0x05,0x06,0x00,0x28,0x00,0x38,0x00,0x05,0x07,0x00,0x28,0x00};             
static char OTM8018_para123[] = {0x00,0x80};             //panel CLKc1/2 setting                                          
static char OTM8018_para124[] = {0xcf,0x78,0x07,0x04,0xff,0x00,0x18,0x10,0x78,0x06,0x05,0x00,0x00,0x18,0x10};             
static char OTM8018_para125[] = {0x00,0x90};             //panel CLKc3/4 setting                                          
static char OTM8018_para126[] = {0xcf,0x78,0x05,0x05,0x01,0x00,0x18,0x10,0x78,0x04,0x05,0x02,0x00,0x18,0x10};             
static char OTM8018_para127[] = {0x00,0xa0};             //panel CLKd1/2 setting                                          
static char OTM8018_para128[] = {0xcf,0x70,0x00,0x05,0x00,0x00,0x18,0x10,0x70,0x01,0x05,0x01,0x00,0x18,0x10};             
static char OTM8018_para129[] = {0x00,0xb0};             //panel CLKd3/4 setting                                          
static char OTM8018_para130[] = {0xcf,0x70,0x02,0x05,0x02,0x00,0x18,0x10,0x70,0x03,0x05,0x03,0x00,0x18,0x10};             
static char OTM8018_para131[] = {0x00,0xc0};             //panel ECLK setting                                             
static char OTM8018_para132[] = {0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x81,0x00,0x03,0x08}; //gate pre. ena.           
static char OTM8018_para133[] = {0x00,0x00};			                                                                        
static char OTM8018_para134[] = {0xE1,0x00,0x07,0x0c,0x0e,0x06,0x11,0x0d,0x0d,0x01,0x05,0x0f,0x0f,0x17,0x30,0x25,0x06};//{0xE1,0x00,0x04,0x08,0x0B,0x04,0x0D,0x0C,0x0C,0x03,0x06,0x12,0x0F,0x17,0x30,0x25,0x06};   
static char OTM8018_para135[] = {0x00,0x00};                                                                              
static char OTM8018_para136[] = {0xE2,0x00,0x06,0x0c,0x0e,0x06,0x11,0x0c,0x0e,0x00,0x05,0x0f,0x0F,0x17,0x30,0x25,0x06};//{0xE2,0x00,0x04,0x08,0x0B,0x04,0x0D,0x0C,0x0C,0x03,0x06,0x12,0x0F,0x17,0x30,0x25,0x06};   

/***add***/
//static char OTM8018_para_00_1[] = {0x00,0xc3};
//static char OTM8018_para_f5[] = {0xf5,0x81};
//static char OTM8018_para_00_2[] = {0xf5,0x82};
//static char OTM8018_para_f4[] = {0xf4,0x00};
/*end*/

static char OTM8018_para137[] = {0x00,0x00};             //Orise mode disable                                             
static char OTM8018_para138[] = {0xff,0xff,0xff,0xff}; 



static char OTM8018_para139[] = {0x11,0x00};
static char OTM8018_para140[] = {0x29,0x00};

static struct dsi_cmd_desc orise_video_on_cmds[] = {

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para001), OTM8018_para001},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para002), OTM8018_para002},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para003), OTM8018_para003},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para004), OTM8018_para004},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para005), OTM8018_para005},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para006), OTM8018_para006},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para007), OTM8018_para007},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para008), OTM8018_para008},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para009), OTM8018_para009},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para010), OTM8018_para010},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para011), OTM8018_para011},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para012), OTM8018_para012},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para013), OTM8018_para013},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para014), OTM8018_para014},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para015), OTM8018_para015},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para016), OTM8018_para016},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para017), OTM8018_para017},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para018), OTM8018_para018},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para019), OTM8018_para019},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para020), OTM8018_para020},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para021), OTM8018_para021},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para022), OTM8018_para022},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para023), OTM8018_para023},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para024), OTM8018_para024},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para025), OTM8018_para025},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para026), OTM8018_para026},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para027), OTM8018_para027},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para028), OTM8018_para028},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para029), OTM8018_para029},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para030), OTM8018_para030},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para031), OTM8018_para031},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para032), OTM8018_para032},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para033), OTM8018_para033},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para034), OTM8018_para034},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para035), OTM8018_para035},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para036), OTM8018_para036},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para037), OTM8018_para037},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para038), OTM8018_para038},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para039), OTM8018_para039},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para040), OTM8018_para040},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para041), OTM8018_para041},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para042), OTM8018_para042},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para043), OTM8018_para043},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para044), OTM8018_para044},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para045), OTM8018_para045},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para046), OTM8018_para046},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para047), OTM8018_para047},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para048), OTM8018_para048},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para049), OTM8018_para049},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para050), OTM8018_para050},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para051), OTM8018_para051},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para052), OTM8018_para052},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para053), OTM8018_para053},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para054), OTM8018_para054},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para055), OTM8018_para055},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para056), OTM8018_para056},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para057), OTM8018_para057},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para058), OTM8018_para058},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para059), OTM8018_para059},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para060), OTM8018_para060},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para061), OTM8018_para061},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para062), OTM8018_para062},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para063), OTM8018_para063},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para064), OTM8018_para064},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para065), OTM8018_para065},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para066), OTM8018_para066},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para067), OTM8018_para067},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para068), OTM8018_para068},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para069), OTM8018_para069},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para070), OTM8018_para070},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para071), OTM8018_para071},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para072), OTM8018_para072},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para073), OTM8018_para073},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para074), OTM8018_para074},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para075), OTM8018_para075},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para076), OTM8018_para076},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para077), OTM8018_para077},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para078), OTM8018_para078},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para079), OTM8018_para079},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para080), OTM8018_para080},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para081), OTM8018_para081},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para082), OTM8018_para082},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para083), OTM8018_para083},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para084), OTM8018_para084},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para085), OTM8018_para085},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para086), OTM8018_para086},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para087), OTM8018_para087},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para088), OTM8018_para088},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para089), OTM8018_para089},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para090), OTM8018_para090},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para091), OTM8018_para091},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para092), OTM8018_para092},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para093), OTM8018_para093},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para094), OTM8018_para094},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para095), OTM8018_para095},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para096), OTM8018_para096},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para097), OTM8018_para097},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para098), OTM8018_para098},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para099), OTM8018_para099},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para100), OTM8018_para100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para101), OTM8018_para101},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para102), OTM8018_para102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para103), OTM8018_para103},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para104), OTM8018_para104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para105), OTM8018_para105},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para106), OTM8018_para106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para107), OTM8018_para107},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para108), OTM8018_para108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para109), OTM8018_para109},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para110), OTM8018_para110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para111), OTM8018_para111},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para112), OTM8018_para112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para113), OTM8018_para113},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para114), OTM8018_para114},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para115), OTM8018_para115},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para116), OTM8018_para116},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para117), OTM8018_para117},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para118), OTM8018_para118},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para119), OTM8018_para119},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para120), OTM8018_para120},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para121), OTM8018_para121},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para122), OTM8018_para122},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para123), OTM8018_para123},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para124), OTM8018_para124},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para125), OTM8018_para125},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para126), OTM8018_para126},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para127), OTM8018_para127},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para128), OTM8018_para128},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para129), OTM8018_para129},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para130), OTM8018_para130},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para131), OTM8018_para131},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para132), OTM8018_para132},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para133), OTM8018_para133},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para134), OTM8018_para134},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para135), OTM8018_para135},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM8018_para136), OTM8018_para136},

	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para_00_1), OTM8018_para_00_1},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para_f5), OTM8018_para_f5},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para_00_2), OTM8018_para_00_2},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para_f4), OTM8018_para_f4},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(OTM8018_para137), OTM8018_para137},

#ifdef COLOR_ENHANCE

	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para01),SSD2825_write_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para02),SSD2825_write_para02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para03),SSD2825_write_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para04),SSD2825_write_para04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para05),SSD2825_write_para05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para06),SSD2825_write_para06},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para07),SSD2825_write_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para08),SSD2825_write_para08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para09),SSD2825_write_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para10),SSD2825_write_para10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para11),SSD2825_write_para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para12),SSD2825_write_para12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para13),SSD2825_write_para13},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para14),SSD2825_write_para14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para15),SSD2825_write_para15},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para16),SSD2825_write_para16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 1, sizeof(SSD2825_write_para17),SSD2825_write_para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(SSD2825_write_para18),SSD2825_write_para18},

#endif

	{DTYPE_DCS_LWRITE, 1, 0, 0, 20, sizeof(OTM8018_para138), OTM8018_para138},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(OTM8018_para139), OTM8018_para139},
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(OTM8018_para140), OTM8018_para140}, 
	

};
#endif
static char boe_otm1283a_para001[] = {0x00,0x00};
static char boe_otm1283a_para002[] = {0xff,0x12,0x83,0x01};	//EXTC=1
static char boe_otm1283a_para003[] = {0x00,0x80};	            //Orise mode enable
static char boe_otm1283a_para004[] = {0xff,0x12,0x83};
static char boe_otm1283a_para005[] = {0x00,0x80};             //TCON Setting
static char boe_otm1283a_para006[] = {0xc0,0x00,0x64,0x00,0x0f,0x11,0x00,0x64,0x0f,0x11};
static char boe_otm1283a_para007[] = {0x00,0x90};             //Panel Timing Setting
static char boe_otm1283a_para008[] = {0xc0,0x00,0x55,0x00,0x01,0x00,0x04};   //Charge time setting,gate and source
static char boe_otm1283a_para009[] = {0x00,0xa4};             //source pre.
static char boe_otm1283a_para010[] = {0xc0,0x00};

static char boe_otm1283a_para011[] = {0x00,0xb3};             //Interval Scan Frame: 0 frame, column inversion
static char boe_otm1283a_para012[] = {0xc0,0x00,0x50};

static char boe_otm1283a_para013[] = {0x00,0x81};             //frame rate:60Hz
static char boe_otm1283a_para014[] = {0xc1,0x55};
static char boe_otm1283a_para015[] = {0x00,0x81};             //source bias 0.75uA
static char boe_otm1283a_para016[] = {0xc4,0x82};                                                                                     
static char boe_otm1283a_para017[] = {0x00,0x82};             //flash-orise add                                                       
static char boe_otm1283a_para018[] = {0xc4,0x02};                                                                                     
static char boe_otm1283a_para019[] = {0x00,0x90};             //clock delay for data latch                                            
static char boe_otm1283a_para020[] = {0xc4,0x49};                                                                                     
static char boe_otm1283a_para021[] = {0x00,0xc6};  //debounce                                                              
static char boe_otm1283a_para022[] = {0xB0,0x03};              
static char boe_otm1283a_para023[] = {0x00,0x90};
static char boe_otm1283a_para024[] = {0xf5,0x02,0x11,0x02,0x11};
static char boe_otm1283a_para025[] = {0x00,0x90};             //3xVPNL
static char boe_otm1283a_para026[] = {0xc5,0x50};
static char boe_otm1283a_para027[] = {0x00,0x94};             //2xVPNL
static char boe_otm1283a_para028[] = {0xc5,0x66};
static char boe_otm1283a_para029[] = {0x00,0xb2};             //VGLO1
static char boe_otm1283a_para030[] = {0xf5,0x00,0x00};
static char boe_otm1283a_para031[] = {0x00,0xb4};             //VGLO1_S
static char boe_otm1283a_para032[] = {0xf5,0x00,0x00};
static char boe_otm1283a_para033[] = {0x00,0xb6};             //VGLO2
static char boe_otm1283a_para034[] = {0xf5,0x00,0x00};
static char boe_otm1283a_para035[] = {0x00,0xb8};             //VGLO2_S
static char boe_otm1283a_para036[] = {0xf5,0x00,0x00};

static char boe_otm1283a_para037[] = {0x00,0xb2};             //C31 cap. not remove
static char boe_otm1283a_para038[] = {0xc5,0x40};
static char boe_otm1283a_para039[] = {0x00,0xb4};             //VGLO1/2 Pull low setting
static char boe_otm1283a_para040[] = {0xc5,0xc0};		//d[7] vglo1 d[6] vglo2 => 0: pull vss, 1: pull vgl


static char boe_otm1283a_para040_1[] = {0x00,0x94};   //pan add 0924
static char boe_otm1283a_para040_2[] = {0xf5,0x02};   
static char boe_otm1283a_para040_3[] = {0x00,0xba};   
static char boe_otm1283a_para040_4[] = {0xf5,0x03};  

static char boe_otm1283a_para041[] = {0x00,0xa0};             //dcdc setting PFM Fre}
static char boe_otm1283a_para042[] = {0xc4,0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10};
static char boe_otm1283a_para043[] = {0x00,0xb0};             //clamp voltage setting
static char boe_otm1283a_para044[] = {0xc4,0x00,0x00};         //VSP and VSN Change 5.6V,-5.6V}
static char boe_otm1283a_para045[] = {0x00,0x91};             //VGH=12V, VGL=-12V, pump ratio:VGH=6x, VGL=-5x
static char boe_otm1283a_para046[] = {0xc5,0x19,0x50};
static char boe_otm1283a_para047[] = {0x00,0x00};             //GVDD=4.87V, NGVDD=-4.87V
static char boe_otm1283a_para048[] = {0xd8,0xbc,0xbc};
static char boe_otm1283a_para049[] = {0x00,0xb0};             //VDD_18V=1.6V, LVDSVDD=1.55V
static char boe_otm1283a_para050[] = {0xc5,0x04,0xb8};

//static char boe_otm1283a_para050_1[] = {0x00,0x82};		//pan add 0924  pan del 1112
//static char boe_otm1283a_para050_2[] = {0xf5,0x00,0x00};

static char boe_otm1283a_para051[] = {0x00,0xbb};             //LVD voltage level setting
static char boe_otm1283a_para052[] = {0xc5,0x80};
static char boe_otm1283a_para053[] = {0x00,0x00};             //ID1
static char boe_otm1283a_para054[] = {0xd0,0x40};
static char boe_otm1283a_para055[] = {0x00,0x00};             //ID2, ID3

static char boe_otm1283a_para056[] = {0xd1,0x00,0x00};
static char boe_otm1283a_para057[] = {0x00,0x00};
static char boe_otm1283a_para058[] = {0xE1,0x05,0x12,0x19,0x0f,0x07,0x10,0x0c,0x0b,0x00,0x06,0x0c,0x06,0x0f,0x11,0x0c,0x05};
static char boe_otm1283a_para059[] = {0x00,0x00};
static char boe_otm1283a_para060[] = {0xE2,0x05,0x11,0x16,0x0d,0x05,0x0c,0x0c,0x0b,0x04,0x06,0x10,0x09,0x10,0x11,0x0c,0x05};

//static char boe_otm1283a_para061[] = {0x00,0x00};// pan,maybe we add this ourselves before ?   pan del 1112
//static char boe_otm1283a_para062[] = {0xD9,0x73};

static char boe_otm1283a_para063[] = {0x00,0x80}; //panel timing state control

static char boe_otm1283a_para064[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para065[] = {0x00,0x90};             //panel timing state control
static char boe_otm1283a_para066[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para067[] = {0x00,0xa0};             //panel timing state control
static char boe_otm1283a_para068[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para069[] = {0x00,0xb0};             //panel timing state control
static char boe_otm1283a_para070[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para071[] = {0x00,0xc0};             //panel timing state control
static char boe_otm1283a_para072[] = {0xcb,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para073[] = {0x00,0xd0};             //panel timing state control
static char boe_otm1283a_para074[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00};
static char boe_otm1283a_para075[] = {0x00,0xe0};             //panel timing state control
static char boe_otm1283a_para076[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05};
static char boe_otm1283a_para077[] = {0x00,0xf0};             //panel timing state control
static char boe_otm1283a_para078[] = {0xcb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};
static char boe_otm1283a_para079[] = {0x00,0x80};             //panel pad mapping control
static char boe_otm1283a_para080[] = {0xcc,0x0a,0x0c,0x0e,0x10,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para081[] = {0x00,0x90};             //panel pad mapping control
static char boe_otm1283a_para082[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x2e,0x2d,0x09,0x0b,0x0d,0x0f,0x01,0x03,0x00,0x00};
static char boe_otm1283a_para083[] = {0x00,0xa0};             //panel pad mapping control
static char boe_otm1283a_para084[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e,0x2d};
static char boe_otm1283a_para085[] = {0x00,0xb0};             //panel pad mapping control
static char boe_otm1283a_para086[] = {0xcc,0x0F,0x0D,0x0B,0x09,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char boe_otm1283a_para087[] = {0x00,0xc0};             //panel pad mapping control
static char boe_otm1283a_para088[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x2d,0x2e,0x10,0x0E,0x0C,0x0A,0x04,0x02,0x00,0x00};
static char boe_otm1283a_para089[] = {0x00,0xd0};             //panel pad mapping control
static char boe_otm1283a_para090[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2d,0x2e};
static char boe_otm1283a_para091[] = {0x00,0x80};             //panel VST setting
static char boe_otm1283a_para092[] = {0xce,0x8d,0x03,0x00,0x8c,0x03,0x00,0x8b,0x03,0x00,0x8a,0x03,0x00};
static char boe_otm1283a_para093[] = {0x00,0x90};             //panel VEND setting
static char boe_otm1283a_para094[] = {0xce,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para095[] = {0x00,0xa0};             //panel CLKA1/2 setting
static char boe_otm1283a_para096[] = {0xce,0x38,0x0b,0x04,0xfc,0x00,0x10,0x10,0x38,0x0a,0x04,0xfd,0x00,0x10,0x10};
static char boe_otm1283a_para097[] = {0x00,0xb0};             //panel CLKA3/4 setting
static char boe_otm1283a_para098[] = {0xce,0x38,0x09,0x04,0xfe,0x00,0x10,0x10,0x38,0x08,0x04,0xff,0x00,0x10,0x10};
static char boe_otm1283a_para099[] = {0x00,0xc0};             //panel CLKb1/2 setting
static char boe_otm1283a_para100[] = {0xce,0x38,0x07,0x05,0x00,0x00,0x10,0x10,0x38,0x06,0x05,0x01,0x00,0x10,0x10};
static char boe_otm1283a_para101[] = {0x00,0xd0};             //panel CLKb3/4 setting
static char boe_otm1283a_para102[] = {0xce,0x38,0x05,0x05,0x02,0x00,0x10,0x10,0x38,0x04,0x05,0x03,0x00,0x10,0x10};
static char boe_otm1283a_para103[] = {0x00,0x80};             //panel CLKc1/2 setting
static char boe_otm1283a_para104[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para105[] = {0x00,0x90};             //panel CLKc3/4 setting
static char boe_otm1283a_para106[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para107[] = {0x00,0xa0};             //panel CLKd1/2 setting
static char boe_otm1283a_para108[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para109[] = {0x00,0xb0};             //panel CLKd3/4 setting
static char boe_otm1283a_para110[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};
static char boe_otm1283a_para111[] = {0x00,0xc0};             //panel ECLK setting, gate pre. ena.
static char boe_otm1283a_para112[] = {0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0x08};
static char boe_otm1283a_para113[] = {0x00,0xb5};             //TCON_GOA_OUT Setting
static char boe_otm1283a_para114[] = {0xc5,0x33,0xf1,0xff,0x33,0xf1,0xff};  //normal output with VGH/VGL

static char boe_otm1283a_para114_1[] ={0x00,0x00};  //pan add rgb swap
static char boe_otm1283a_para114_2[] ={0x36,0x00};  
	
static char boe_otm1283a_para115[] = {0x00,0x00};
static char boe_otm1283a_para116[] = {0xFF,0xFF,0xFF,0xFF};


static struct dsi_cmd_desc boe_otm1283a_display_on_cmds[] =  	
{                                                               
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para001), boe_otm1283a_para001},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para002), boe_otm1283a_para002},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para003), boe_otm1283a_para003},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para004), boe_otm1283a_para004},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para005), boe_otm1283a_para005},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para006), boe_otm1283a_para006},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para007), boe_otm1283a_para007},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para008), boe_otm1283a_para008},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para009), boe_otm1283a_para009},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para010), boe_otm1283a_para010},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para011), boe_otm1283a_para011},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para012), boe_otm1283a_para012},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para013), boe_otm1283a_para013},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para014), boe_otm1283a_para014},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para015), boe_otm1283a_para015},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para016), boe_otm1283a_para016},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para017), boe_otm1283a_para017},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para018), boe_otm1283a_para018},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para019), boe_otm1283a_para019},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para020), boe_otm1283a_para020},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para021), boe_otm1283a_para021},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para022), boe_otm1283a_para022},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para023), boe_otm1283a_para023},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para024), boe_otm1283a_para024},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para025), boe_otm1283a_para025},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para026), boe_otm1283a_para026},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para027), boe_otm1283a_para027},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para028), boe_otm1283a_para028},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para029), boe_otm1283a_para029},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para030), boe_otm1283a_para030},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para031), boe_otm1283a_para031},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para032), boe_otm1283a_para032},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para033), boe_otm1283a_para033},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para034), boe_otm1283a_para034},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para035), boe_otm1283a_para035},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para036), boe_otm1283a_para036},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para037), boe_otm1283a_para037},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para038), boe_otm1283a_para038},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para039), boe_otm1283a_para039},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para040), boe_otm1283a_para040},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para040_1), boe_otm1283a_para040_1},//pan add
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para040_2), boe_otm1283a_para040_2},//pan add
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para040_3), boe_otm1283a_para040_3},//pan add
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para040_4), boe_otm1283a_para040_4},//pan add
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para041), boe_otm1283a_para041},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para042), boe_otm1283a_para042},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para043), boe_otm1283a_para043},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para044), boe_otm1283a_para044},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para045), boe_otm1283a_para045},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para046), boe_otm1283a_para046},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para047), boe_otm1283a_para047},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para048), boe_otm1283a_para048},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para049), boe_otm1283a_para049},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para050), boe_otm1283a_para050},

	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para050_1), boe_otm1283a_para050_1},//pan add
	//{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para050_2), boe_otm1283a_para050_2},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para051), boe_otm1283a_para051},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para052), boe_otm1283a_para052},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para053), boe_otm1283a_para053},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para054), boe_otm1283a_para054},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para055), boe_otm1283a_para055},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para056), boe_otm1283a_para056},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para057), boe_otm1283a_para057},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para058), boe_otm1283a_para058},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para059), boe_otm1283a_para059},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para060), boe_otm1283a_para060},
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para061), boe_otm1283a_para061},
	
	//{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para062), boe_otm1283a_para062},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para063), boe_otm1283a_para063},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para064), boe_otm1283a_para064},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para065), boe_otm1283a_para065},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para066), boe_otm1283a_para066},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para067), boe_otm1283a_para067},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para068), boe_otm1283a_para068},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para069), boe_otm1283a_para069},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para070), boe_otm1283a_para070},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para071), boe_otm1283a_para071},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para072), boe_otm1283a_para072},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para073), boe_otm1283a_para073},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para074), boe_otm1283a_para074},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para075), boe_otm1283a_para075},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para076), boe_otm1283a_para076},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para077), boe_otm1283a_para077},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para078), boe_otm1283a_para078},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para079), boe_otm1283a_para079},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para080), boe_otm1283a_para080},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para081), boe_otm1283a_para081},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para082), boe_otm1283a_para082},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para083), boe_otm1283a_para083},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para084), boe_otm1283a_para084},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para085), boe_otm1283a_para085},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para086), boe_otm1283a_para086},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para087), boe_otm1283a_para087},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para088), boe_otm1283a_para088},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para089), boe_otm1283a_para089},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para090), boe_otm1283a_para090},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para091), boe_otm1283a_para091},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para092), boe_otm1283a_para092},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para093), boe_otm1283a_para093},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para094), boe_otm1283a_para094},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para095), boe_otm1283a_para095},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para096), boe_otm1283a_para096},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para097), boe_otm1283a_para097},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para098), boe_otm1283a_para098},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para099), boe_otm1283a_para099},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para100), boe_otm1283a_para100},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para101), boe_otm1283a_para101},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para102), boe_otm1283a_para102},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para103), boe_otm1283a_para103},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para104), boe_otm1283a_para104},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para105), boe_otm1283a_para105},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para106), boe_otm1283a_para106},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para107), boe_otm1283a_para107},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para108), boe_otm1283a_para108},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para109), boe_otm1283a_para109},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para110), boe_otm1283a_para110},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para111), boe_otm1283a_para111},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para112), boe_otm1283a_para112},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para113), boe_otm1283a_para113},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para114), boe_otm1283a_para114},

	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para114_1), boe_otm1283a_para114_1},//pan add
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para114_2), boe_otm1283a_para114_2},
	
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(boe_otm1283a_para115), boe_otm1283a_para115},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(boe_otm1283a_para116), boe_otm1283a_para116},
#ifdef  COLOR_ENHANCE
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para01),SSD2825_write_para01},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para02),SSD2825_write_para02},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para03),SSD2825_write_para03},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para04),SSD2825_write_para04},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para05),SSD2825_write_para05},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para06),SSD2825_write_para06},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para07),SSD2825_write_para07},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para08),SSD2825_write_para08},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para09),SSD2825_write_para09},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para10),SSD2825_write_para10},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para11),SSD2825_write_para11},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para12),SSD2825_write_para12},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para13),SSD2825_write_para13},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para14),SSD2825_write_para14},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para15),SSD2825_write_para15},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para16),SSD2825_write_para16},
	{DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para17),SSD2825_write_para17},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para18),SSD2825_write_para18},

#endif 
                                                                                   
    {DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},                 
    {DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_on), display_on},
  
};

/************end********************************/

/******************* YASSY CPT5.7 OTM1283A added by zzl,2012.12.20**************************/

static char cpt_otm1283a_para001[] = {0x00,0x00};                                                                            
static char cpt_otm1283a_para002[] = {0xff,0x12,0x83,0x01};	//EXTC=1                                                         
static char cpt_otm1283a_para003[] = {0x00,0x80};	            //Orise mode enable                                            
static char cpt_otm1283a_para004[] = {0xff,0x12,0x83};                                                                       
static char cpt_otm1283a_para005[] = {0x00,0x80};             //TCON Setting                                                 
static char cpt_otm1283a_para006[] = {0xc0,0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10};                                    
static char cpt_otm1283a_para007[] = {0x00,0x90};             //Panel Timing Setting                                         
static char cpt_otm1283a_para008[] = {0xc0,0x00,0x5c,0x00,0x01,0x00,0x04};                                                   
static char cpt_otm1283a_para009[] = {0x00,0xa4};             //source pre.                                                  
static char cpt_otm1283a_para010[] = {0xc0,0x22};                                                                            
static char cpt_otm1283a_para011[] = {0x00,0xb3};             //Interval Scan Frame: 0 frame, column inversion               
static char cpt_otm1283a_para012[] = {0xc0,0x00,0x50};                                                                       
static char cpt_otm1283a_para013[] = {0x00,0x81};             //frame rate:60Hz                                              
static char cpt_otm1283a_para014[] = {0xc1,0x55};                                                                            
static char cpt_otm1283a_para015[] = {0x00,0x90};             //clock delay for data latch                                   
static char cpt_otm1283a_para016[] = {0xc4,0x49};                                                                            
static char cpt_otm1283a_para017[] = {0x00,0xa0};             //dcdc setting                                                 
static char cpt_otm1283a_para018[] = {0xc4,0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10};           
static char cpt_otm1283a_para019[] = {0x00,0xb0};             //clamp voltage setting                                        
static char cpt_otm1283a_para020[] = {0xc4,0x00,0x00};                                                                       
static char cpt_otm1283a_para021[] = {0x00,0x91};             //VGH=15V, VGL=-10V, pump ratio:VGH=6x, VGL=-5x                
static char cpt_otm1283a_para022[] = {0xc5,0x46,0x40};                                                                       
static char cpt_otm1283a_para023[] = {0x00,0x00};             //GVDD=4.87V, NGVDD=-4.87V                                     
static char cpt_otm1283a_para024[] = {0xd8,0xbc,0xbc};                                                                       
static char cpt_otm1283a_para025[] = {0x00,0x00};             //VCOMDC=-0.9                                                  
static char cpt_otm1283a_para026[] = {0xd9,0x44};                                                                            
static char cpt_otm1283a_para027[] = {0x00,0x81};             //source bias 0.75uA                                           
static char cpt_otm1283a_para028[] = {0xc4,0x82};                                                                            
static char cpt_otm1283a_para029[] = {0x00,0xb0};             //VDD_18V=1.6V, LVDSVDD=1.55V                                  
static char cpt_otm1283a_para030[] = {0xc5,0x04,0xb8};                                                                       
static char cpt_otm1283a_para031[] = {0x00,0xbb};             //LVD voltage level setting                                    
static char cpt_otm1283a_para032[] = {0xc5,0x80};                                                                            
static char cpt_otm1283a_para033[] = {0x00,0x82};		// chopper 0: frame 2: line 4: disable                                   
static char cpt_otm1283a_para034[] = {0xC4,0x02};                                                                            
static char cpt_otm1283a_para035[] = {0x00,0xc6};		// debounce                                                              
static char cpt_otm1283a_para036[] = {0xB0,0x03};                                                                            
static char cpt_otm1283a_para037[] = {0x00,0x00};             //ID1                                                          
static char cpt_otm1283a_para038[] = {0xd0,0x40};                                                                            
static char cpt_otm1283a_para039[] = {0x00,0x00};             //ID2, ID3                                                     
static char cpt_otm1283a_para040[] = {0xd1,0x00,0x00};                                                                       
static char cpt_otm1283a_para041[] = {0x00,0x80};             //panel timing state control                                   
static char cpt_otm1283a_para042[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};                          
static char cpt_otm1283a_para043[] = {0x00,0x90};             //panel timing state control                                   
static char cpt_otm1283a_para044[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      
static char cpt_otm1283a_para045[] = {0x00,0xa0};             //panel timing state control                                   
static char cpt_otm1283a_para046[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      
static char cpt_otm1283a_para047[] = {0x00,0xb0};             //panel timing state control                                   
static char cpt_otm1283a_para048[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      
static char cpt_otm1283a_para049[] = {0x00,0xc0};             //panel timing state control                                   
static char cpt_otm1283a_para050[] = {0xcb,0x05,0x00,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      
static char cpt_otm1283a_para051[] = {0x00,0xd0};             //panel timing state control                                   
static char cpt_otm1283a_para052[] = {0xcb,0x00,0x00,0x00,0x05,0x00,0x05,0x05,0x05,0x00,0x05,0x05,0x05,0x05,0x00,0x00};      
static char cpt_otm1283a_para053[] = {0x00,0xe0};             //panel timing state control                                   
static char cpt_otm1283a_para054[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x05,0x05};           
static char cpt_otm1283a_para055[] = {0x00,0xf0};             //panel timing state control                                   
static char cpt_otm1283a_para056[] = {0xcb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};                          
static char cpt_otm1283a_para057[] = {0x00,0x80};             //panel pad mapping control                                    
static char cpt_otm1283a_para058[] = {0xcc,0x02,0x00,0x0a,0x0e,0x0c,0x10,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      
static char cpt_otm1283a_para059[] = {0x00,0x90};             //panel pad mapping control                                    
static char cpt_otm1283a_para060[] = {0xcc,0x00,0x00,0x00,0x06,0x00,0x2e,0x2d,0x01,0x00,0x09,0x0d,0x0b,0x0f,0x00,0x00};      
static char cpt_otm1283a_para061[] = {0x00,0xa0};             //panel pad mapping control                                    
static char cpt_otm1283a_para062[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x00,0x2e,0x2d};           
static char cpt_otm1283a_para063[] = {0x00,0xb0};             //panel pad mapping control                                    
static char cpt_otm1283a_para064[] = {0xcc,0x05,0x00,0x0f,0x0b,0x0d,0x09,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};      
static char cpt_otm1283a_para065[] = {0x00,0xc0};             //panel pad mapping control                                    
static char cpt_otm1283a_para066[] = {0xcc,0x00,0x00,0x00,0x01,0x00,0x2d,0x2e,0x06,0x00,0x10,0x0c,0x0e,0x0a,0x00,0x00};      
static char cpt_otm1283a_para067[] = {0x00,0xd0};             //panel pad mapping control                                    
static char cpt_otm1283a_para068[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x2d,0x2e};           
static char cpt_otm1283a_para069[] = {0x00,0x80};             //panel VST setting                                            
static char cpt_otm1283a_para070[] = {0xce,0x87,0x03,0x18,0x86,0x03,0x18,0x00,0x00,0x00,0x00,0x00,0x00};                     
static char cpt_otm1283a_para071[] = {0x00,0x90};             //panel VEND setting                                           
static char cpt_otm1283a_para072[] = {0xce,0x34,0xfe,0x18,0x34,0xff,0x18,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};           
static char cpt_otm1283a_para073[] = {0x00,0xa0};             //panel CLKA1/2 setting                                        
static char cpt_otm1283a_para074[] = {0xce,0x38,0x03,0x05,0x00,0x00,0x18,0x00,0x38,0x02,0x05,0x01,0x00,0x18,0x00};           
static char cpt_otm1283a_para075[] = {0x00,0xb0};             //panel CLKA3/4 setting                                        
static char cpt_otm1283a_para076[] = {0xce,0x38,0x01,0x05,0x02,0x00,0x18,0x00,0x38,0x00,0x05,0x03,0x00,0x18,0x00};           
static char cpt_otm1283a_para077[] = {0x00,0xc0};             //panel CLKb1/2 setting                                        
static char cpt_otm1283a_para078[] = {0xce,0x30,0x00,0x05,0x04,0x00,0x18,0x00,0x30,0x01,0x05,0x05,0x00,0x18,0x00};           
static char cpt_otm1283a_para079[] = {0x00,0xd0};             //panel CLKb3/4 setting                                        
static char cpt_otm1283a_para080[] = {0xce,0x30,0x02,0x05,0x06,0x00,0x18,0x00,0x30,0x03,0x05,0x07,0x00,0x18,0x00};           
static char cpt_otm1283a_para081[] = {0x00,0x80};             //panel CLKc1/2 setting                                        
static char cpt_otm1283a_para082[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};           
static char cpt_otm1283a_para083[] = {0x00,0x90};             //panel CLKc3/4 setting                                        
static char cpt_otm1283a_para084[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};           
static char cpt_otm1283a_para085[] = {0x00,0xa0};             //panel CLKd1/2 setting                                        
static char cpt_otm1283a_para086[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};           
static char cpt_otm1283a_para087[] = {0x00,0xb0};             //panel CLKd3/4 setting                                        
static char cpt_otm1283a_para088[] = {0xcf,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};           
static char cpt_otm1283a_para089[] = {0x00,0xc0};             //panel ECLK setting                                           
static char cpt_otm1283a_para090[] = {0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x81,0x00,0x03,0x08}; //gate pre. ena.         
static char cpt_otm1283a_para091[] = {0x00,0xb5};             //TCON_GOA_OUT Setting                                         
static char cpt_otm1283a_para092[] = {0xc5,0x37,0xf1,0xfd,0x37,0xf1,0xfd};                                                   
static char cpt_otm1283a_para093[] = {0x00,0x90};             //Mode-3                                                       
static char cpt_otm1283a_para094[] = {0xf5,0x02,0x11,0x02,0x11};                                                             
static char cpt_otm1283a_para095[] = {0x00,0x90};             //3xVPNL                                                       
static char cpt_otm1283a_para096[] = {0xc5,0x50};                                                                            
static char cpt_otm1283a_para097[] = {0x00,0x94};             //2xVPNL                                                       
static char cpt_otm1283a_para098[] = {0xc5,0x66};                                                                            
static char cpt_otm1283a_para099[] = {0x00,0xb2};             //VGLO1                                                        
static char cpt_otm1283a_para100[] = {0xf5,0x00,0x00};                                                                       
static char cpt_otm1283a_para101[] = {0x00,0xb4};             //VGLO1_S                                                      
static char cpt_otm1283a_para102[] = {0xf5,0x00,0x00};                                                                       
static char cpt_otm1283a_para103[] = {0x00,0xb6};             //VGLO2                                                        
static char cpt_otm1283a_para104[] = {0xf5,0x00,0x00};                                                                       
static char cpt_otm1283a_para105[] = {0x00,0xb8};             //VGLO2_S                                                      
static char cpt_otm1283a_para106[] = {0xf5,0x00,0x00};                                                                       
static char cpt_otm1283a_para107[] = {0x00,0xb4};             //VGLO1/2 Pull low setting                                     
static char cpt_otm1283a_para108[] = {0xc5,0xc0};		//d[7] vglo1 d[6] vglo2 => 0: pull vss, 1: pull vgl                      
static char cpt_otm1283a_para109[] = {0x00,0xb2};             //C31 cap. not remove                                          
static char cpt_otm1283a_para110[] = {0xc5,0x40};                                                                            
static char cpt_otm1283a_para111[] = {0x00,0x00};                                                                            
static char cpt_otm1283a_para112[] = {0xE1,0x00,0x16,0x1d,0x0e,0x07,0x10,0x0c,0x0c,0x03,0x06,0x0b,0x07,0x0e,0x12,0x0d,0x06}; 
static char cpt_otm1283a_para113[] = {0x00,0x00};                                                                            
static char cpt_otm1283a_para114[] = {0xE2,0x00,0x16,0x1d,0x0e,0x07,0x10,0x0c,0x0c,0x03,0x06,0x0b,0x07,0x0e,0x12,0x0d,0x06}; 
static char cpt_otm1283a_para115[] = {0x00,0x00};             //Orise mode disable                                           
static char cpt_otm1283a_para116[] = {0xff,0xff,0xff,0xff};                                                                  

static struct dsi_cmd_desc cpt_otm1283a_display_on_cmds[] =  	                      
{                                                                                   
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para001), cpt_otm1283a_para001},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para002), cpt_otm1283a_para002},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para003), cpt_otm1283a_para003},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para004), cpt_otm1283a_para004},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para005), cpt_otm1283a_para005},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para006), cpt_otm1283a_para006},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para007), cpt_otm1283a_para007},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para008), cpt_otm1283a_para008},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para009), cpt_otm1283a_para009},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para010), cpt_otm1283a_para010},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para011), cpt_otm1283a_para011},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para012), cpt_otm1283a_para012},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para013), cpt_otm1283a_para013},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para014), cpt_otm1283a_para014},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para015), cpt_otm1283a_para015},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para016), cpt_otm1283a_para016},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para017), cpt_otm1283a_para017},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para018), cpt_otm1283a_para018},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para019), cpt_otm1283a_para019},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para020), cpt_otm1283a_para020},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para021), cpt_otm1283a_para021},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para022), cpt_otm1283a_para022},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para023), cpt_otm1283a_para023},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para024), cpt_otm1283a_para024},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para025), cpt_otm1283a_para025},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para026), cpt_otm1283a_para026},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para027), cpt_otm1283a_para027},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para028), cpt_otm1283a_para028},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para029), cpt_otm1283a_para029},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para030), cpt_otm1283a_para030},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para031), cpt_otm1283a_para031},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para032), cpt_otm1283a_para032},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para033), cpt_otm1283a_para033},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para034), cpt_otm1283a_para034},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para035), cpt_otm1283a_para035},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para036), cpt_otm1283a_para036},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para037), cpt_otm1283a_para037},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para038), cpt_otm1283a_para038},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para039), cpt_otm1283a_para039},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para040), cpt_otm1283a_para040},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para041), cpt_otm1283a_para041},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para042), cpt_otm1283a_para042},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para043), cpt_otm1283a_para043},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para044), cpt_otm1283a_para044},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para045), cpt_otm1283a_para045},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para046), cpt_otm1283a_para046},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para047), cpt_otm1283a_para047},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para048), cpt_otm1283a_para048},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para049), cpt_otm1283a_para049},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para050), cpt_otm1283a_para050},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para051), cpt_otm1283a_para051},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para052), cpt_otm1283a_para052},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para053), cpt_otm1283a_para053},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para054), cpt_otm1283a_para054},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para055), cpt_otm1283a_para055},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para056), cpt_otm1283a_para056},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para057), cpt_otm1283a_para057},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para058), cpt_otm1283a_para058},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para059), cpt_otm1283a_para059},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para060), cpt_otm1283a_para060},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para061), cpt_otm1283a_para061},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para062), cpt_otm1283a_para062},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para063), cpt_otm1283a_para063},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para064), cpt_otm1283a_para064},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para065), cpt_otm1283a_para065},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para066), cpt_otm1283a_para066},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para067), cpt_otm1283a_para067},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para068), cpt_otm1283a_para068},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para069), cpt_otm1283a_para069},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para070), cpt_otm1283a_para070},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para071), cpt_otm1283a_para071},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para072), cpt_otm1283a_para072},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para073), cpt_otm1283a_para073},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para074), cpt_otm1283a_para074},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para075), cpt_otm1283a_para075},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para076), cpt_otm1283a_para076},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para077), cpt_otm1283a_para077},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para078), cpt_otm1283a_para078},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para079), cpt_otm1283a_para079},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para080), cpt_otm1283a_para080},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para081), cpt_otm1283a_para081},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para082), cpt_otm1283a_para082},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para083), cpt_otm1283a_para083},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para084), cpt_otm1283a_para084},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para085), cpt_otm1283a_para085},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para086), cpt_otm1283a_para086},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para087), cpt_otm1283a_para087},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para088), cpt_otm1283a_para088},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para089), cpt_otm1283a_para089},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para090), cpt_otm1283a_para090},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para091), cpt_otm1283a_para091},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para092), cpt_otm1283a_para092},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para093), cpt_otm1283a_para093},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para094), cpt_otm1283a_para094},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para095), cpt_otm1283a_para095},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para096), cpt_otm1283a_para096},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para097), cpt_otm1283a_para097},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para098), cpt_otm1283a_para098},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para099), cpt_otm1283a_para099},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para100), cpt_otm1283a_para100},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para101), cpt_otm1283a_para101},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para102), cpt_otm1283a_para102},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para103), cpt_otm1283a_para103},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para104), cpt_otm1283a_para104},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para105), cpt_otm1283a_para105},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para106), cpt_otm1283a_para106},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para107), cpt_otm1283a_para107},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para108), cpt_otm1283a_para108},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para109), cpt_otm1283a_para109},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para110), cpt_otm1283a_para110},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para111), cpt_otm1283a_para111},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para112), cpt_otm1283a_para112},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para113), cpt_otm1283a_para113},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para114), cpt_otm1283a_para114},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(cpt_otm1283a_para115), cpt_otm1283a_para115},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(cpt_otm1283a_para116), cpt_otm1283a_para116},
#ifdef  COLOR_ENHANCE
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para01),SSD2825_write_para01},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para02),SSD2825_write_para02},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para03),SSD2825_write_para03},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para04),SSD2825_write_para04},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para05),SSD2825_write_para05},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para06),SSD2825_write_para06},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para07),SSD2825_write_para07},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para08),SSD2825_write_para08},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para09),SSD2825_write_para09},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para10),SSD2825_write_para10},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para11),SSD2825_write_para11},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para12),SSD2825_write_para12},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para13),SSD2825_write_para13},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para14),SSD2825_write_para14},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para15),SSD2825_write_para15},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para16),SSD2825_write_para16},
 {DTYPE_DCS_WRITE1, 1, 0, 0, 0, sizeof(SSD2825_write_para17),SSD2825_write_para17},
 {DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(SSD2825_write_para18),SSD2825_write_para18},
	 
#endif 

 {DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},                 
 {DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_on), display_on},
};

/****************end CPT***********************/



//OTM1283+AUO5.7GAMMA2.2 20131205.txt
static char OTM1283A_auo_param001[] = {0x00,0x00};
static char OTM1283A_auo_param002[] = {0xff,0x12,0x83,0x01};	//EXTC=1
static char OTM1283A_auo_param003[] = {0x00,0x80};	        //Orise mode enable
static char OTM1283A_auo_param004[] = {0xff,0x12,0x83};

static char OTM1283A_auo_param005[] = {0x00,0xc6};		//debounce
static char OTM1283A_auo_param006[] = {0xb0,0x03};


static char OTM1283A_auo_param007[] = {0x00,0x80};             //TCON Setting
static char OTM1283A_auo_param008[] = {0xc0,0x00,0x64,0x00,0x10,0x10,0x00,0x64,0x10,0x10};

static char OTM1283A_auo_param009[] = {0x00,0x90};             //[] = anel Timing Setting
static char OTM1283A_auo_param010[] = {0xc0,0x00,0x5c,0x00,0x01,0x00,0x04};

static char OTM1283A_auo_param011[] = {0x00,0xa2};
static char OTM1283A_auo_param012[] = {0xC0,0x01,0x00,0x00};

static char OTM1283A_auo_param013[] = {0x00,0xb3};             //Interval Scan Frame: 0 frame, column inversion
static char OTM1283A_auo_param014[] = {0xc0,0x00,0x50};

static char OTM1283A_auo_param015[] = {0x00,0x81};             //frame rate:60Hz
static char OTM1283A_auo_param016[] = {0xc1,0x66};

static char OTM1283A_auo_param017[] = {0x00,0x90};             //clock delay for data latch 
static char OTM1283A_auo_param018[] = {0xc4,0x49};

static char OTM1283A_auo_param019[] = {0x00,0x81};             //source bias 0.75uA
static char OTM1283A_auo_param020[] = {0xc4,0x82};

static char OTM1283A_auo_param021[] = {0x00,0x82};		//cho[] = [] = er
static char OTM1283A_auo_param022[] = {0xC4,0x02};

static char OTM1283A_auo_param023[] = {0x00,0xa0};             //dcdc setting
static char OTM1283A_auo_param024[] = {0xc4,0x05,0x10,0x04,0x02,0x05,0x15,0x11,0x05,0x10,0x07,0x02,0x05,0x15,0x11};

static char OTM1283A_auo_param025[] = {0x00,0xb0};             //clam[] =  voltage setting
static char OTM1283A_auo_param026[] = {0xc4,0x00,0x00};

static char OTM1283A_auo_param027[] = {0x00,0x91};             //VGH=13V, VGL=-12V, [] = um[] =  ratio:VGH=6x, VGL=-5x
static char OTM1283A_auo_param028[] = {0xc5,0x29,0x50};

static char OTM1283A_auo_param029[] = {0x00,0xb0};             //VDD_18V=1.6V, LVDSVDD=1.55V
static char OTM1283A_auo_param030[] = {0xc5,0x04,0x38};

static char OTM1283A_auo_param031[] = {0x00,0xbb};             //LVD voltage level setting
static char OTM1283A_auo_param032[] = {0xc5,0x80};

static char OTM1283A_auo_param033[] = {0x00,0xB9}; 
static char OTM1283A_auo_param034[] = {0xB0,0x51};   //S[] = eed U[] =  


static char OTM1283A_auo_param035[] = {0x00,0x00};             //ID1
static char OTM1283A_auo_param036[] = {0xd0,0x40};

static char OTM1283A_auo_param037[] = {0x00,0x00};             //ID2, ID3
static char OTM1283A_auo_param038[] = {0xd1,0x00,0x00};


static char OTM1283A_auo_param039[] = {0x00,0x80};             //source blanking frame = black, defacult='30'
static char OTM1283A_auo_param040[] = {0xc4,0x00};

static char OTM1283A_auo_param041[] = {0x00,0x98};             //vcom discharge=gnd:'10', '00'=disable
static char OTM1283A_auo_param042[] = {0xc5,0x10};

static char OTM1283A_auo_param043[] = {0x00,0x81};
static char OTM1283A_auo_param044[] = {0xf5,0x15};  // ibias off
static char OTM1283A_auo_param045[] = {0x00,0x83}; 
static char OTM1283A_auo_param046[] = {0xf5,0x15};  // lvd off
static char OTM1283A_auo_param047[] = {0x00,0x85};
static char OTM1283A_auo_param048[] = {0xf5,0x15};  // gvdd off
static char OTM1283A_auo_param049[] = {0x00,0x87}; 
static char OTM1283A_auo_param050[] = {0xf5,0x15};  // lvdsvdd off
static char OTM1283A_auo_param051[] = {0x00,0x89};
static char OTM1283A_auo_param052[] = {0xf5,0x15};  // nvdd_18 off
static char OTM1283A_auo_param053[] = {0x00,0x8b}; 
static char OTM1283A_auo_param054[] = {0xf5,0x15};  // en_vcom off

static char OTM1283A_auo_param055[] = {0x00,0x95};
static char OTM1283A_auo_param056[] = {0xf5,0x15};  // [] = um[] = 3 off
static char OTM1283A_auo_param057[] = {0x00,0x97}; 
static char OTM1283A_auo_param058[] = {0xf5,0x15};  // [] = um[] = 4 off
static char OTM1283A_auo_param059[] = {0x00,0x99};
static char OTM1283A_auo_param060[] = {0xf5,0x15};  // [] = um[] = 5 off

static char OTM1283A_auo_param061[] = {0x00,0xa1}; 
static char OTM1283A_auo_param062[] = {0xf5,0x15};  // gamma off
static char OTM1283A_auo_param063[] = {0x00,0xa3};
static char OTM1283A_auo_param064[] = {0xf5,0x15};  // sd ibias off
static char OTM1283A_auo_param065[] = {0x00,0xa5}; 
static char OTM1283A_auo_param066[] = {0xf5,0x15};  // sd[] = ch off
static char OTM1283A_auo_param067[] = {0x00,0xa7}; 
static char OTM1283A_auo_param068[] = {0xf5,0x15};  // sd[] = ch bias off
static char OTM1283A_auo_param069[] = {0x00,0xab};
static char OTM1283A_auo_param070[] = {0xf5,0x18};  // ddc osc off

static char OTM1283A_auo_param071[] = {0x00,0xb1};             //VGLO, VGHO setting
static char OTM1283A_auo_param072[] = {0xf5,0x15,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x15,0x08,0x15};

static char OTM1283A_auo_param073[] = {0x00,0x94};  //VCL on  	
static char OTM1283A_auo_param074[] = {0xF5,0x02};

static char OTM1283A_auo_param075[] = {0x00,0xBA};  //VS[] =  on   	
static char OTM1283A_auo_param076[] = {0xF5,0x03};

static char OTM1283A_auo_param077[] = {0x00,0xb4};        //VGLO1/2 [] = ull low setting
static char OTM1283A_auo_param078[] = {0xc5,0xc0};	//d[7] vglo1 d[6] vglo2 => 0: [] = ull vss, 1: [] = ull vgl


static char OTM1283A_auo_param079[] = {0x00,0x90};             //Mode-3
static char OTM1283A_auo_param080[] = {0xf5,0x02,0x11,0x02,0x11};

static char OTM1283A_auo_param081[] = {0x00,0x90};             //2xV[] = NL, 1.5*=00, 2*=50, 3*=a0
static char OTM1283A_auo_param082[] = {0xc5,0x50};

static char OTM1283A_auo_param083[] = {0x00,0x94};             //Frequency
static char OTM1283A_auo_param084[] = {0xc5,0x66};


static char OTM1283A_auo_param085[] = {0x00,0x80};             //[] = anel timing state control
static char OTM1283A_auo_param086[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char OTM1283A_auo_param087[] = {0x00,0x90};             //[] = anel timing state control
static char OTM1283A_auo_param088[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xff,0x00,0xff,0x00};

static char OTM1283A_auo_param089[] = {0x00,0xa0};             //[] = anel timing state control
static char OTM1283A_auo_param090[] = {0xcb,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

static char OTM1283A_auo_param091[] = {0x00,0xb0};             //[] = anel timing state control
static char OTM1283A_auo_param092[] = {0xcb,0x00,0x00,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0xff,0x00,0x00,0x00,0x00,0x00};

static char OTM1283A_auo_param093[] = {0x00,0xc0};             //[] = anel timing state control
static char OTM1283A_auo_param094[] = {0xcb,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x00,0x05,0x05,0x05,0x05,0x05};

static char OTM1283A_auo_param095[] = {0x00,0xd0};             //[] = anel timing state control
static char OTM1283A_auo_param096[] = {0xcb,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05};

static char OTM1283A_auo_param097[] = {0x00,0xe0};             //[] = anel timing state control
static char OTM1283A_auo_param098[] = {0xcb,0x05,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00};

static char OTM1283A_auo_param099[] = {0x00,0xf0};             //[] = anel timing state control
static char OTM1283A_auo_param100[] = {0xcb,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff};


static char OTM1283A_auo_param101[] = {0x00,0x80};             //[] = anel [] = ad ma[] = [] = ing control
static char OTM1283A_auo_param102[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x07,0x00,0x0d,0x09,0x0f,0x0b,0x15};

static char OTM1283A_auo_param103[] = {0x00,0x90};             //[] = anel [] = ad ma[] = [] = ing control
static char OTM1283A_auo_param104[] = {0xcc,0x11,0x17,0x13,0x01,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x06};

static char OTM1283A_auo_param105[] = {0x00,0xa0};             //[] = anel [] = ad ma[] = [] = ing control
static char OTM1283A_auo_param106[] = {0xcc,0x08,0x00,0x0e,0x0a,0x10,0x0c,0x16,0x12,0x18,0x14,0x02,0x04,0x00,0x00};

static char OTM1283A_auo_param107[] = {0x00,0xb0};             //[] = anel [] = ad ma[] = [] = ing control
static char OTM1283A_auo_param108[] = {0xcc,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x04,0x02,0x00,0x0c,0x10,0x0a,0x0e,0x18};

static char OTM1283A_auo_param109[] = {0x00,0xc0};             //[] = anel [] = ad ma[] = [] = ing control
static char OTM1283A_auo_param110[] = {0xcc,0x14,0x16,0x12,0x08,0x06,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03};

static char OTM1283A_auo_param111[] = {0x00,0xd0};             //[] = anel [] = ad ma[] = [] = ing control
static char OTM1283A_auo_param112[] = {0xcc,0x01,0x00,0x0b,0x0f,0x09,0x0d,0x17,0x13,0x15,0x11,0x07,0x05,0x00,0x00};


static char OTM1283A_auo_param113[] = {0x00,0x80};             //[] = anel VST setting
static char OTM1283A_auo_param114[] = {0xce,0x87,0x03,0x28,0x86,0x03,0x28,0x85,0x03,0x28,0x84,0x03,0x28};

static char OTM1283A_auo_param115[] = {0x00,0x90};             //[] = anel VEND setting
static char OTM1283A_auo_param116[] = {0xce,0x34,0xfc,0x28,0x34,0xfd,0x28,0x34,0xfe,0x28,0x34,0xff,0x28,0x00,0x00};

static char OTM1283A_auo_param117[] = {0x00,0xa0};             //[] = anel CLKA1/2 setting
static char OTM1283A_auo_param118[] = {0xce,0x38,0x07,0x05,0x00,0x00,0x28,0x00,0x38,0x06,0x05,0x01,0x00,0x28,0x00};

static char OTM1283A_auo_param119[] = {0x00,0xb0};             //[] = anel CLKA3/4 setting
static char OTM1283A_auo_param120[] = {0xce,0x38,0x05,0x05,0x02,0x00,0x28,0x00,0x38,0x04,0x05,0x03,0x00,0x28,0x00};

static char OTM1283A_auo_param121[] = {0x00,0xc0};             //[] = anel CLKb1/2 setting
static char OTM1283A_auo_param122[] = {0xce,0x38,0x03,0x05,0x04,0x00,0x28,0x00,0x38,0x02,0x05,0x05,0x00,0x28,0x00};

static char OTM1283A_auo_param123[] = {0x00,0xd0};             //[] = anel CLKb3/4 setting
static char OTM1283A_auo_param124[] = {0xce,0x38,0x01,0x05,0x06,0x00,0x28,0x00,0x38,0x00,0x05,0x07,0x00,0x28,0x00};

static char OTM1283A_auo_param125[] = {0x00,0x80};             //[] = anel CLKc1/2 setting
static char OTM1283A_auo_param126[] = {0xcf,0x38,0x07,0x05,0x00,0x00,0x18,0x25,0x38,0x06,0x05,0x01,0x00,0x18,0x25};

static char OTM1283A_auo_param127[] = {0x00,0x90};             //[] = anel CLKc3/4 setting
static char OTM1283A_auo_param128[] = {0xcf,0x38,0x05,0x05,0x02,0x00,0x18,0x25,0x38,0x04,0x05,0x03,0x00,0x18,0x25};

static char OTM1283A_auo_param129[] = {0x00,0xa0};             //[] = anel CLKd1/2 setting
static char OTM1283A_auo_param130[] = {0xcf,0x38,0x03,0x05,0x04,0x00,0x18,0x25,0x38,0x02,0x05,0x05,0x00,0x18,0x25};

static char OTM1283A_auo_param131[] = {0x00,0xb0};             //[] = anel CLKd3/4 setting
static char OTM1283A_auo_param132[] = {0xcf,0x38,0x01,0x05,0x06,0x00,0x18,0x25,0x38,0x00,0x05,0x07,0x00,0x18,0x25};

static char OTM1283A_auo_param133[] = {0x00,0xc0};             //[] = anel ECLK setting
//static char OTM1283A_auo_param134[] = {0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x84,0x00,0x03,0x08}; //gate [] = re. ena.
static char OTM1283A_auo_param134[] = {0xcf,0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x81,0x00,0x03,0x08}; //gate [] = re. ena. //panmingdong

static char OTM1283A_auo_param135[] = {0x00,0x00};             
static char OTM1283A_auo_param136[] = {0xd8,0x84,0x84};         

static char OTM1283A_auo_param137[] = {0x00,0x00};             
static char OTM1283A_auo_param138[] = {0xd9,0x35};             //VCOM 0X1D 20140104

static char OTM1283A_auo_param139[] = {0x00,0x00}; 
static char OTM1283A_auo_param140[] = {0xE1,0x0A,0x15,0x1B,0x0E,0x06,0x10,0x0B,0x0A,0x04,0x07,0x0B,0x07,0x0F,0x11,0x0C,0x06};
                         
static char OTM1283A_auo_param141[] = {0x00,0x00}; 
static char OTM1283A_auo_param142[] = {0xE2,0x0A,0x15,0x1B,0x0E,0x06,0x11,0x0B,0x0A,0x04,0x07,0x0C,0x07,0x0F,0x11,0x0C,0x06}; 

static char OTM1283A_auo_param143[] = {0x00,0x00};             //Orise mode disable
static char OTM1283A_auo_param144[] = {0xff,0xff,0xff,0xff};

/******************color enhance************************************/ 
static char OTM1283A_auo_param201[] = {0x00,0xa0}; 
static char OTM1283A_auo_param202[] = {0xd6,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD}; 
static char OTM1283A_auo_param203[] = {0x00,0xB0}; 
static char OTM1283A_auo_param204[] = {0xd6,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD,0x01,0xCD}; 
static char OTM1283A_auo_param205[] = {0x00,0xC0}; 
static char OTM1283A_auo_param206[] = {0xD6,0x89,0x11,0x89,0x89,0x11,0x89,0x89,0x11,0x89,0x89,0x11,0x89}; 
static char OTM1283A_auo_param207[] = {0x00,0xD0}; 
static char OTM1283A_auo_param208[] = {0xD6,0x89,0x11,0x89,0x89,0x11,0x89}; 
static char OTM1283A_auo_param209[] = {0x00,0xE0}; 
static char OTM1283A_auo_param210[] = {0xD6,0x44,0x11,0x44,0x44,0x11,0x44,0x44,0x11,0x44,0x44,0x11,0x44}; 
static char OTM1283A_auo_param211[] = {0x00,0xF0}; 
static char OTM1283A_auo_param212[] = {0xD6,0x44,0x11,0x44,0x44,0x11,0x44}; 
static char OTM1283A_auo_param213[] = {0x00,0x90};                          //Clever CMD1 
static char OTM1283A_auo_param214[] = {0xd6,0x80};                                                                   
static char OTM1283A_auo_param215[] = {0x00,0x00};                          //CE - High                                               
static char OTM1283A_auo_param216[] = {0x55,0xB0};                                                                   
static char OTM1283A_auo_param217[] = {0x00,0x81};                          //SHARPNESS Level - 9                                     
static char OTM1283A_auo_param218[] = {0xD6,0x00,0xff};   
/********************************************************************/  

static struct dsi_cmd_desc new_otm1283_auo_lead_display_on_cmds[] =  	                      
{                                                                                   
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param001), OTM1283A_auo_param001},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param002), OTM1283A_auo_param002},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param003), OTM1283A_auo_param003},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param004), OTM1283A_auo_param004},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param005), OTM1283A_auo_param005},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param006), OTM1283A_auo_param006},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param007), OTM1283A_auo_param007},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param008), OTM1283A_auo_param008},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param009), OTM1283A_auo_param009},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param010), OTM1283A_auo_param010},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param011), OTM1283A_auo_param011},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param012), OTM1283A_auo_param012},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param013), OTM1283A_auo_param013},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param014), OTM1283A_auo_param014},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param015), OTM1283A_auo_param015},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param016), OTM1283A_auo_param016},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param017), OTM1283A_auo_param017},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param018), OTM1283A_auo_param018},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param019), OTM1283A_auo_param019},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param020), OTM1283A_auo_param020},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param021), OTM1283A_auo_param021},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param022), OTM1283A_auo_param022},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param023), OTM1283A_auo_param023},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param024), OTM1283A_auo_param024},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param025), OTM1283A_auo_param025},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param026), OTM1283A_auo_param026},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param027), OTM1283A_auo_param027},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param028), OTM1283A_auo_param028},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param029), OTM1283A_auo_param029},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param030), OTM1283A_auo_param030},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param031), OTM1283A_auo_param031},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param032), OTM1283A_auo_param032},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param033), OTM1283A_auo_param033},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param034), OTM1283A_auo_param034},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param035), OTM1283A_auo_param035},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param036), OTM1283A_auo_param036},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param037), OTM1283A_auo_param037},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param038), OTM1283A_auo_param038},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param039), OTM1283A_auo_param039},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param040), OTM1283A_auo_param040},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param041), OTM1283A_auo_param041},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param042), OTM1283A_auo_param042},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param043), OTM1283A_auo_param043},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param044), OTM1283A_auo_param044},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param045), OTM1283A_auo_param045},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param046), OTM1283A_auo_param046},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param047), OTM1283A_auo_param047},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param048), OTM1283A_auo_param048},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param049), OTM1283A_auo_param049},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param050), OTM1283A_auo_param050},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param051), OTM1283A_auo_param051},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param052), OTM1283A_auo_param052},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param053), OTM1283A_auo_param053},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param054), OTM1283A_auo_param054},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param055), OTM1283A_auo_param055},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param056), OTM1283A_auo_param056},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param057), OTM1283A_auo_param057},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param058), OTM1283A_auo_param058},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param059), OTM1283A_auo_param059},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param060), OTM1283A_auo_param060},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param061), OTM1283A_auo_param061},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param062), OTM1283A_auo_param062},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param063), OTM1283A_auo_param063},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param064), OTM1283A_auo_param064},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param065), OTM1283A_auo_param065},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param066), OTM1283A_auo_param066},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param067), OTM1283A_auo_param067},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param068), OTM1283A_auo_param068},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param069), OTM1283A_auo_param069},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param070), OTM1283A_auo_param070},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param071), OTM1283A_auo_param071},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param072), OTM1283A_auo_param072},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param073), OTM1283A_auo_param073},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param074), OTM1283A_auo_param074},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param075), OTM1283A_auo_param075},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param076), OTM1283A_auo_param076},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param077), OTM1283A_auo_param077},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param078), OTM1283A_auo_param078},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param079), OTM1283A_auo_param079},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param080), OTM1283A_auo_param080},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param081), OTM1283A_auo_param081},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param082), OTM1283A_auo_param082},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param083), OTM1283A_auo_param083},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param084), OTM1283A_auo_param084},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param085), OTM1283A_auo_param085},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param086), OTM1283A_auo_param086},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param087), OTM1283A_auo_param087},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param088), OTM1283A_auo_param088},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param089), OTM1283A_auo_param089},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param090), OTM1283A_auo_param090},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param091), OTM1283A_auo_param091},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param092), OTM1283A_auo_param092},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param093), OTM1283A_auo_param093},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param094), OTM1283A_auo_param094},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param095), OTM1283A_auo_param095},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param096), OTM1283A_auo_param096},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param097), OTM1283A_auo_param097},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param098), OTM1283A_auo_param098},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param099), OTM1283A_auo_param099},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param100), OTM1283A_auo_param100},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param101), OTM1283A_auo_param101},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param102), OTM1283A_auo_param102},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param103), OTM1283A_auo_param103},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param104), OTM1283A_auo_param104},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param105), OTM1283A_auo_param105},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param106), OTM1283A_auo_param106},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param107), OTM1283A_auo_param107},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param108), OTM1283A_auo_param108},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param109), OTM1283A_auo_param109},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param110), OTM1283A_auo_param110},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param111), OTM1283A_auo_param111},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param112), OTM1283A_auo_param112},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param113), OTM1283A_auo_param113},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param114), OTM1283A_auo_param114},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param115), OTM1283A_auo_param115},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param116), OTM1283A_auo_param116},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param117), OTM1283A_auo_param117},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param118), OTM1283A_auo_param118},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param119), OTM1283A_auo_param119},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param120), OTM1283A_auo_param120},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param121), OTM1283A_auo_param121},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param122), OTM1283A_auo_param122},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param123), OTM1283A_auo_param123},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param124), OTM1283A_auo_param124},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param125), OTM1283A_auo_param125},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param126), OTM1283A_auo_param126},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param127), OTM1283A_auo_param127},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param128), OTM1283A_auo_param128},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param129), OTM1283A_auo_param129},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param130), OTM1283A_auo_param130},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param131), OTM1283A_auo_param131},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param132), OTM1283A_auo_param132},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param133), OTM1283A_auo_param133},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param134), OTM1283A_auo_param134},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param135), OTM1283A_auo_param135},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param136), OTM1283A_auo_param136},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param137), OTM1283A_auo_param137},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param138), OTM1283A_auo_param138},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param139), OTM1283A_auo_param139},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param140), OTM1283A_auo_param140},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param141), OTM1283A_auo_param141},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param142), OTM1283A_auo_param142},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param143), OTM1283A_auo_param143},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param144), OTM1283A_auo_param144},
	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param201), OTM1283A_auo_param201},//similar with boe if disable CE
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param202), OTM1283A_auo_param202},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param203), OTM1283A_auo_param203},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param204), OTM1283A_auo_param204},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param205), OTM1283A_auo_param205},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param206), OTM1283A_auo_param206},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param207), OTM1283A_auo_param207},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param208), OTM1283A_auo_param208},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param209), OTM1283A_auo_param209},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param210), OTM1283A_auo_param210},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param211), OTM1283A_auo_param211},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param212), OTM1283A_auo_param212},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param213), OTM1283A_auo_param213},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param214), OTM1283A_auo_param214},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param215), OTM1283A_auo_param215},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param216), OTM1283A_auo_param216},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param217), OTM1283A_auo_param217},
	{DTYPE_DCS_LWRITE, 1, 0, 0, 0, sizeof(OTM1283A_auo_param218), OTM1283A_auo_param218},

	
	{DTYPE_DCS_WRITE, 1, 0, 0, 120, sizeof(exit_sleep), exit_sleep},                 
	{DTYPE_DCS_WRITE, 1, 0, 0, 50, sizeof(display_on), display_on},                  
};
/***********************************************************************************************/
static char display_off[]={0x28,0x00};
static char enter_sleep[]={0x10,0x00};
static struct dsi_cmd_desc display_off_cmds[] = {
	{DTYPE_DCS_WRITE, 1, 0, 0, 30,
		sizeof(display_off), display_off},
	{DTYPE_DCS_WRITE, 1, 0, 0, 120,
		sizeof(enter_sleep), enter_sleep}
};


#define HX8394_IC_ID	0X94
#define IC_ID			5

#define PM8921_GPIO_BASE		NR_GPIO_IRQS
#define PM8921_GPIO_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_GPIO_BASE)
#define PM8921_MPP_BASE			(PM8921_GPIO_BASE + PM8921_NR_GPIOS)
#define PM8921_MPP_PM_TO_SYS(pm_gpio)	(pm_gpio - 1 + PM8921_MPP_BASE)
#define PM8921_IRQ_BASE			(NR_MSM_IRQS + NR_GPIO_IRQS)

extern u32 LcdPanleID ;

struct mipi_manufacture_ic {
	struct dsi_cmd_desc *readid_tx;
	int readid_len_tx;
	struct dsi_cmd_desc *readid_rx;
	int readid_len_rx;
	int mode;
};

static char hx8394_setpassword_para[4]={0xB9,0xFF,0x83,0x94};
static struct dsi_cmd_desc hx8394_setpassword_cmd[] = 
{	
	{DTYPE_DCS_LWRITE, 1, 0, 0, 1, sizeof(hx8394_setpassword_para),hx8394_setpassword_para},

};

static char hx8394_rd_ic_id_para[2] = {0xf4, 0x00}; /* DTYPE_DCS_READ */
static struct dsi_cmd_desc hx8394_rd_ic_id_cmd = {
	DTYPE_DCS_READ, 1, 0, 0, 1, sizeof(hx8394_rd_ic_id_para), hx8394_rd_ic_id_para};
	

static uint32 mipi_get_manufacture_id(struct msm_fb_data_type *mfd)
{
	struct dsi_buf *rp, *tp;
	//struct dsi_cmd_desc *cmd;
	uint32 *lp,i,id;
	
	 struct mipi_manufacture_ic mipi_manufacture_icid[1] = 
	 {	 	
		{hx8394_setpassword_cmd,ARRAY_SIZE(hx8394_setpassword_cmd),&hx8394_rd_ic_id_cmd,2,1},
	 };

	 for(i = 0; i < ARRAY_SIZE(mipi_manufacture_icid) ; i++)	
	 {
		tp = &novatek_tx_buf;
		rp = &novatek_rx_buf;
		
		lcd_panle_reset();
		mipi_set_tx_power_mode(1);		
		//cmd = &hx8394_rd_ic_id_cmd;
		if(mipi_manufacture_icid[i].readid_tx)
			mipi_dsi_cmds_tx(&novatek_tx_buf, mipi_manufacture_icid[i].readid_tx,mipi_manufacture_icid[i].readid_len_tx);
		
		if(!mipi_manufacture_icid[i].mode)
			mipi_set_tx_power_mode(0);	
		
		mipi_dsi_cmd_bta_sw_trigger(); 
		
		mipi_dsi_cmds_rx(mfd, tp, rp,  mipi_manufacture_icid[i].readid_rx, mipi_manufacture_icid[i].readid_len_rx);

		//if(mipi_manufacture_icid[i].mode)
		//	mipi_set_tx_power_mode(0);
				
		lp = (uint32 *)rp->data;
		//pr_info("%s: manufacture_id=%x\n", __func__, *lp);
		id = *lp;
		printk("\n lcd panel read id value =%x",id);
#if 0//not define panel
		if ((id & 0xff )== HX8394_IC_ID)		
			return LCD_PANEL_5P0_HX8394_BOE_BOE;
		else
			return LCD_PANEL_5P0_NT35590_CMI_CMI;
#endif
	 }
	 
	 return 0;
	 
}


#define GPIO_LCD_RESET 58
static void lcd_panle_reset(void)
{	
	
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(10);
	gpio_direction_output(GPIO_LCD_RESET,0);
	msleep(20);
	gpio_direction_output(GPIO_LCD_RESET,1);
	msleep(50);

}



static int first_time_panel_on = 1;

static int mipi_novatek_lcd_on(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct msm_panel_info *pinfo;
	struct dcs_cmd_req cmdreq;
	int ret = 0;
	
	mfd = platform_get_drvdata(pdev);
	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	pinfo = &mfd->panel_info;

	mipi  = &mfd->panel_info.mipi;	
	
	printk("\n LCD panle on,LcdPanleID = %d\n",LcdPanleID);
	
	if (first_time_panel_on)
	{		
		first_time_panel_on = 0;
						
		ret=gpio_tlmm_config(GPIO_CFG(1, 0,
       				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
       				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       	if (ret) 
		{
       		printk("LCD Could not configure lcd id gpio 1 to input \n" );
       	}
		ret=gpio_tlmm_config(GPIO_CFG(3, 0,
       				GPIO_CFG_INPUT, GPIO_CFG_NO_PULL,
       				GPIO_CFG_2MA), GPIO_CFG_ENABLE);
       	if (ret) 
		{
       		printk("LCD Could not configure lcd id gpio 3 to input \n" );
       	}
		
		printk("\n LCD first time on ---- return !\n");
		
		return 0;   // should not read ID 
		
		if( LcdPanleID == LCD_PANEL_NOPANEL)
			LcdPanleID = mipi_get_manufacture_id(mfd);
		else		
			return 0;   
	}
	
	lcd_panle_reset();
	
	if (mipi->mode == DSI_VIDEO_MODE) 
	{	
		switch(LcdPanleID) 
		{
			
			case LCD_PANEL_5P7_OTM1283A_AUO_LEAD:
				cmdreq.cmds = new_otm1283_auo_lead_display_on_cmds;
				cmdreq.cmds_cnt = ARRAY_SIZE(new_otm1283_auo_lead_display_on_cmds);
				cmdreq.flags = CMD_REQ_COMMIT;
				cmdreq.rlen = 0;
				cmdreq.cb = NULL;
				mipi_dsi_cmdlist_put(&cmdreq);
				printk("\n LCD_PANEL_5P7_OTM1283A_AUO_LEAD\n");
				return 0;
				
				mipi_dsi_cmds_tx(&novatek_tx_buf, new_otm1283_auo_lead_display_on_cmds,
				ARRAY_SIZE(new_otm1283_auo_lead_display_on_cmds));
				printk("\n LCD_PANEL_5P7_OTM1283A_AUO_LEAD\n");
				break;
			case LCD_PANEL_5P7_OTM1283A_BOE_BOE:
				cmdreq.cmds = boe_otm1283a_display_on_cmds;
				cmdreq.cmds_cnt = ARRAY_SIZE(boe_otm1283a_display_on_cmds);
				cmdreq.flags = CMD_REQ_COMMIT;
				cmdreq.rlen = 0;
				cmdreq.cb = NULL;
				mipi_dsi_cmdlist_put(&cmdreq);
				printk("\n LCD_PANEL_5P7_OTM1283A_BOE_BOE\n");
				return 0;
				
				mipi_dsi_cmds_tx(&novatek_tx_buf, boe_otm1283a_display_on_cmds,
				ARRAY_SIZE(boe_otm1283a_display_on_cmds));
				printk("\n LCD_PANEL_5P7_OTM1283A_BOE_BOE\n");
				break;
			case LCD_PANEL_5P7_OTM1283A_CPT:
				cmdreq.cmds = cpt_otm1283a_display_on_cmds;
				cmdreq.cmds_cnt = ARRAY_SIZE(cpt_otm1283a_display_on_cmds);
				cmdreq.flags = CMD_REQ_COMMIT;
				cmdreq.rlen = 0;
				cmdreq.cb = NULL;
				mipi_dsi_cmdlist_put(&cmdreq);
				printk("\n LCD_PANEL_5P7_OTM1283A_CPT(YASSY)\n");
				return 0;
				
				mipi_dsi_cmds_tx(&novatek_tx_buf, cpt_otm1283a_display_on_cmds,
				ARRAY_SIZE(cpt_otm1283a_display_on_cmds));
				printk("\n LCD_PANEL_5P7_OTM1283A_CPT\n");
				break;				
			default:
				printk("\n LCD Error, No panel initialize");
				break;

		}
		
	} 
	else 
	{
		printk("\n LCD Error, CMD MODE");
	}

	return 0;
}

static int mipi_novatek_lcd_off(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;

	mfd = platform_get_drvdata(pdev);

	if (!mfd)
		return -ENODEV;
	if (mfd->key != MFD_KEY)
		return -EINVAL;

	mipi_dsi_cmds_tx(&novatek_tx_buf, display_off_cmds,
			ARRAY_SIZE(display_off_cmds));
	printk("\n LCD panel off \n");
	return 0;
}


struct dcs_cmd_req cmdreq;


static bool onewiremode = false;
static int lcd_bkl_ctl=2;//
void myudelay(unsigned int usec)
{
	udelay(usec);
}
static void select_1wire_mode(void)
{
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(120);
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(280);				////ZTE_LCD_LUYA_20100226_001
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(650);				////ZTE_LCD_LUYA_20100226_001
	
}


static void send_bkl_address(void)
{
	unsigned int i,j;
	i = 0x72;
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	printk("[LY] send_bkl_address \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}

static void send_bkl_data(int level)
{
	unsigned int i,j;
	i = level & 0x1F;
	gpio_direction_output(lcd_bkl_ctl, 1);
	myudelay(10);
	printk("[LY] send_bkl_data \n");
	for(j = 0; j < 8; j++)
	{
		if(i & 0x80)
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(10);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(180);
		}
		else
		{
			gpio_direction_output(lcd_bkl_ctl, 0);
			myudelay(180);
			gpio_direction_output(lcd_bkl_ctl, 1);
			myudelay(10);
		}
		i <<= 1;
	}
	gpio_direction_output(lcd_bkl_ctl, 0);
	myudelay(10);
	gpio_direction_output(lcd_bkl_ctl, 1);

}
static void mipi_zte_set_backlight(struct msm_fb_data_type *mfd)
{
       /*value range is 1--32*/
	 int current_lel = mfd->bl_level;
  	 unsigned long flags;


    	printk("[ZYF] lcdc_set_bl level=%d, %d\n", 
		   current_lel , mfd->panel_power_on);

    	if(!mfd->panel_power_on)
	{
    		gpio_direction_output(lcd_bkl_ctl, 0);
    		onewiremode = FALSE;
	    	return;
    	}

    	if(current_lel < 1)
    	{
        	current_lel = 0;
   	 }

    	if(current_lel > 32)
    	{
        	current_lel = 32;
    	}

    	local_irq_save(flags);
		
   	if(current_lel==0)
    	{
    		gpio_direction_output(lcd_bkl_ctl, 0);
		mdelay(3);
		onewiremode = FALSE;
			
    	}
    	else 
	{
		if(!onewiremode)	
		{
			printk("[LY] before select_1wire_mode\n");
			select_1wire_mode();
			onewiremode = TRUE;
		}
		send_bkl_address();
		send_bkl_data(current_lel-1);

	}
		
    	local_irq_restore(flags);
}



static int __devinit mipi_novatek_lcd_probe(struct platform_device *pdev)
{
	struct msm_fb_data_type *mfd;
	struct mipi_panel_info *mipi;
	struct platform_device *current_pdev;
	//static struct mipi_dsi_phy_ctrl *phy_settings;
	static char dlane_swap;

	if (pdev->id == 0) 
	{
		
		mipi_novatek_pdata = pdev->dev.platform_data;
		
		
		//mipi_get_manufacture_id(mfd);
/*		
		if (mipi_novatek_pdata&& mipi_novatek_pdata->phy_ctrl_settings) 
		{
			phy_settings = (mipi_novatek_pdata->phy_ctrl_settings);
		}

		if (mipi_novatek_pdata&& mipi_novatek_pdata->dlane_swap) 
		{
			dlane_swap = (mipi_novatek_pdata->dlane_swap);
		}
*/
		return 0;
	}

	current_pdev = msm_fb_add_device(pdev);

	if (current_pdev) 
	{
		mfd = platform_get_drvdata(current_pdev);
		if (!mfd)
			return -ENODEV;
		if (mfd->key != MFD_KEY)
			return -EINVAL;

		mipi  = &mfd->panel_info.mipi;
#if 0
		if (phy_settings != NULL)
		{
			mipi->dsi_phy_db = &phy_settings;
			mipi->dsi_phy_db_count = 1;
		}
#endif
		if (dlane_swap)
			mipi->dlane_swap = dlane_swap;
	}
	return 0;
}
/*
static void mipi_lcd_shutdown(struct platform_device *pdev)
{
	lcd_panle_reset();	
	mipi_dsi_cmds_tx(&novatek_tx_buf, display_off_cmds,
			ARRAY_SIZE(display_off_cmds));
	printk("LCD panel off before restart \n");
}
*/
static struct platform_driver this_driver = {
	.probe  = mipi_novatek_lcd_probe,
	.driver = {
		.name   = "mipi_novatek",
	},
	//.shutdown = mipi_lcd_shutdown,
};

static struct msm_fb_panel_data novatek_panel_data = {
	.on		= mipi_novatek_lcd_on,
	.off		= mipi_novatek_lcd_off,
	.set_backlight = mipi_zte_set_backlight,//mipi_novatek_set_backlight,
};


static int ch_used[3];

int mipi_novatek_device_register(struct msm_panel_info *pinfo,
					u32 channel, u32 panel)
{
	struct platform_device *pdev = NULL;
	int ret;

	if ((channel >= 3) || ch_used[channel])
		return -ENODEV;

	ch_used[channel] = TRUE;

	ret = mipi_novatek_lcd_init();
	if (ret) {
		pr_err("mipi_novatek_lcd_init() failed with ret %u\n", ret);
		return ret;
	}

	pdev = platform_device_alloc("mipi_novatek", (panel << 8)|channel);
	if (!pdev)
		return -ENOMEM;

	novatek_panel_data.panel_info = *pinfo;

	ret = platform_device_add_data(pdev, &novatek_panel_data,
		sizeof(novatek_panel_data));
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_add_data failed!\n", __func__);
		goto err_device_put;
	}

	ret = platform_device_add(pdev);
	if (ret) {
		printk(KERN_ERR
		  "%s: platform_device_register failed!\n", __func__);
		goto err_device_put;
	}

	return 0;

err_device_put:
	platform_device_put(pdev);
	return ret;
}

static int mipi_novatek_lcd_init(void)
{

	mipi_dsi_buf_alloc(&novatek_tx_buf, DSI_BUF_SIZE);
	mipi_dsi_buf_alloc(&novatek_rx_buf, DSI_BUF_SIZE);

	return platform_driver_register(&this_driver);
}
