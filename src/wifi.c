/*
 * wifi.c
 * ESP8266 DRIVER Functions
 *
 *  Created on: 2015 jún. 19
 *      Author: Richárd
 */
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "ch.h"
#include "hal.h"
#include "wifi.h"
#include "chprintf.h"

//Connection String: AT+CIPSTART=0,"UDP","192.168.4.255",4000,4001,0
char connstring[] = {0x41, 0x54, 0x2b, 0x43, 0x49, 0x50, 0x53,
                     0x54, 0x41, 0x52, 0x54, 0x3d, 0x30, 0x2c,
                     0x22, 0x55, 0x44, 0x50, 0x22, 0x2c, 0x22,
                     0x31, 0x39, 0x32, 0x2e, 0x31, 0x36, 0x38,
                     0x2e, 0x34, 0x2e, 0x32, 0x35, 0x35, 0x22,
                     0x2c, 0x34, 0x30, 0x30, 0x30, 0x2c, 0x34,
                     0x30, 0x30, 0x31, 0x2c, 0x30, 0x0D, 0x0A};

static SerialConfig uartCfg1 =
{
   230400, // bit rate
};
/*
 * ESP8266 INIT
 */
void init_wifi()
{
  sdStart(&SD1, &uartCfg1);
  chThdSleepMilliseconds(1000);                         //Wait For Ready state
  chSequentialStreamWrite(&SD1, "AT+CIPMUX=1\r\n",13);  //Multiple connection
  chThdSleepMilliseconds(10);
  chSequentialStreamWrite(&SD1,  connstring, 49);       //Connection string
  chThdSleepMilliseconds(10);
  chSequentialStreamWrite(&SD1, "ATE0\r\n",6);          //ECHO OFF
  chThdSleepMilliseconds(10);
  clear_serial_buffer();                                //Clear serial buffer
}

/*
 * Send Packet via UDP connection
 */
void wifi_send(char *data[], int length)
{
  chSysLock();
  //clear_serial_buffer();
  char *send_string[16];
  sprintf(&send_string, "AT+CIPSEND=0,%d\r\n", length); //Start UDP connection
  chprintf(&SD1, send_string);                          //Send
  chThdSleepMilliseconds(1);
  //WaitForPrompt();                                      //Wait for start the connection
  chSequentialStreamWrite(&SD1, data, length);          //Send the datas
  chSysUnlock();
}

/*
 * Clear serial buffer for the incoming data
 */
void clear_serial_buffer()
{
  char c;
  while(!sdGetWouldBlock(&SD1))
  {
    chSequentialStreamRead(&SD1, &c, 1);
  }
}

/*
 * Wait the prompt from the ESP8266
 */
void WaitForPrompt()
{
  char c;
  do{
    chSequentialStreamRead(&SD1, &c, 1);
  }while(c != 0x3E);
}


/*
 * Debug functions
 */
void send_at(BaseSequentialStream *chp, int argc, char *argv[])
{
  clear_serial_buffer();
  chprintf(&SD1, "AT\r\n");
  chThdSleepMilliseconds(4);
  read_buffer(chp);
}
void read_buffer(BaseSequentialStream *chp)
{
  uint8_t scanbuff[50];
  int counter = 0;
  char sajt[30];
  while(!sdGetWouldBlock(&SD1))
  {
    scanbuff[counter] = sdGet(&SD1);
    counter++;
  }

  sscanf((const char*)scanbuff, "%s", &sajt);

  if (strcmp(sajt, "SEND") ==  0)
  {
    palClearPad(GPIOB, GPIOB_LED1);
    chThdSleepMilliseconds(200);

    palSetPad(GPIOB, GPIOB_LED1);
    chThdSleepMilliseconds(200);
  }

  chprintf(chp, "%s\r\n", &sajt);
}
void wifi_debug(BaseSequentialStream *chp, int argc, char *argv[]) {
  chprintf(chp, "DATA SENT: %s %d\r\n", argv[0], strlen(argv[0]));
  wifi_send(argv[0], strlen(argv[0]));

  clear_serial_buffer();
  chThdSleepMilliseconds(4);
  read_buffer(chp);
}
void read_buffer_debug(BaseSequentialStream *chp, int argc, char *argv[]) {
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B\x63");
    chprintf(chp, "\x1B[2J");

    read_buffer(chp);


    chThdSleepMilliseconds(200);
  }

}

//------------------------------------------------------------------
