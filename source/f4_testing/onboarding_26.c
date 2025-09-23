/**
 * @file onboarding_26.c
 * @author Howard Li (li5642@purdue.edu)
 * @brief  Onboarding 26 starter file
 * @version 0.1
 * @date 2025-09-20
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "f4_testing.h"

#if (F4_TESTING_CHOSEN == TEST_ONBOARDING_26)

#include "common/freertos/freertos.h"
#include "common/phal/gpio.h"
#include "common/phal/rcc.h"

GPIOInitConfig_t gpio_config[] = {
    GPIO_INIT_OUTPUT(GPIOB, 9, GPIO_OUTPUT_LOW_SPEED),
    GPIO_INIT_OUTPUT(GPIOD, 12, GPIO_OUTPUT_LOW_SPEED),
    GPIO_INIT_OUTPUT(GPIOD, 13, GPIO_OUTPUT_LOW_SPEED),
    GPIO_INIT_OUTPUT(GPIOD, 14, GPIO_OUTPUT_LOW_SPEED),
    GPIO_INIT_OUTPUT(GPIOD, 15, GPIO_OUTPUT_LOW_SPEED),
};

extern uint32_t APB1ClockRateHz;
extern uint32_t APB2ClockRateHz;
extern uint32_t AHBClockRateHz;
extern uint32_t PLLClockRateHz;

#define TargetCoreClockrateHz 16000000
ClockRateConfig_t clock_config = {
    .clock_source              = CLOCK_SOURCE_HSI,
    .use_pll                   = false,
    .vco_output_rate_target_hz = 160000000,
    .system_clock_target_hz    = TargetCoreClockrateHz,
    .ahb_clock_target_hz       = (TargetCoreClockrateHz / 1),
    .apb1_clock_target_hz      = (TargetCoreClockrateHz / (1)),
    .apb2_clock_target_hz      = (TargetCoreClockrateHz / (1)),
};

void HardFault_Handler();
void ledblink1();
void greenLEDBlink();
void orangeLEDBlink();
void redLEDBlink();
void blueLEDBlink();
void addDelay(int delay);

void bitwiseInit();
void EXTI9_5_IRQHandler();

bool normFreq = true;
 
defineThreadStack(ledblink1, 100, osPriorityNormal, 64);
defineThreadStack(greenLEDBlink, 100, osPriorityNormal, 64);
defineThreadStack(orangeLEDBlink, 250, osPriorityNormal, 64);
defineThreadStack(redLEDBlink, 500, osPriorityNormal, 64);
defineThreadStack(blueLEDBlink, 500, osPriorityNormal, 64);


int main() {
    osKernelInitialize();

    if (0 != PHAL_configureClockRates(&clock_config)) {
        HardFault_Handler();
    }
    if (!PHAL_initGPIO(gpio_config, sizeof(gpio_config) / sizeof(GPIOInitConfig_t))) {
        HardFault_Handler();
    }

    bitwiseInit();

    createThread(ledblink1);
    createThread(greenLEDBlink);
    createThread(orangeLEDBlink);
    createThread(redLEDBlink);
    createThread(blueLEDBlink);

    osKernelStart(); // Go!

    return 0;
}

void ledblink1() {
    PHAL_toggleGPIO(GPIOB, 9);
}

void greenLEDBlink() {
    PHAL_toggleGPIO(GPIOD, 12);
}

void orangeLEDBlink() {
    PHAL_toggleGPIO(GPIOD, 13);
}

void redLEDBlink() {
    PHAL_toggleGPIO(GPIOD, 14);
}

void blueLEDBlink() {
    PHAL_toggleGPIO(GPIOD, 15);
    if(normFreq){
        osDelay(500);
    }
}

void HardFault_Handler() {
    while (1) {
        __asm__("nop");
    }
}

void bitwiseInit() {
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    SYSCFG->EXTICR[1] |= (0b0001 << 12);

    EXTI->IMR |= (0x1 << 7);

    EXTI->RTSR |= (0x1 << 7);

    EXTI->FTSR &= ~(0x1 << 7);

    NVIC_EnableIRQ(EXTI9_5_IRQn);
}

void EXTI9_5_IRQHandler() {
	if (EXTI->PR & (1 << 7)) {
		EXTI->PR |= (1 << 7);
        normFreq = !normFreq;
	}
}

#endif // F4_TESTING_CHOSEN == TEST_ONBOARDING_26
