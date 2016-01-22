/*
 * Uart.h
 *
 *  Created on: Oct 4, 2015
 *      Author: root
 */

#ifndef UART_H_
#define UART_H_
#include "Pin.h"
#include <string.h>
#include <stdio.h>
#include <avr/wdt.h>

constexpr uint32_t UART_CLOCK = 16000000;
constexpr uint8_t receiverBufferSize = 200;
constexpr uint8_t senderBufferSize = 200;


class Uart {

	char receiver[receiverBufferSize];
	char sender[senderBufferSize];
	uint8_t isReadyReadFlag {0};
	uint8_t senderCounter {0};
	uint8_t receiverCounter {0};
public:
	Uart(uint32_t baudrate);

	void send(const char *data);
	void send(int data);
	uint8_t isReadyReadLine() const { return isReadyReadFlag; }
	char *readLine(char *input);
	void clearBuffer();

	void writeReceiveBuffer() { this->send(receiver); }
	void __tx();
	inline  void __rx_append_char(const char &data);
	void __rx_clear_buffer();
};

extern Uart uart;

void setUartWatchdog();



#endif /* UART_H_ */
