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
#include "LuaStack.h"

pthread_mutex_t g_mutexSynchro = PTHREAD_RECURSIVE_MUTEX_INITIALIZER_NP; // Mutex used to prevent reading RF when we Sending something
int g_pinTx = 0;
int g_pinRx = 1;
protocolRF *g_ProtocolRF;

void show_usage(){
	std::cout << "Usage: ydle-rf -s script" << std::endl;
	std::cout << "Example : ydle-rf -s script/init.lua" << std::endl;
	std::cout << "Options: "<< std::endl;
	std::cout << "Mandatory" << std::endl;
	std::cout << " -s : Script (string) " << std::endl;
	std::cout << "Optional:" << std::endl;
	std::cout << " -x : Rx Pin (int) " << std::endl;
	std::cout << " -t : Tx Pin (int) " << std::endl;
}
int main (int argc, char **argv){
	char *script, *receiver, *data_type, *data_value, *count = NULL, *crc = NULL, *tx_pin = NULL, *rx_pin = NULL, *command=NULL;
	int send,rec, dtype, dvalue, cmd;
	int pscript = 0;
	int c;
	opterr = 0;

	while ((c = getopt (argc, argv, "t:x:s:?::h::")) != -1)
		switch (c)
		{
		case 'h':
			show_usage();
			return 0;
			break;
		case 's':
			script = optarg;
			pscript = 1;
			break;
		case 'x':
			rx_pin = optarg;
			break;
		case 't':
			tx_pin = optarg;
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

	if(!pscript){
		std::cout << "A parameter is missing, check your arguments" << std::endl;
		show_usage();
		return 0;
	}

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

	ydle::LuaStack *stack;
	stack = new ydle::LuaStack();

	g_ProtocolRF = new protocolRF(g_pinRx, g_pinTx, stack);

	struct sched_param p;
	p.__sched_priority = sched_get_priority_max(SCHED_RR);
	if (sched_setscheduler(0, SCHED_RR, &p) == -1) {
		perror("Failed to switch to realtime scheduler.");
	}

	stack->runScript(script);

	// Start listen thread
	if (pthread_create(&threadListenRF, NULL, protocolRF::listenSignal,
			g_ProtocolRF) < 0) {
		YDLE_FATAL << "Can't start ListenRF thread";
	}
	YDLE_DEBUG << "Everything is start!";

	while(1){
		sleep(100);
	}

	return 0;
}
