/**
 * @file Serial_port.h
 * @author Bernardo Ramalho & Tiago Alves
 * @brief contains everything related to the serial port
 * @version 0.1
 * @date 2020-01-06
 * 
 * @copyright Copyright (c) 2020
 * 
 */
#pragma once
#include <lcom/lcf.h>

 #ifndef _SERIAL_PORT_H_
 #define _SERIAL_PORT_H_

 /** @defgroup Serial_Port Serial_Port
 * @{
 * Responsible for all the operations regarding the Serial Port
 */


//Serial Port

#define COM1 0x3F8
#define COM1_IRQ 4

//Serial Port Registers

#define RECEIVER_BUFFER_REG 0
#define TRANSMITTER_HOLDING_REG 0
#define INTERRUPT_ENABLE_REG 1
#define INTERRUPT_INDICATION_REG 2
#define FIFO_CONTROL_REG 2
#define LINE_CONTROL_REG 3
#define LINE_STATUS_REG 5
#define DL_LSB 0
#define DL_MSB 1

//LCR Bits

#define LCR_BITS_PER_CHAR_8 (BIT(1) | BIT(0))
#define LCR_STOP_BITS_1 0
#define LCR_EVEN_PARITY (BIT(4) | BIT(3))
#define LCR_ODD_PARITY BIT(3)
#define LCR_DLAB BIT(7)

//IER Bits

#define IER_ENABLE_RECEIVED_DATA_INTERRUPT BIT(0)
#define ENABLE_TRANSMITTER_EMPTY_INTERRUPT BIT(1)
#define IER_ENABLE_RECEIVER_LINE_STATUS_INTERRUPT BIT(2)

//LSR Bits

#define LSR_RECEIVER_READY BIT(0)
#define LSR_TRANSMITTER_EMPTY BIT(5)
#define LSR_OVERRUN_ERROR BIT(1)
#define LSR_PARITY_ERROR BIT(2)
#define LSR_FRAMING_ERROR BIT(3)

//IIR Bits

#define INTERRUPT_ORIGIN_TRANSMITTER_EMPTY BIT(1)
#define INTERRUPT_ORIGIN_RECEIVED_DATA BIT(2)
#define INTERRUPT_ORIGIN_LINE_STATUS (BIT(2) | BIT(1))
#define NO_FIFO 0
#define FIFO_UNUSABLE BIT(7)
#define FIFO_ENABLED (BIT(7) | BIT(6))

//FCR Bits

#define ENABLE_FIFO BIT(0)
#define FCR_CLEAR_RECEIVE_FIFO BIT(1)
#define FCR_CLEAR_TRANSMIT_FIFO BIT(2)
#define FIFO_INTERRUPT_TRIGGER_LEVEL_1 0



/**
 * @brief Subscribes to the uart interrupts
 * 
 * @param bit_no - bit number of the uart
 * @return int - 0 if success or 1 if fail
 */
int uart_subscribe(uint8_t *bit_no);


/**
 * @brief Unsubscribes to the uart interrupts
 * 
 * @return int - 0 if success or 1 if fail
 */
int uart_unsubscribe();


/**
 * @brief Configures the uart so that it can receive interrupts 
 * 
 */
void uart_config();


/**
 * @brief Handles the uart interrupts
 * 
 * @param message - place to save the message received
 * @param size - size of the message received
 */
void uart_ih(char **message, int *size);

/**
 * @brief Checks if the uart as received a full message
 * 
 * @param message - message received
 * @param size - size of the message received
 * @return true - if the uart as received a full message
 * @return false - if the uart as not received a full message
 */
bool uart_check_full_message(char **message, int *size);

/**
 * @brief Sends a message throw de uart
 * 
 * @param message - message to be sent
 * @param size - size of the message to be sent
 */
void uart_send_message(char *message, int size);

/**
 * @brief Sends a byte throw the uart
 * 
 * @param info - byte to be sent
 */
void uart_send_byte(uint8_t info);

/**
 * @brief Clears the uart buffer
 * 
 */
void uart_clear_buffer();

/**@}*/

#endif
