/*
 * can_comm.c

 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */
#include <stdlib.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "can_comm.h"
#include "console.h"
#include "wifi.h"


char buffer[BUFFER_SIZE];
int packet_counter=0;

enum canState
{
  CAN_SM,
  CAN_ML,
  CAN_RPY,
  CAN_LC,
  CAN_WAIT,
  CAN_NUM_CH
}canstate;

static CANTxFrame txmsg;
static CANRxFrame rxmsg;

static const CANConfig cancfg = {
  CAN_MCR_ABOM,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(5)
};

struct can_instance {
  CANDriver     *canp;
  uint32_t      led;
};

static WORKING_AREA(wifi_send_wa, 1024);
static msg_t wifi_send_task(void * p) {
  chRegSetThreadName("wifi send");
  char sajt[] = "sajt3";
  wifi_send(buffer, BUFFER_SIZE);
}

static WORKING_AREA(can_rx_wa, 1024);
static msg_t can_rx(void *p) {
  EventListener el;
  int i;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while(!chThdShouldTerminate()) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK) {

      int curr_pos = packet_counter * FRAME_SIZE;

      buffer[curr_pos + 0] = rxmsg.EID >> 8;
      buffer[curr_pos + 1] = (uint8_t)rxmsg.EID;
      buffer[curr_pos + 2] = 0x00;
      buffer[curr_pos + 3] = rxmsg.data8[0];
      buffer[curr_pos + 4] = rxmsg.data8[1];
      buffer[curr_pos + 5] = rxmsg.data8[2];
      buffer[curr_pos + 6] = rxmsg.data8[3];
      buffer[curr_pos + 7] = rxmsg.data8[4];
      buffer[curr_pos + 8] = rxmsg.data8[5];
      buffer[curr_pos + 9] = rxmsg.data8[6];
      buffer[curr_pos + 10] = rxmsg.data8[7];
      buffer[curr_pos + 11] = 0x00;

      packet_counter++;

      if(packet_counter > PACKET_SIZE)
      {
        packet_counter = 0;
        //chThdCreateStatic(wifi_send_wa, sizeof(wifi_send_wa), NORMALPRIO, wifi_send_task, NULL);
        wifi_send(buffer, BUFFER_SIZE);
        clear_buffer();
      }
    }
  }
  chEvtUnregister(&CAND1.rxfull_event, &el);

  return 0;
}



static WORKING_AREA(can_tx_wa, 256);
static msg_t can_tx(void * p) {

  (void)p;
  chRegSetThreadName("transmitter");
  txmsg.IDE = CAN_IDE_EXT;
  txmsg.EID = 0x01234567;
  txmsg.RTR = CAN_RTR_DATA;
  txmsg.DLC = 8;
  txmsg.data32[0] = 0x55AA55AA;
  txmsg.data32[1] = 0x00FF00FF;

  //canTransmit(&CAND1, &txmsg, MS2ST(100));

  while (!chThdShouldTerminate()) {
    canTransmit(&CAND1, CAN_ANY_MAILBOX ,&txmsg, MS2ST(100));
    chThdSleepMilliseconds(100);
  }
  return 0;
}

void can_commInit(void){
  clear_buffer();

  canStart(&CAND1, &cancfg);


   //chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
   chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO, can_rx, NULL);

   char sajt[] = "sajt2\r\n";
   wifi_send(&sajt, 6);
}

void clear_buffer(){
  int i;
  for(i = 0; i < BUFFER_SIZE; i++)
    buffer[i] = 0x00;
}


void can_buffer(BaseSequentialStream *chp, int argc, char *argv[]) {
  chprintf(chp, "\x1B\x63");
  chprintf(chp, "\x1B[2J");
  while (chnGetTimeout((BaseChannel *)chp, TIME_IMMEDIATE) == Q_TIMEOUT) {
    chprintf(chp, "\x1B\x63");
    chprintf(chp, "\x1B[2J");
    chprintf(chp, "CAN BUFFER: %d\r\n", packet_counter);

    int i;
    for(i = 0; i < PACKET_SIZE; i++)
    {
      int curr_pos = i * FRAME_SIZE;
      chSysLock();
      chprintf(chp, "%2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x %2x\r\n",
               buffer[curr_pos + 0], \
               buffer[curr_pos + 1], \
               buffer[curr_pos + 2], \
               buffer[curr_pos + 3], \
               buffer[curr_pos + 4], \
               buffer[curr_pos + 5], \
               buffer[curr_pos + 6], \
               buffer[curr_pos + 7], \
               buffer[curr_pos + 8], \
               buffer[curr_pos + 9], \
               buffer[curr_pos + 10], \
               buffer[curr_pos + 11]);
      chSysUnlock();
    }
    chThdSleepMilliseconds(200);
  }
}





