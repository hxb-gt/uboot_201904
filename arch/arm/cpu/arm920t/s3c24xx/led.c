// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2010, 2009 Matthias Kaehlcke <matthias@kaehlcke.net>
 */

#include <asm/io.h>
#include <asm/arch/s3c2440.h>
#include <asm/arch/s3c24xx.h>
#include <config.h>
#include <status_led.h>

#define PIN_INDEX(x)		(x % 0xff)
#define GPIO_INDEX(x)		(x / 0xff)

static uint8_t saved_state[2] = {CONFIG_LED_STATUS_OFF, CONFIG_LED_STATUS_OFF};
static uint32_t gpio_pin[2] = {1 << PIN_INDEX(CONFIG_LED_STATUS_GREEN),
			       1 << PIN_INDEX(CONFIG_LED_STATUS_RED)};
static uint32_t gpio_port[2] = {GPIO_INDEX(CONFIG_LED_STATUS_GREEN),
			       GPIO_INDEX(CONFIG_LED_STATUS_RED)};

S3C24X0_GPIO_SIMP* get_gpio_base(int index)
{
	S3C24X0_GPIO_SIMP* gpio;
	switch(index) {
	case 0:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOA_BASE;
		break;
	case 1:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOB_BASE;
		break;
	case 2:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOC_BASE;
		break;
	case 3:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOD_BASE;
		break;
	case 4:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOE_BASE;
		break;
	case 5:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOF_BASE;
		break;
	case 6:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOG_BASE;
		break;
	case 7:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOH_BASE;
		break;
	case 8:
		gpio = (S3C24X0_GPIO_SIMP*)S3C24X0_GPIOJ_BASE;
		break;
	default:
		gpio = NULL;
	}
	return gpio;
}

static inline void switch_LED_on(uint8_t led)
{
	S3C24X0_GPIO_SIMP *gpio = get_gpio_base(gpio_port[led]);
	if (gpio == NULL)
		return;
	writel(readl(&gpio->GPJDAT) | gpio_pin[led], &gpio->GPJDAT);
	saved_state[led] = CONFIG_LED_STATUS_ON;
}

static inline void switch_LED_off(uint8_t led)
{
	S3C24X0_GPIO_SIMP *gpio = get_gpio_base(gpio_port[led]);

	if (gpio == NULL)
			return;
	writel(readl(&gpio->GPJDAT) & ~gpio_pin[led], &gpio->GPJDAT);
	saved_state[led] = CONFIG_LED_STATUS_OFF;
}

void red_led_on(void)
{
	switch_LED_on(1);
}

void red_led_off(void)
{
	switch_LED_off(1);
}

void green_led_on(void)
{
	switch_LED_on(0);
}

void green_led_off(void)
{
	switch_LED_off(0);
}

void __led_init(led_id_t mask, int state)
{
	__led_set(mask, state);
}

void __led_toggle(led_id_t mask)
{
	if (CONFIG_LED_STATUS_RED == mask) {
		if (CONFIG_LED_STATUS_ON == saved_state[1])
			red_led_off();
		else
			red_led_on();
	} else if (CONFIG_LED_STATUS_GREEN == mask) {
		if (CONFIG_LED_STATUS_ON == saved_state[0])
			green_led_off();
		else
			green_led_on();
	}
}

void __led_set(led_id_t mask, int state)
{
	if (CONFIG_LED_STATUS_RED  == mask) {
		if (CONFIG_LED_STATUS_ON == state)
			red_led_on();
		else
			red_led_off();
	} else if (CONFIG_LED_STATUS_GREEN == mask) {
		if (CONFIG_LED_STATUS_ON == state)
			green_led_on();
		else
			green_led_off();
	}
}
