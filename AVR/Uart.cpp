/*
 * Uart.cpp
 *
 *  Created on: Oct 4, 2015
 *      Author: root
 */

#include "Uart.h"

Uart uart(115200);


ISR(USART_RX_vect)
{
	auto data = UDR0;
	uart.__rx_append_char(data);
}

ISR(USART_UDRE_vect)
{
	uart.__tx();
}
Uart::Uart(uint32_t baudrate)
{
	/*
	 * UART constructor for ATmega328p.
	 * Baudrate must be one of the previously presumed values.
	 * See uC for more information.
	 */
	memset(receiver, '\0', receiverBufferSize);
	memset(sender, '\0', senderBufferSize);

	const uint16_t __UBRR = UART_CLOCK / ( 8 * baudrate ) - 1;
	UBRR0H = static_cast<uint8_t>(__UBRR>>8);
	UBRR0L = static_cast<uint8_t>(__UBRR);
	UCSR0A |= 1<<U2X0;
	UCSR0B = 1<<RXCIE0 | 1<<RXEN0 | 1<<TXEN0;
	UCSR0C = 1<<UCSZ01 | 1<<UCSZ00;	//8-bit, no parity, async, 1 stop-bit
}

void Uart::send(const char *data)
{
	/*
	 * Send data over Uart.
	 * Max data size is senderBufferSize
	 */
	strcpy(sender, data);
	UCSR0B |= 1<<UDRIE0;

}
void Uart::send(int data)
{
	constexpr uint8_t TEMP = 30;
	char temp[TEMP];
	memset(temp, '\0', TEMP);
	sprintf(temp, "%d", data);
	this->send(temp);
}
char *Uart::readLine(char *input)
{
	/*
	 * Read line and copy received data into
	 * input variable.
	 * After the data is copied the buffer is purged.
	 */
	auto ptr = strcpy(input, receiver);
	__rx_clear_buffer();
	return ptr;
}

void Uart::clearBuffer()
{
	__rx_clear_buffer();
}

void Uart::__tx()
{
	/*
	 * Send data in the interrupt service.
	 * The data is passed from the send() method
	 * and stored in the linear buffer, which is
	 * cleared on '\0' character found
	 */
	auto data = sender[senderCounter++];
	if(data != '\0')
		UDR0 = data;
	else
	{
		UDR0 = '\0';
		memset(sender, '\0', senderBufferSize);
		senderCounter = 0;
		UCSR0B &= ~1<<UDRIE0;
		UCSR0B |= 1<<TXEN0 | 1<<RXEN0 | 1<<RXCIE0;
	}
}

void Uart::__rx_append_char(const char &data)
{
	/*
	 * Add data into buffer. This is based on the interrupt
	 * service.
	 * IsReadyReadLine returns true if '\0', '\n'
	 * or '\r' character is found or received data exceeds
	 * the size of the buffer
	 */
	if(receiverCounter < receiverBufferSize)
	{
		receiver[receiverCounter++] = data;
		if(data == '\0' || data == '\n' || data == '\r')
			isReadyReadFlag = 1;
	}
	else
		isReadyReadFlag = 1;
}

void Uart::__rx_clear_buffer()
{
	/*
	 * Clean the receiving buffer and prepare it
	 * for the next load of data.
	 */
	memset(receiver, '\0', receiverBufferSize);
	isReadyReadFlag = 0;
	receiverCounter = 0;
}


