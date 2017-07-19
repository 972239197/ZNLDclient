#ifndef _SHT1X__H_
#define _SHT1X__H
	
#define u8 unsigned char
#define u16 unsigned short int	

typedef struct 
{
	u16 raw_temperature;
	u8 raw_temperature_crc8; /* read */
	u8 raw_temperature_crc8c; /* calculated */
	u16 raw_humidity;
	u8 raw_humidity_crc8;
	u8 raw_humidity_crc8c;
	u8 status_reg;
	u8 status_reg_crc8;
	u8 status_reg_crc8c;
	double temperature;
	double humidity_linear;
	double humidity_compensated;
	double dewpoint;
	u8 cmd; /* command to send */
	u16 result; /* result of the command */
	u8 crc8; /* crc8 returned */
	u8 crc8c; /* crc8 calculated */
	
}sht11_t;


extern void sht11_init(sht11_t *sht11);
extern void sht11_read_all(sht11_t *sht11);

extern u8 Sht1xMeasure(void);
extern u8 GetSht1xTemp(void);
extern u8 GetSht1xHumi(void);


#endif


