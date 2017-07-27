#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "driver/gpio.h"
#include "driver/i2c.h"
#include "driver/i2s.h"
#include "wm8978.h"
#include "iot_debug.h"

#define I2C_MASTER_SCL_IO    		19    	/*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO    		18    	/*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM 				I2C_NUM_1 /*!< I2C port number for master dev */
#define I2C_MASTER_TX_BUF_DISABLE   0   	/*!< I2C master do not need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0   	/*!< I2C master do not need buffer */
#define I2C_MASTER_FREQ_HZ    		100000  /*!< I2C master clock frequency */

#define ESP_SLAVE_ADDR      		0x1A    /*!< ESP32 slave address, you can set any 7bit value */
#define WRITE_BIT           		I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT            		I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN   				0x1     /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS  				0x0     /*!< I2C master will not check ack from slave */
#define ACK_VAL    					0x0     /*!< I2C ack value */
#define NACK_VAL   					0x1     /*!< I2C nack value */


esp_err_t i2c_master_read_slave(i2c_port_t i2c_num, uint8_t* data_rd, size_t size)
{
    if (size == 0) {
        return ESP_OK;
    }
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | READ_BIT, ACK_CHECK_EN);
    if (size > 1) {
        i2c_master_read(cmd, data_rd, size - 1, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_rd + size - 1, NACK_VAL);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

esp_err_t i2c_master_write_slave(i2c_port_t i2c_num, uint8_t* data_wr, size_t size)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, ( ESP_SLAVE_ADDR << 1 ) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write(cmd, data_wr, size, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

void i2c_master_init()
{
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

static uint16_t WM8978_REGVAL[58]=
{
	0x0000,0x0000,0x0000,0x0000,0x0050,0x0000,0x0140,0x0000,
	0x0000,0x0000,0x0000,0x00FF,0x00FF,0x0000,0x0100,0x00FF,
	0x00FF,0x0000,0x012C,0x002C,0x002C,0x002C,0x002C,0x0000,
	0x0032,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,
	0x0038,0x000B,0x0032,0x0000,0x0008,0x000C,0x0093,0x00E9,
	0x0000,0x0000,0x0000,0x0000,0x0003,0x0010,0x0010,0x0100,
	0x0100,0x0002,0x0001,0x0001,0x0039,0x0039,0x0039,0x0039,
	0x0001,0x0001
};

esp_err_t wm_write_register(uint8_t reg, uint16_t val)
{
	esp_err_t ret;
	uint8_t writeBuff[2];
	writeBuff[0]=( reg << 1 )|( (uint8_t)( ( val>>8 ) & 0x01 ) );
	writeBuff[1]=(uint8_t)val;
    ret = i2c_master_write_slave(I2C_MASTER_NUM, writeBuff, 2);
	if(ret == 0){
		WM8978_REGVAL[reg]=val;
    }
	return ret;
}

uint16_t wm_read_register(uint8_t reg)
{  
	return WM8978_REGVAL[reg];	
}


void WM8978_ADDA_Cfg(uint8_t dacen,uint8_t adcen)
{
	uint16_t regval;
	regval=wm_read_register(3);						
	if(dacen){
		regval|=3<<0;										
    }else{
		regval&=~(3<<0);
    }								
	wm_write_register(3,regval);							
	regval=wm_read_register(2);							
	if(adcen){
		regval|=3<<0;			        				
    }else{ 
		regval&=~(3<<0);
    }				  				
	wm_write_register(2,regval);							
}

void WM8978_Input_Cfg(uint8_t micen,uint8_t lineinen,uint8_t auxen)
{
	uint16_t regval;  
	regval=wm_read_register(2);							
	if(micen){
		regval|=3<<2;									
    }else{ 
		regval&=~(3<<2);
    }									
 	wm_write_register(2,regval);							
	regval=wm_read_register(44);							
	if(micen){
		regval|=3<<4|3<<0;										
    }else {
		regval&=~(3<<4|3<<0);	
    }								
	wm_write_register(44,regval);						
	if(lineinen){
		WM8978_LINEIN_Gain(5);								
    }else{ 
		WM8978_LINEIN_Gain(0);	
    }							
	if(auxen){
		WM8978_AUX_Gain(7);									
    }else{
		WM8978_AUX_Gain(0);	
    }									
}

void WM8978_MIC_Gain(uint8_t gain)
{
	gain&=0X3F;
	wm_write_register(45,gain);						 
	wm_write_register(46,gain|1<<8);					
}

void WM8978_LINEIN_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=wm_read_register(47);							
	regval&=~(7<<4);									
 	wm_write_register(47,regval|gain<<4);		
	regval=wm_read_register(48);							
	regval&=~(7<<4);											
 	wm_write_register(48,regval|gain<<4);		
} 

void WM8978_AUX_Gain(uint8_t gain)
{
	uint16_t regval;
	gain&=0X07;
	regval=wm_read_register(47);							
	regval&=~(7<<0);												
 	wm_write_register(47,regval|gain<<0);		
	regval=wm_read_register(48);					
	regval&=~(7<<0);							
 	wm_write_register(48,regval|gain<<0);		
}  


void WM8978_Output_Cfg(uint8_t dacen,uint8_t bpsen)
{
	uint16_t regval=0;
	if(dacen){
		regval|=1<<0;
    }													
	if(bpsen){
		regval|=1<<1;													
		regval|=5<<2;													
	} 
	wm_write_register(50,regval);						
	wm_write_register(51,regval);						
}

void WM8978_HPvol_Set(uint8_t voll,uint8_t volr)
{
	voll&=0X3F;
	volr&=0X3F;															
	if(voll==0)voll|=1<<6;									
	if(volr==0)volr|=1<<6;									
	wm_write_register(52,voll);							
	wm_write_register(53,volr|(1<<8));				
}

void WM8978_SPKvol_Set(uint8_t volx)
{
	volx&=0X3F;
	if(volx==0)volx|=1<<6;									
 	wm_write_register(54,volx);							
	wm_write_register(55,volx|(1<<8));				
}

void WM8978_I2S_Cfg(uint8_t fmt,uint8_t len)
{
	fmt&=0x03;
	len&=0x03;															
	wm_write_register(4,(fmt<<3)|(len<<5));
}


int WM8978_Init(void)
{
    i2c_master_init();

	wm_write_register(0,0);														
	wm_write_register(1,0x1B);		
	wm_write_register(2,0x1B0);						
	wm_write_register(3,0x6C);							
	wm_write_register(6,0);							
	wm_write_register(43,1<<4);						
	wm_write_register(47,1<<8);						
	wm_write_register(48,1<<8);						
	wm_write_register(49,1<<1);						
	wm_write_register(10,1<<3);							
	wm_write_register(14,1<<3);							
	WM8978_I2S_Cfg(2,0);
	WM8978_ADDA_Cfg(1,0);								
	WM8978_Input_Cfg(0,0,0);
	WM8978_Output_Cfg(1,0);
	WM8978_HPvol_Set(30,30);
	WM8978_SPKvol_Set(30);

	return 0;
}

// #define WM8978_REG00  0
// #define WM8978_REG01  1 
// #define WM8978_REG02  2 
// #define WM8978_REG03  3
// #define WM8978_REG04  4
// #define WM8978_REG06  6
// #define WM8978_REG14  14
// #define WM8978_REG10  10
// #define WM8978_REG43  43
// #define WM8978_REG47  47
// #define WM8978_REG48  48
// #define WM8978_REG49  49

// typedef struct {_
// 	unsigned int reg_addr;
// 	unsigned int reg_val;
// } wm8978_cmd_entry_t;

// wm8978_cmd_entry_t wm8978_cmd_entries[] = {
// 	{ WM8978_REG00,		0x00 },
// 	{ WM8978_REG01,		0x1B },
// 	{ WM8978_REG02,		0x1B0},
// 	{ WM8978_REG03,		0x6F },
// 	{ WM8978_REG06,		0x00 },
// 	{ WM8978_REG43,		0x10 },
// 	{ WM8978_REG47,		0x100},
// 	{ WM8978_REG48, 	0x100},
// 	{ WM8978_REG49, 	0x02 },
// 	{ WM8978_REG10, 	0x08 },
// 	{ WM8978_REG14, 	0x08 },
// 	{ WM8978_REG04, 	0x18 },
// };

// esp_err_t wm8978_config(i2c_port_t i2c_num, wm8978_cmd_entry_t* cmd_entries, unsigned int entries_count, unsigned int relaxation_time)
// {
// 	i2c_cmd_handle_t cmd;
// 	int ret;
// 	uint8_t writeBuff[2];

// 	for (uint8_t i = 0; i < entries_count; i++){
// 		writeBuff[0] = (cmd_entries[i].reg_addr << 1) | (cmd_entries[i].reg_val >> 8);
// 		writeBuff[1] = cmd_entries[i].reg_val & 0xff;

// 		ret = i2c_master_write_slave(I2C_MASTER_NUM, writeBuff, 2);
// 		if(ret==0){
// 			WM8978_REGVAL[reg]=val;
// 		}
// 		vTaskDelay(relaxation_time / portTICK_RATE_MS);
// 	}

// 	return ESP_OK;
// }

