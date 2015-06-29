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
#include "console.h"
#include "can_comm.h"
#include "at_mode.h"
#include "wifi.h"

static WORKING_AREA(waThread1, 128);
static msg_t Thread1(void *arg) {

  (void)arg;
  chRegSetThreadName("blinker");
  while (TRUE) {

    palClearPad(GPIOB, GPIOB_LED1);
    chThdSleepMilliseconds(200);

    palSetPad(GPIOB, GPIOB_LED1);
    chThdSleepMilliseconds(200);
  }
}


/*
 * Application entry point.
 */
int main(void) {

  halInit();
  chSysInit();

  if (palReadPad(GPIOA, GPIOA_IN0) == PAL_HIGH)
    init_atmode();

  palClearPad(GPIOB, GPIOB_LED1);

  consoleInit();

  init_wifi();

  //char sajt[] = "sajt";
  //wifi_send(sajt, 4);

  can_commInit();



  /*
   * Creates the example thread.
   */
  chThdCreateStatic(waThread1, sizeof(waThread1), NORMALPRIO, Thread1, NULL);
  while (TRUE) {
    consoleStart();
    chThdSleepMilliseconds(1000);
  }
}
