/*!
** Copyright 2020 NXP
** @file main.c
** @brief
**         Main module.
**         This module contains user's application code.
*/
/*!
**  @addtogroup main_module main module documentation
**  @{
*/
/* MODULE main */


/* Including necessary configuration files. */
#include "sdk_project_config.h"
#include "S32K144.h"
#include "queue.h"
volatile int exit_code = 0;
uint32_t baud = 0;


void LPUART1_Init(void)
{
    /*1. Enable Clock Port C*/
    PCC->PCCn[PCC_PORTC_INDEX] |= PCC_PCCn_CGC_MASK;

    /*2. Configure LPUART1 Pins*/
    PORTC->PCR[6] = PORT_PCR_MUX(2);  //RX
    PORTC->PCR[7] = PORT_PCR_MUX(2);  //TX

    /*3. Enable and Configure SIRC*/
    SCG->SIRCCSR &= ~SCG_SIRCCSR_SIRCEN_MASK;
    while ((SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) != 0U);
    SCG->SIRCCFG = SCG_SIRCCFG_RANGE(1);
    SCG->SIRCDIV = SCG_SIRCDIV_SIRCDIV2(1);
    SCG->SIRCCSR |= SCG_SIRCCSR_SIRCEN_MASK;
    while ((SCG->SIRCCSR & SCG_SIRCCSR_SIRCVLD_MASK) == 0U);

    /*4. Enable clock for LPUART1*/
    PCC->PCCn[PCC_LPUART1_INDEX] &= ~PCC_PCCn_CGC_MASK;  // Disable LPUART1 trước
    PCC->PCCn[PCC_LPUART1_INDEX] |= PCC_PCCn_PCS(2)      // SIRC
                                  | PCC_PCCn_CGC_MASK;   // Enable clock

    /*5. Disable TX/RX trước khi config*/
    LPUART1->CTRL = 0x00000000;  // Disable

    /*6. Configure BAUD register*/
    LPUART1->BAUD &= ~(LPUART_BAUD_M10_MASK | LPUART_BAUD_SBNS_MASK);
    LPUART1->STAT &= ~(LPUART_STAT_MSBF_MASK | LPUART_STAT_RXINV_MASK);

    /*7. Set Baudrate */
    baud = LPUART1->BAUD;
    baud &= ~(LPUART_BAUD_OSR_MASK | LPUART_BAUD_SBR_MASK);
    baud |= LPUART_BAUD_OSR(9);
    baud |= LPUART_BAUD_SBR(7);

    LPUART1->BAUD = baud;

    /*8. Configure: 8-bit data, No parity*/
    LPUART1->CTRL &= ~(LPUART_CTRL_M_MASK | LPUART_CTRL_PE_MASK);

    /*9. Enable TX and RX*/
    LPUART1->CTRL |= (LPUART_CTRL_TE_MASK | LPUART_CTRL_RE_MASK);
}

void LPUART1_EnableRxInterrupt(void){

	LPUART1->CTRL |= LPUART_CTRL_RIE_MASK;

	S32_NVIC->IP[LPUART1_RxTx_IRQn] = (2 << 4);

	S32_NVIC->ISER[LPUART1_RxTx_IRQn / 32] = (1UL << (LPUART1_RxTx_IRQn % 32));
}

void LPUART1_RxTx_IRQHandler(void)
{
    if (LPUART1->STAT & LPUART_STAT_RDRF_MASK)
    {
        uint8_t ch = (uint8_t)LPUART1->DATA;

        LPUART1->DATA = ch;
    }
}


void LPUART1_Transmit_Char(char c)
{

    while (!(LPUART1->STAT & LPUART_STAT_TDRE_MASK));

    LPUART1->DATA = c;
}

void LPUART1_Transmit_String(const char *str)
{
    while (*str != '\0') {
        LPUART1_Transmit_Char(*str);
        str++;
    }
}


int main(void)
{
    LPUART1_Init();
    LPUART1_Transmit_String("Hello World");
    LPUART1_EnableRxInterrupt();

    while (1) {
//        for (volatile int i = 0; i < 10000000; i++);
//        LPUART1_Transmit_String("Hello World");
    }
}

/* END main */
/*!
** @}
*/
