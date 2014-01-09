/*
 * ydle-data.h
 *
 *  Created on: Jan 3, 2014
 *      Author: denia
 */

#ifndef YDLE_DATA_H_
#define YDLE_DATA_H_

namespace ydle{

// Définie un type de structure Signal_t
struct Frame_t
{
	unsigned char sender;
	unsigned char receptor;
	unsigned char type;
	unsigned char taille;	// data len + crc in BYTES
	unsigned char data[30];
	unsigned char crc;
};


}



#endif /* YDLE_DATA_H_ */
