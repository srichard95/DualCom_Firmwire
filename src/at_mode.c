/*
 * at_mode.c
 *
 *  Created on: 2015 jún. 12
 *      Author: Richárd
 *
 *      AT mode for WiFi
 */
#include "ch.h"
#include "hal.h"
#include "at_mode.h"

static SerialConfig uartCfg1 =
{
230400, // bit rate
0,
0,
0
};

static SerialConfig uartCfg2 =
{
115200, // bit rate
0,
0,
0
};

/*
 * Receive from the RS232 and send to the WiFi
 */
static WORKING_AREA(waSending, 128);
static msg_t Sending(void *arg) {

  (void) arg; // Unused parameter
  while (1)
  {
      palClearPad(GPIOB, GPIOB_LED1);
      char c;
      sdRead(&SD2, &c, 1);
      palSetPad(GPIOB, GPIOB_LED1);
      sdPut(&SD1, c);
  }
}

/*
 * Receive from the WiFi and send to the RS232
 */
static WORKING_AREA(waReceive, 128);
static msg_t Receive(void *arg) {

  (void) arg; // Unused parameter
  while (1)
  {
      palClearPad(GPIOB, GPIOB_LED1);
      char c;
      sdRead(&SD1, &c, 1);
      palSetPad(GPIOB, GPIOB_LED1);
      sdPut(&SD2, c);
  }
}


/*
 * Init AT MODE
 */
void init_atmode()
{
  sdInit();
  sdStart(&SD1, &uartCfg1);     //Start Serial Driver 1
  sdStart(&SD2, &uartCfg2);     //Start Serial Driver 2

  chThdCreateStatic(waSending, sizeof(waSending), NORMALPRIO, Sending, NULL);
  chThdCreateStatic(waReceive, sizeof(waReceive), NORMALPRIO, Receive, NULL);

  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}


