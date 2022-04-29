/* 
 * File:   platform_uart.c
 * Author: cocus
 *
 * Created on 12 de junio de 2019, 16:54
 */

#include "platform_uart.h"


void InitUSART(uint16_t baud)
{
    CloseUSART();
    
    
#if (_XTAL_FREQ ==  4000000)    
    switch(baud)
    {
        case 1200:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 207);            
            break;
        }        
        case 4800:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 51);            
            break;
        }
        case 9600:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 25);
            break;
        }
        case 19200:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_HIGH, 12);
            break;
        }
    }
#elif (_XTAL_FREQ == 16000000)
    switch(baud)
    {
        case 1200:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 207);            
            break;
        }        
        case 4800:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 51);            
            break;
        }
        case 9600:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 25);
            break;
        }
        case 19200:
        {
            OpenUSART(USART_TX_INT_OFF & USART_RX_INT_ON & USART_ASYNCH_MODE & 
                      USART_EIGHT_BIT & USART_CONT_RX & USART_BRGH_LOW, 12);
            break;
        }
    }    
#endif
}
