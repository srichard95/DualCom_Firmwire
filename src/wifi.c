/*
 * wifi.c
 * ESP8266 DRIVER Functions
 *
 *  Created on: 2015 jún. 19
 *      Author: Richárd
 */
#include "ch.h"
#include "hal.h"
#include "wifi.h"
#include "chprintf.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
   115200, // bit rate
   0,
   0,
   0
};

void init_wifi()
{
  sdStart(&SD1, &uartCfg1);
  chThdSleepMilliseconds(2000);
  chSequentialStreamWrite(&SD1, "AT+CIPMUX=1\r\n",13);
  chThdSleepMilliseconds(100);
  chSequentialStreamWrite(&SD1,  connstring, 49);
  chThdSleepMilliseconds(10);
  chSequentialStreamWrite(&SD1, "ATE0\r\n",6);
  chThdSleepMilliseconds(10);
  clear_serial_buffer();
}

void wifi_send(char *data[], int length)
{
  clear_serial_buffer();
  char send_string[16];
  sprintf(send_string, "AT+CIPSEND=0,%d\r\n", length);
  chprintf(&SD1, send_string);
  WaitForPrompt();
  chprintf(&SD1, data);
}
void clear_serial_buffer()
{
  while(!sdGetWouldBlock(&SD1))
  {
    char c;
    chSequentialStreamRead(&SD1, &c, 1);
  }
}

void WaitForPrompt()
{
  char c;
  do{
    chSequentialStreamRead(&SD1, &c, 1);
  }while(c != 0x3E);
}


//debug functions--------------------------------------------------
void send_at(BaseSequentialStream *chp, int argc, char *argv[])
{
  clear_serial_buffer();
  chprintf(&SD1, "AT\r\n");
  chThdSleepMilliseconds(4);
  read_buffer(chp);
}
void read_buffer(BaseSequentialStream *chp)
{
  while(!sdGetWouldBlock(&SD1))
  {
    char c;
    c = sdGet(&SD1);
    chprintf(chp, "%c", c);
  }
  chprintf(chp, "\r\n");
}
void wifi_debug(BaseSequentialStream *chp, int argc, char *argv[]) {
  chprintf(chp, "DATA SENT: %s %d\r\n", argv[0], strlen(argv[0]));
  wifi_send(argv[0], strlen(argv[0]));

  clear_serial_buffer();
  chThdSleepMilliseconds(4);
  read_buffer(chp);
}
//------------------------------------------------------------------
