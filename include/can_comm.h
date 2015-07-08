/*
 * can_comm.h
 *
 *  Created on: 2015 márc. 10
 *      Author: Richárd
 */

#ifndef CAN_COMM_H_
#define CAN_COMM_H_

#define PACKET_SIZE 200
#define FRAME_SIZE 12
#define BUFFER_SIZE (FRAME_SIZE*PACKET_SIZE)

static msg_t can_tx(void * p);
static msg_t can_rx(void *p);
static msg_t Villog();
void can_commInit(void);
void can_buffer(BaseSequentialStream *chp, int argc, char *argv[]);
static msg_t wifi_send_task(void * p);

#endif /* CAN_COMM_H_ */
