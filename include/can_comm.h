/*
 * can_comm.h
 *
 *  Created on: 2015 m�rc. 10
 *      Author: Rich�rd
 */

#ifndef CAN_COMM_H_
#define CAN_COMM_H_

static msg_t can_tx(void * p);
static msg_t can_rx(void *p);
static msg_t Villog();
void can_commInit(void);



#endif /* CAN_COMM_H_ */
