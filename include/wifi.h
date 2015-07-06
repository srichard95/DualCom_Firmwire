/*
 * wifi.h
 *
 *  Created on: 2015 jún. 19
 *      Author: Richárd
 */

#ifndef INCLUDE_WIFI_H_
#define INCLUDE_WIFI_H_

void init_wifi();
void wifi_debug(BaseSequentialStream *chp, int argc, char *argv[]);
void send_at(BaseSequentialStream *chp, int argc, char *argv[]);
void read_buffer_debug(BaseSequentialStream *chp, int argc, char *argv[]);
void wifi_send(char *data[], int length);

#endif /* INCLUDE_WIFI_H_ */
