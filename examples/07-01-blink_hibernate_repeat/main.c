/******************************************************************
 * Hubert Data Logger
 * Blink Hibenerate Repeat Example
 * Developed by Sevun Scientific, Inc.
 * http://sevunscientific.com
 * *****************************************************************
 *
 *    _____/\\\\\\\\\\\_______/\\\\\\\\\\\____/\\\\\\\\\\\_
 *     ___/\\\/////////\\\___/\\\/////////\\\_\/////\\\///__
 *      __\//\\\______\///___\//\\\______\///______\/\\\_____
 *       ___\////\\\___________\////\\\_____________\/\\\_____
 *        ______\////\\\___________\////\\\__________\/\\\_____
 *         _________\////\\\___________\////\\\_______\/\\\_____
 *          __/\\\______\//\\\___/\\\______\//\\\______\/\\\_____
 *           _\///\\\\\\\\\\\/___\///\\\\\\\\\\\/____/\\\\\\\\\\\_
 *            ___\///////////_______\///////////_____\///////////__
 *
 * *****************************************************************
 */

#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "utils/uartstdio.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "driverlib/hibernate.h"
#include "driverlib/pin_map.h"
#include "driverlib/sysctl.h"

#define UART_SPEED              115200
#define HIBERNATE_WAKE_DELAY    5

int main(void)
{
    // Set microcontroller to use the 16 MHz external crystal
    SysCtlClockSet(SYSCTL_SYSDIV_1 | SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_16MHZ);

    //*****************************************************************************
    // Pins Setup
    //*****************************************************************************

    // Enable the peripherals
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    // Sets the pin associated with IND1 to be an output
    GPIOPinTypeGPIOOutput(GPIO_PORTC_BASE, GPIO_PIN_4);

    // Sets the pins associated with UART0
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //*****************************************************************************
    // UART Setup
    //*****************************************************************************

    // Initialize the UART0 using uartstdio
    UARTStdioConfig(0, UART_SPEED, SysCtlClockGet());

    //*****************************************************************************
    // Hibernate Setup
    //*****************************************************************************

    // Perform normal power-on initialization
    SysCtlPeripheralEnable(SYSCTL_PERIPH_HIBERNATE);
    while(!SysCtlPeripheralReady(SYSCTL_PERIPH_HIBERNATE)) {}
    HibernateEnableExpClk(SysCtlClockGet());
    HibernateGPIORetentionEnable();
    SysCtlDelay(SysCtlClockGet()/3/50); // Delay 2 ms
    HibernateClockConfig(HIBERNATE_OSC_HIGHDRIVE);
    HibernateRTCEnable();
    HibernateRTCTrimSet (0x7FFF);   // This line is necessary due to bug in Hibernate
    HibernateWakeSet(HIBERNATE_WAKE_PIN | HIBERNATE_WAKE_RTC);

    //*****************************************************************************
    // Main Code
    //*****************************************************************************

    UARTprintf("\r\n%d seconds",HibernateRTCGet());

    // Writes HIGH to pins
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, GPIO_PIN_4);  // IND1 LED On
    SysCtlDelay(SysCtlClockGet()/3/10);                     // Delay 0.1 second
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_4, 0);           // IND2 LED Off

    HibernateRTCMatchSet(0,HibernateRTCGet()+HIBERNATE_WAKE_DELAY);
    HibernateRequest();

    while(1) {}
}
