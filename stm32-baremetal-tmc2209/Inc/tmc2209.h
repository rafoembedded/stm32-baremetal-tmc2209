#ifndef TMC2209_H_
#define TMC2209_H_

void usart3_halfduplex(void);
void send_array(uint8_t *arr, uint8_t size);
uint8_t tmc2209_crc(uint8_t *data, uint8_t length);

#endif /* TMC2209_H_ */
