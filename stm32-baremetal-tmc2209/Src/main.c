#include "stm32f1xx.h"
#include "stdio.h"
#include "uart.h"
#include "encoder.h"
#include "tmc2209.h"

void tmc2209_init(void) {

	for(volatile uint32_t i = 0; i < 0x60000; i++);

	/* GCONF 00000000 00000000 00000001 11000000 */
    uint8_t gconf[8] = {0x05, 0x00, 0x00 | 0x80, 0x00, 0x00, 0x01, 0xC0};
    gconf[7] = tmc2209_crc(gconf, 7);
    send_array(gconf, 8);

    /* IHOLD_IRUN 00000110 00000011 00010011 00001001 */
    /*
    IHOLD = 9 (bits 4..0)
	IRUN = 19 (bits 12..8)
	IHOLDDELAY = 6 (bits 19..16)
     */

    uint8_t ihold_irun[8] = {0x05, 0x00, 0x10 | 0x80, 0x00, 0x06, 0x13, 0x09};
    ihold_irun[7] = tmc2209_crc(ihold_irun, 7);
    send_array(ihold_irun, 8);

    /* CHOPCONF 00010100 00000000 10000001 10110011 */

    uint8_t chopconf[8] = {0x05, 0x00, 0x6C | 0x80, 0x14, 0x00, 0x81, 0xB3};
    chopconf[7] = tmc2209_crc(chopconf, 7);
    send_array(chopconf, 8);
}

void vactual(uint32_t value)
{
	uint8_t b4 = (value >> 24) & 0xFF;
	uint8_t b3 = (value >> 16) & 0xFF;
	uint8_t b2 = (value >> 8) & 0xFF;
	uint8_t b1 =  value & 0xFF;

    uint8_t v_byts[8] = {0x05, 0x00, 0x22 | 0x80, b4, b3, b2, b1};
    v_byts[7] = tmc2209_crc(v_byts, 7);
    send_array(v_byts, 8);
}

int main(void)
{
    usart3_halfduplex();
    tmc2209_init();

    /* VACTUAL 00000000 00000000 00010011 10001000 */

//    uint8_t vactual[8] = {0x05, 0x00, 0x22 | 0x80, 0x00, 0x00, 0x13, 0x88};
//    vactual[7] = tmc2209_crc(vactual, 7);

//    send_array(vactual, 8);
//
    vactual(5000);

    while(1)
    {

    }
}
