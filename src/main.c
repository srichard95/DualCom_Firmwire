/*
    ChibiOS/RT - Copyright (C) 2006-2013 Giovanni Di Sirio

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

        http://www.apache.org/licenses/LICENSE-2.0

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
*/

#include "ch.h"
#include "hal.h"
#include "can_comm.h"

static SerialConfig uartCfg =
{
115200, // bit rate
0,
0,
0
};

void serial_init(void)
{

  sdInit();
  sdStart(&SD2, &uartCfg);
}
static uint8_t txbuf = 0x01;
void serial_send(void)
{
  //txbuf++;
  //sdWrite(&SD6, (uint8_t *) txbuf, 1);
  sdPut(&SD2, txbuf);
}



static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {

    palClearPad(GPIOB, GPIOB_LED1);     //LS1
    chThdSleepMilliseconds(100);

    serial_send();

    palSetPad(GPIOB, GPIOB_LED1);     //LS1
    chThdSleepMilliseconds(100);

    serial_send();

  }
}

/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();


  serial_init();

  can_commInit();


  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  while (TRUE) {
    chThdSleepMilliseconds(500);
  }
}
