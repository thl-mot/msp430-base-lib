/*
 * Wire.cpp
 *
 *  Created on: 06.12.2020
 *      Author: thomas
 */

#include <Msp430Wire.h>

Msp430Wire::Msp430Wire()
{
}

Msp430Wire::~Msp430Wire()
{
}

void Msp430Wire::begin()
{
    /* Configure P1.6 and P1.7 for I2C */
    P1SEL |= BIT6 + BIT7;
    P1SEL2 |= BIT6 + BIT7;

    /* Ensure USCI_B0 is in reset before configuring */
    UCB0CTL1 = UCSWRST;

    /* Set USCI_B0 to master mode I2C mode */
    UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;

    /**
     * Configure the baud rate registers for 100kHz when sourcing from SMCLK
     * where SMCLK = 1MHz
     */
    UCB0BR0 = 10;
    UCB0BR1 = 0;

    /* Take USCI_B0 out of reset and source clock from SMCLK */
    UCB0CTL1 = UCSSEL_2;
}

int Msp430Wire::transfer(uint8_t addr, uint8_t *outBuffer, uint8_t outSize,
                         uint8_t *inBuffer, uint8_t inSize)
{
    int err = 0;

    /* Set the slave device address */
    UCB0I2CSA = addr;

    /* Transmit data is there is any */
    if (outSize > 0)
    {
        err = transmit(outBuffer, outSize);
    }

    /* Receive data is there is any */
    if ((err == 0) && (inSize > 0))
    {
        err = receive(inBuffer, inSize);
    }
    else
    {
        /* No bytes to receive send the stop condition */
        UCB0CTL1 |= UCTXSTP;
    }

    return err;
}

int Msp430Wire::transmit(uint8_t *buffer, uint8_t size)
{
    /* Send the start condition */
    UCB0CTL1 |= UCTR | UCTXSTT;

    /* Wait for the start condition to be sent and ready to transmit interrupt */
    while ((UCB0CTL1 & UCTXSTT) && ((IFG2 & UCB0TXIFG) == 0))
        ;

    /* Check for ACK */
    int err = check_ack();

    /* If no error and bytes left to send, transmit the data */
    while ((err == 0) && (size > 0))
    {
        UCB0TXBUF = *buffer;
        while ((IFG2 & UCB0TXIFG) == 0)
        {
            err = check_ack();
            if (err < 0)
            {
                break;
            }
        }

        buffer++;
        size--;
    }

    return err;
}

int Msp430Wire::receive(uint8_t *buffer, uint8_t size)
{
    /* Send the start and wait */
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;

    /* Wait for the start condition to be sent */
    while (UCB0CTL1 & UCTXSTT)
        ;

    /*
     * If there is only one byte to receive, then set the stop
     * bit as soon as start condition has been sent
     */
    if (size == 1)
    {
        UCB0CTL1 |= UCTXSTP;
    }

    /* Check for ACK */
    int err = check_ack();

    /* If no error and bytes left to receive, receive the data */
    while ((err == 0) && (size > 0))
    {
        /* Wait for the data */
        while ((IFG2 & UCB0RXIFG) == 0)
            ;

        *buffer = UCB0RXBUF;
        buffer++;
        size--;

        /*
         * If there is only one byte left to receive
         * send the stop condition
         */
        if (size == 1)
        {
            UCB0CTL1 |= UCTXSTP;
        }
    }

    return err;
}

int Msp430Wire::ping(uint8_t addr) {
    /* Set the slave device address */
    UCB0I2CSA = (addr >> 1) | 1;

    /* Send the start and wait */
    UCB0CTL1 &= ~UCTR;
    UCB0CTL1 |= UCTXSTT;

    /* Wait for the start condition to be sent */
    while (UCB0CTL1 & UCTXSTT)
        ;

    UCB0CTL1 |= UCTXSTP;

    /* Check for ACK */
    int err = check_ack();

    return err;
}

int Msp430Wire::check_ack()
{
    int err = 0;

    /* Check for ACK */
    if (UCB0STAT & UCNACKIFG)
    {
        /* Stop the I2C transmission */
        UCB0CTL1 |= UCTXSTP;

        /* Clear the interrupt flag */
        UCB0STAT &= ~UCNACKIFG;

        /* Set the error code */
        err = -2;
    }
    return err;
}

Msp430Wire Wire = Msp430Wire();
