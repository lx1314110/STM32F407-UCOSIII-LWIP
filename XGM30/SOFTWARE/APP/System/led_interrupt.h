#ifndef CONTROL_HEADER
#define CONTROL_HEADER

//!
#define LED_RUN_PORT      GPIOE
#define LED_RUN_PIN       GPIO_Pin_4

//! GPIO_PinState defintion.
typedef enum
{
  ON = 0,
  OFF
}State;

//! led io init Fuc.
void led_gpio_init(void);

//! led indicate Fuc.
void Led_indicate(void);
#endif