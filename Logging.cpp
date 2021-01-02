/*
 * Logging.cpp
 *
 *  Created on: 08.12.2020
 *      Author: thomas
 */

#include <Logging.h>

Logging::Logging()
{
    // TODO Auto-generated constructor stub

}

Logging::~Logging()
{
    // TODO Auto-generated destructor stub
}

void Logging::begin( uint16_t baudSetting)
{
    /* Configure hardware UART */
    P1SEL = BIT1 + BIT2; // P1.1 = RXD, P1.2=TXD
    P1SEL2 = BIT1 + BIT2; // P1.1 = RXD, P1.2=TXD
    UCA0CTL1 |= UCSSEL_2; // Use SMCLK
    UCA0BR0 = baudSetting & 0xFF; // Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
    UCA0BR1 = baudSetting >> 8; // Set baud rate to 9600 with 1MHz clock
    UCA0MCTL = UCBRS0; // Modulation UCBRSx = 1
    UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
    IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt

}
void Logging::write(char c)
{
    while (!(IFG2 & UCA0TXIFG))
        ; // Wait for TX buffer to be ready for new data
    UCA0TXBUF = c;
}

void Logging::log_Hex(uint8_t b)
{
    log_i(b, 16);
}

void Logging::log_f( float number, uint8_t decimals) {
	uint32_t n= (uint32_t)number;
	log_i( n,10);
	write('.');
	float comma= number-n;
	for (int i=0; i<decimals; i++) {
		comma=comma*10;
		n= (uint8_t)comma;
		comma= comma-n;
		write( '0'+n);
	}
}

void Logging::log_i(int32_t value, uint8_t base)
{
    const char out[] = "0123456789ABCDEF";

    char buffer[32];

    buffer[0]=0;

    if (value < 0)
    {
        write('-');
        value = value * -1;
    }
    int8_t i = 0;
    while (value > 0 && i < 32)
    {
        buffer[i] = value % base;
        value = value / base;
        if (value==0) {
            break;
        }
        i++;
    }

    if (base == 16)
    {
        write('0');
        write('x');
        if (i <= 0)
        {
            write('0');
        }
    }
    else if (base == 2)
    {
        write('0');
        write('b');
    }

    while (i >= 0)
    {
        write(out[buffer[i]]);
        i--;
    }
}

void Logging::logTxt(char *TxArray, unsigned char ArrayLength)
{
    while (ArrayLength--)
    { // Loop until StringLength == 0 and post decrement
        while (!(IFG2 & UCA0TXIFG))
            ; // Wait for TX buffer to be ready for new data
        UCA0TXBUF = *TxArray; //Write the character at the location specified py the pointer
        TxArray++; //Increment the TxString pointer to point to the next character
    }
}
void Logging::CRLF()
{
    write(13);
    write(10);
}

Logging Log = Logging();
