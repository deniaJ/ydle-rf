/*
 * main.cpp
 *
 *  Created on: Jan 8, 2014
 *      Author: denia
 */

#include <thread>
#include <cstring>
#include <signal.h>
#include <cstdarg>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <wiringPi.h>
#include "logging.h"
#include "protocolRF.h"


// global pin used for emit
int g_pinTx = 1;
// global pin used for receive
int g_pinRx = 0;
protocolRF *g_ProtocolRF;

void show_usage(){
	std::cout << "Usage: ydle-rf -s [Sender] -r [Receiver] -d [Data type] -v [Data Value]" << std::endl;
	std::cout << "Example : ydle-rf -s 1 -r 2 -d 4 -v 10" << std::endl;
	std::cout << "Options: "<< std::endl;
	std::cout << "Mandatory" << std::endl;
	std::cout << " -s : Sender (integer) " << std::endl;
	std::cout << " -r : Receiver (integer) " << std::endl;
	std::cout << " -m : Command (integer) " << std::endl;
	std::cout << " -d : Data type (integer, value: 1, 2, 3, 4, 5) " << std::endl;
	std::cout << " -v : Data Value (integer) " << std::endl;
	std::cout << "Optional" << std::endl;
	std::cout << " -t : Transmiter pin (integer) " << std::endl;
	std::cout << " -x : Receptor pin (integer) " << std::endl;
	std::cout << " -n : Send n packet" << std::endl;
	//std::cout << " -c : send false crc " << std::endl;
}

int main (int argc, char **argv){
	char *sender, *receiver, *data_type, *data_value, *count = NULL, *crc = NULL, *tx_pin = NULL, *rx_pin = NULL, *command=NULL;
	int send,rec, dtype, dvalue, cmd;
	int c;
	int psender = 0, preciever = 0, pdtype = 0, pdvalue = 0;
	opterr = 0;

	while ((c = getopt (argc, argv, "m:t:x:s:r:d:v:c:n:?::h::")) != -1)
		switch (c)
		{
		case 'h':
			show_usage();
			return 0;
			break;
		case 's':
			sender = optarg;
			send = atoi(sender);
			psender = 1;
			break;
		case 'r':
			receiver = optarg;
			rec = atoi(receiver);
			preciever = 1;
			break;
		case 'd':
			data_type = optarg;
			dtype = atoi(data_type);
			pdtype = 1;
			break;
		case 'm':
			command = optarg;
			cmd = atoi(command);
			break;
		case 'v':
			data_value = optarg;
			dvalue = atoi(data_value);
			pdvalue = 1;
			break;
		case 'x':
			rx_pin = optarg;
			break;
		case 't':
			tx_pin = optarg;
			break;
		case 'c':
			crc = optarg;
			break;
		case 'n':
			count = optarg;
			break;
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,
						"Unknown option character `\\x%x'.\n",
						optopt);
			return 1;
		default:
			abort ();
		}

	if(!psender || !preciever || !pdtype || !pdvalue || command != NULL){
		std::cout << "A parameter is missing, check your arguments" << std::endl;
		show_usage();
		return 0;
	}
	std::cout << sender;



	pthread_t threadListenRF;
	ydle::InitLogging(ydle::YDLE_LOG_DEBUG, ydle::YDLE_LOG_STDERR);

	if (setuid(0)) {
		perror("setuid");
		return 1;
	}

	if (wiringPiSetup() == -1) {
		YDLE_FATAL << ("Lib Wiring PI not found...");
		return -1;
	}
	if(rx_pin != NULL && tx_pin != NULL){
		g_pinRx = atoi(rx_pin);
		g_pinTx = atoi(tx_pin);
	}
	g_ProtocolRF = new protocolRF(g_pinRx, g_pinTx);

	int max = 1;
	if(count != NULL){
		YDLE_INFO << "Packets count: " << count << std::endl;
		max = atoi(count);
	}

	for(int i = 1; i <= max; i++ ){
		g_ProtocolRF->dataToFrame(rec,send,cmd);
		g_ProtocolRF->addData(dtype, dvalue);
		g_ProtocolRF->transmit();
		YDLE_INFO << "Send trame " << i;
	}

	return 0;
}
