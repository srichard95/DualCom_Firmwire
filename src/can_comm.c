/*
 * can_comm.c

 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */
#include <stdio.h>
#include <string.h>
#include "ch.h"
#include "hal.h"
#include "chprintf.h"
#include "can_comm.h"
#include "console.h"
#include "wifi.h"

static CANTxFrame txmsg;
static CANRxFrame rxmsg;

static const CANConfig cancfg = {
  CAN_MCR_ABOM,
  CAN_BTR_SJW(0) | CAN_BTR_TS2(1) |
  CAN_BTR_TS1(8) | CAN_BTR_BRP(5)
};


char buffer[BUFFER_SIZE];   //BUFFER for the incoming CAN messages
int packet_counter=0;       //Count the BUFFER's packets
static Semaphore buf_sem;   //Semaphore to lock the BUFFER read/write


/*
 * 40 ms WiFi send task
 * Send the data from the buffer and clear the buffer
 */
static WORKING_AREA(wifi_send_wa, 1024);
static msg_t wifi_send_task(void * p) {
  chRegSetThreadName("wifi send");
  systime_t time;
  time = chTimeNow();
  while(TRUE)
  {
    time += MS2ST(40);
    chSemWait(&buf_sem);

    if(packet_counter > 0)
    {
      int actual_packet_size = packet_counter * FRAME_SIZE;
      wifi_send(buffer, actual_packet_size);
      packet_counter = 0;
      clear_buffer();
    }
    chSemSignal(&buf_sem);
    chThdSleepUntil(time);
  }
  return 0;
}

/*
 * CAN Receiver thread
 * Receive the datas from the CAN bus and pack it to the buffer
 */
static WORKING_AREA(can_rx_wa, 256);
static msg_t can_rx(void *p) {
  EventListener el;

  (void)p;
  chRegSetThreadName("receiver");
  chEvtRegister(&CAND1.rxfull_event, &el, 0);
  while(!chThdShouldTerminate()) {
    if (chEvtWaitAnyTimeout(ALL_EVENTS, MS2ST(100)) == 0)
      continue;
    while (canReceive(&CAND1, CAN_ANY_MAILBOX, &rxmsg, TIME_IMMEDIATE) == RDY_OK) {

      chSemWait(&buf_sem);
      if (packet_counter < PACKET_SIZE)
      {
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
      }else if (packet_counter > PACKET_SIZE)
      {
        //packet_counter = 0;
        //wifi_send(buffer, BUFFER_SIZE);
        //clear_buffer();
      }
      rxmsg.EID = 0x00;
      chSemSignal(&buf_sem);


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

/*
 * Init CAN communication
 */
void can_commInit(void){
  clear_buffer();               //Clear the buffer
  chSemInit(&buf_sem, 1);       //Init the Semaphore
  canStart(&CAND1, &cancfg);    //Start CAN driver

  chThdCreateStatic(can_rx_wa, sizeof(can_rx_wa), NORMALPRIO, can_rx, NULL);                //Start can Receiver thread
  chThdCreateStatic(wifi_send_wa, sizeof(wifi_send_wa), NORMALPRIO, wifi_send_task, NULL);  //Start WiFi sender task

  //chThdCreateStatic(can_tx_wa, sizeof(can_tx_wa), NORMALPRIO + 7, can_tx, NULL);
}
/*
 * Fill the buffer with null;
 */
void clear_buffer(){
  int i;
  for(i = 0; i < BUFFER_SIZE; i++)
    buffer[i] = 0x00;
}


/*
 * Debug Functions
 */
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
//---------------------------------------------------------------------------





