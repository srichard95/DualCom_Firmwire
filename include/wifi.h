/*
 * wifi.h
 *
 *  Created on: 2015 j�n. 19
 *      Author: Rich�rd
 */

#ifndef INCLUDE_WIFI_H_
#define INCLUDE_WIFI_H_

void init_wifi();
void wifi_debug(BaseSequentialStream *chp, int argc, char *argv[]);
void send_at(BaseSequentialStream *chp, int argc, char *argv[]);

#endif /* INCLUDE_WIFI_H_ */