/**
 * \file
 *
 * \brief Driver ISR.
 *
 (c) 2020 Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms,you may use this software and
    any derivatives exclusively with Microchip products.It is your responsibility
    to comply with third party license terms applicable to your use of third party
    software (including open source software) that may accompany Microchip software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
 *
 */

/*
 * Code generated by START.
 *
 * This file will be overwritten when reconfiguring your START project.
 * Please copy examples or other code you want to keep to a separate file
 * to avoid losing it when reconfiguring.
 */

//#include <driver_init.h>
//#include <compiler.h>
#include "include/driver_init.h"
#include "utils/compiler.h"

#include "include/i2c_slave.h"
extern void I2C_0_initialization(void);

volatile uint8_t TimerTrigger = 0;
volatile uint8_t INTTrigger = 0;
volatile uint8_t TimeOut = 0;
volatile uint8_t PinChanged = 0;
volatile uint8_t LastPinState;
volatile uint8_t NowPinState;
volatile uint8_t PinPassed;

ISR(TCA0_CMP0_vect)
{
	/* Insert your TCA Compare 0 Interrupt handling code here */
	TCA0.SINGLE.CNT = 0x0; // Recycle Timer
	TimeOut = 1;
	
	/* The interrupt flag has to be cleared manually */
	TCA0.SINGLE.INTFLAGS = TCA_SINGLE_CMP0_bm;
}


ISR(PORTA_PORT_vect)
{
	/* Insert your PORTA interrupt handling code here */
	if (VPORTA_INTFLAGS & (1 << 7))	// both edge detect on Intruder pin
	{	
		if ((PinChanged == 0x0) && (INTTrigger == 0x0)) {
			LastPinState = VPORTA_IN & (1 << 7);
			PinChanged = 1;
			PinPassed = 2;
			/* Start Timer */
			TimeOut = 0;
			//TCA0.SINGLE.CMP0 = 0x621;
			TCA0.SINGLE.CMP0 = 0x1A0; /* 5mS, modified in version A1 */
			TCA0.SINGLE.CNT = 0x0;
			TCA0.SINGLE.CTRLA |= TCA_SINGLE_ENABLE_bm;
			TimerTrigger = 1;
		}	
		VPORTA_INTFLAGS = (1 << 7);
	}
	if (VPORTA_INTFLAGS & (1 << 6))
	{
		/* check nRSMRST pin to decide open or close I2C */	
		if (VPORTA_IN & (1 << 6))
		{
			I2C_0_open();
		}
		else
		{
			I2C_0_close();
		}
		VPORTA_INTFLAGS = (1 << 6);
	}
	/* Clear interrupt flags */
	//VPORTA_INTFLAGS = (1 << 6) | (1 << 7);
}