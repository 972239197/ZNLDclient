#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "SHT1x.h"
#include "gpio.h"

#define SHT11_CMD_STATUS_REG_W 6
#define SHT11_CMD_STATUS_REG_R 7
#define SHT11_CMD_MEASURE_TEMP 3
#define SHT11_CMD_MEASURE_HUMI 5
#define SHT11_CMD_RESET        15

#define SHT11_C1 -4.0		/* for 12 Bit */
#define SHT11_C2  0.0405	/* for 12 Bit */
#define SHT11_C3 -0.0000028	/* for 12 Bit */
#define SHT11_T1  0.01		/* for 14 Bit @ 5V */
#define SHT11_T2  0.00008	/* for 14 Bit @ 5V */



#define set_sck_high()   gpio_write( 41, 1 )
#define set_sck_low() 	 gpio_write( 41, 0 )
#define sht1x_sck_out()  gpio_set_direction(41, GPIO_OUTPUT)

#define set_data_high()  gpio_write( 42, 1 )
#define set_data_low() 	 gpio_write( 42, 0 )
#define read_data_pin()  gpio_read( 42 )
#define sht1x_data_in()	 gpio_set_direction(42, GPIO_INPUT)
#define sht1x_data_out()	gpio_set_direction(42, GPIO_OUTPUT)

sht11_t sht1x_dev;


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------


void sck_delay(void)
{
	usleep(1000);//osDelay(1)  try try  try????????????
}


u8 wait_until_data_is_ready(void)
{
	/* And if nothing came back this code hangs here */
	sht1x_data_in();
	while(read_data_pin())
		sck_delay();

	sht1x_data_out();
	return(0);	
}

/*
   sensirion has implemented the CRC the wrong way round. We
   need to swap everything.
   bit-swap a byte (bit7->bit0, bit6->bit1 ...)
   code provided by Guido Socher http://www.tuxgraphics.org/
 */
u8 bitswapbyte(u8 byte)
{
	u8 i=8;
	u8 result=0;
	while(i) {
		result=(result<<1);

		if (1 & byte) {
			result=result | 1;
		}

		i--;
		byte=(byte>>1);
	}

	return(result);
}

/* Code from avr-libc http://www.nongnu.org/avr-libc/ */
static u8 _crc_ibutton_update(u8 crc, u8 data)
{
	u8 i;

	crc = crc ^ data;
	for (i = 0; i < 8; i++)
	{
		if (crc & 0x01)
			crc = (crc >> 1) ^ 0x8C;
		else
			crc >>= 1;
	}

	return crc;
}
u8 sht11_crc8(u8 crc, u8 data)
{
	return(_crc_ibutton_update(crc, bitswapbyte(data)));
}

int sht11_io_init(void)
{
	/* sht11 clk pin to output and set high */
	sht1x_data_out();
	sht1x_sck_out();
	
	set_data_low();
	set_sck_high();
	return(0); /* for compatibility reason */
}


//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------
//-----------------------------------------------------------------------



void send_byte(u8 byte)
{
	u8 i;

	i = 8;

	while (i) {
		--i;

		if (byte & (1<<i))
			set_data_high();
		else
			set_data_low();

		sck_delay();
		set_sck_high();
		sck_delay();
		set_sck_low();
	}
}

u8 read_byte(void)
{
	u8 i, result;

	result = 0;
	i = 8;
	
	sht1x_data_in();

	while (i) {
		--i;
		sck_delay();
		set_sck_high();				

		if (read_data_pin())
			result |= (1<<i);

		sck_delay();
		set_sck_low();

	}
	sht1x_data_out();

	return (result);
}

void send_ack(void)
{
	/* Send ack */
	set_data_low();
	sck_delay();
	set_sck_high();
	sck_delay();
	set_sck_low();
}

u8 read_ack(void)
{
	u8 ack;

	/* read ack after command */
	sck_delay();
	set_sck_high();
	
	sht1x_data_in();	
	ack = read_data_pin();
	
	sck_delay();
	set_sck_low();

	sht1x_data_out();

	return (ack);
}

/* terminate a session without sending an ack */
void terminate_no_ack(void)
{
	set_data_high();
	sck_delay();
	set_sck_high();
	sck_delay();
	set_sck_low();
}

void send_start_command(void)
{
	/* DATA:   _____           ________
	   DATA:         |_______|
	   SCK :       ___       ___
	   SCK :  ___|     |___|     |______
	 */

	set_data_high();
	/*   set_data_out (); */
	sck_delay();
	set_sck_high();
	sck_delay();
	set_data_low();
	sck_delay();
	set_sck_low();
	sck_delay();
	set_sck_high();
	sck_delay();
	set_data_high();
	sck_delay();
	set_sck_low();
	sck_delay();
}

void sht11_read_status_reg(sht11_t *sht11)
{
	u8 ack;

	sht11->cmd = 7; /* read status reg cmd */
	sht11->status_reg_crc8 = 0;
	sht11->status_reg_crc8c = 0;
	send_start_command();
	send_byte(sht11->cmd);
	ack = read_ack();
	sht11->status_reg_crc8c = sht11_crc8(sht11->status_reg_crc8c, sht11->cmd);

	if (!ack) {
		sht11->status_reg = read_byte();
		sht11->status_reg_crc8c = sht11_crc8(sht11->status_reg_crc8c, sht11->status_reg);
		send_ack();
		sht11->status_reg_crc8 = read_byte();
		terminate_no_ack();
	}
}

/* Disable Interrupt to avoid possible clk problem. */
void send_cmd(sht11_t *sht11)
{
	u8 ack, byte;

	/* safety 000xxxxx */
	sht11->cmd &= 31;
	sht11->result = 0;
	sht11->crc8 = 0;
	sht11->crc8c = 0;

	send_start_command();
	send_byte(sht11->cmd);
	ack = read_ack();
	sht11->crc8c = sht11_crc8(sht11->crc8c, sht11->cmd);

	if (!ack) {
		/* And if nothing came back this code hangs here */
		wait_until_data_is_ready();

		/* inizio la lettura dal MSB del primo byte */
		byte = read_byte();
		sht11->result = byte << 8;
		sht11->crc8c = sht11_crc8(sht11->crc8c, byte);

		/* Send ack */
		send_ack();

		/* inizio la lettura dal MSB del secondo byte */
		byte = read_byte();
		sht11->result |= byte;
		sht11->crc8c = sht11_crc8(sht11->crc8c, byte);

		send_ack();

		/* inizio la lettura del CRC-8 */
		sht11->crc8 = read_byte();
		terminate_no_ack();
	}
}

void sht11_dewpoint(sht11_t *sht11)
{
	double k;
	k = (log10(sht11->humidity_compensated) - 2) / 0.4343 +
	    (17.62 * sht11->temperature) / (243.12 + sht11->temperature);
	sht11->dewpoint = 243.12 * k / (17.62 - k);
}

void sht11_read_temperature(sht11_t *sht11)
{
	sht11->cmd = SHT11_CMD_MEASURE_TEMP;
	send_cmd(sht11);
	sht11->raw_temperature = sht11->result;
	sht11->raw_temperature_crc8 = sht11->crc8;
	sht11->raw_temperature_crc8c = sht11->crc8c;
	sht11->temperature = SHT11_T1 * sht11->raw_temperature - 40;
}

void sht11_read_humidity(sht11_t *sht11)
{
	sht11->cmd = SHT11_CMD_MEASURE_HUMI;
	send_cmd(sht11);
	sht11->raw_humidity = sht11->result;
	sht11->raw_humidity_crc8 = sht11->crc8;
	sht11->raw_humidity_crc8c = sht11->crc8c;
	sht11->humidity_linear = SHT11_C1 +
	    (SHT11_C2 * sht11->raw_humidity) +
	    (SHT11_C3 * sht11->raw_humidity * sht11->raw_humidity);

	/* Compensate humidity result with temperature */
	sht11->humidity_compensated = (sht11->temperature - 25) *
	    (SHT11_T1 + SHT11_T2 * sht11->raw_humidity) +
	    sht11->humidity_linear;

	/* Range adjustment */
	if (sht11->humidity_compensated > 100)
		sht11->humidity_compensated = 100;
	if (sht11->humidity_compensated < 0.1)
		sht11->humidity_compensated = 0.1;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// High Level Functions
void sht11_init( sht11_t *sht11)
{
	sht11_io_init();
	sht11_read_status_reg(sht11);
}


void sht11_read_all(sht11_t *sht11)
{
	sht11_read_temperature(sht11);
	sht11_read_humidity(sht11);
	sht11_dewpoint(sht11);//????????????????????????
}



u8 GetSht1xTemp(void)
{
	return (u8)sht1x_dev.temperature;
}


u8 GetSht1xHumi(void)
{
	return (u8)sht1x_dev.humidity_linear;
}

u8 Sht1xMeasure(void)
{	
	sht11_init(&sht1x_dev);
 	sht11_read_all(&sht1x_dev);

	return 1;

}







