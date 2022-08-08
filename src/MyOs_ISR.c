/**
 * @file MyOs_ISR.c
 * @author Lucas Orsi (lorsi@itba.edu.ar)
 * @brief 
 * @version 0.1
 * @date 2022-08-08
 * 
 * @copyright Copyright (c) 2022
 * 
 */
#include "MyOs_ISR.h"

#define IRQ_N 53

static MyOs_IsrFunction __MyOs_isrVector[IRQ_N] = {NULL};

bool MyOs_installIRQ(LPC43XX_IRQn_Type irq, MyOs_IsrFunction isr_fun) {
    if (__MyOs_isrVector[irq] == NULL) {
        __MyOs_isrVector[irq] = isr_fun;
        NVIC_ClearPendingIRQ(irq);
        NVIC_EnableIRQ(irq);
        return true;
    } else {
        return false;
    }
}

bool MyOs_uninstallIRQ(LPC43XX_IRQn_Type irq) {
    if (__MyOs_isrVector[irq] != NULL) {
        __MyOs_isrVector[irq] = NULL;
        NVIC_ClearPendingIRQ(irq);
        NVIC_DisableIRQ(irq);
        return true;
    } else {
        return false;
    }
}

void MyOs_IRQHandler(LPC43XX_IRQn_Type irq) {
    MyOs_GeneralState_t osStateBackup =
        MyOs_getInstance()->state;  // FIXME: Getter.
    MyOs_updateState(MY_OS_GENERAL_STATE_IRQ_RUNNING);
    __MyOs_isrVector[irq]();
    MyOs_updateState(osStateBackup);
    NVIC_ClearPendingIRQ(irq);
    if (MyOs_getInstance()->isrSchedulingRequested) {
        MyOs_getInstance()->isrSchedulingRequested = false;  // FIXME: Setter.
        MyOs_yield();
    }
}

/* ************************************************************************* */
/*                                IRQ Wrappers                               */
/* ************************************************************************* */
#define __DECL_IRQ_WRAPPER(name, type) \
    void name##_IRQHandler(void) { MyOs_IRQHandler(type##_IRQn); }

__DECL_IRQ_WRAPPER(DAC, DAC);
__DECL_IRQ_WRAPPER(M0APP, M0APP);
__DECL_IRQ_WRAPPER(DMA, DMA);
__DECL_IRQ_WRAPPER(FLASH_EEPROM, RESERVED1);
__DECL_IRQ_WRAPPER(ETH, ETHERNET);
__DECL_IRQ_WRAPPER(SDIO, SDIO);
__DECL_IRQ_WRAPPER(LCD, LCD);
__DECL_IRQ_WRAPPER(USB0, USB0);
__DECL_IRQ_WRAPPER(USB1, USB1);
__DECL_IRQ_WRAPPER(SCT, SCT);
__DECL_IRQ_WRAPPER(RIT, RITIMER);
__DECL_IRQ_WRAPPER(TIMER0, TIMER0);
__DECL_IRQ_WRAPPER(TIMER1, TIMER1);
__DECL_IRQ_WRAPPER(TIMER2, TIMER2);
__DECL_IRQ_WRAPPER(TIMER3, TIMER3);
__DECL_IRQ_WRAPPER(MCPWM, MCPWM);
__DECL_IRQ_WRAPPER(ADC0, ADC0);
__DECL_IRQ_WRAPPER(I2C0, I2C0);
__DECL_IRQ_WRAPPER(SPI, SPI_INT);
__DECL_IRQ_WRAPPER(I2C1, I2C1);
__DECL_IRQ_WRAPPER(ADC1, ADC1);
__DECL_IRQ_WRAPPER(SSP0, SSP0);
__DECL_IRQ_WRAPPER(SSP1, SSP1);
__DECL_IRQ_WRAPPER(UART0, USART0);
__DECL_IRQ_WRAPPER(UART1, UART1);
__DECL_IRQ_WRAPPER(UART2, USART2);
__DECL_IRQ_WRAPPER(UART3, USART3);
__DECL_IRQ_WRAPPER(I2S0, I2S0);
__DECL_IRQ_WRAPPER(I2S1, I2S1);
__DECL_IRQ_WRAPPER(SPIFI, RESERVED4);
__DECL_IRQ_WRAPPER(SGPIO, SGPIO_INT);
__DECL_IRQ_WRAPPER(GPIO0, PIN_INT0);
__DECL_IRQ_WRAPPER(GPIO1, PIN_INT1);
__DECL_IRQ_WRAPPER(GPIO2, PIN_INT2);
__DECL_IRQ_WRAPPER(GPIO3, PIN_INT3);
__DECL_IRQ_WRAPPER(GPIO4, PIN_INT4);
__DECL_IRQ_WRAPPER(GPIO5, PIN_INT5);
__DECL_IRQ_WRAPPER(GPIO6, PIN_INT6);
__DECL_IRQ_WRAPPER(GPIO7, PIN_INT7);
__DECL_IRQ_WRAPPER(GINT0, GINT0);
__DECL_IRQ_WRAPPER(GINT1, GINT1);
__DECL_IRQ_WRAPPER(EVRT, EVENTROUTER);
__DECL_IRQ_WRAPPER(CAN1, C_CAN1);
__DECL_IRQ_WRAPPER(ADCHS, ADCHS);
__DECL_IRQ_WRAPPER(ATIMER, ATIMER);
__DECL_IRQ_WRAPPER(RTC, RTC);
__DECL_IRQ_WRAPPER(WDT, WWDT);
__DECL_IRQ_WRAPPER(M0SUB, M0SUB);
__DECL_IRQ_WRAPPER(CAN0, C_CAN0);
__DECL_IRQ_WRAPPER(QEI, QEI);