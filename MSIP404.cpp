#ifndef MSIP404_MODULE_CPP
#define MSIP404_MODULE_CPP

#include <stdint.h>
#include <sys/io.h>
#include <iostream>

#include "MSIP404.h"

//Paramterized Constructor
EmbeddedDevice::MSIP404::MSIP404(EmbeddedOperations *eops, uint32_t base_addr) {
	
	//Set the value of the address at the private variable eops
	this->eops = eops;

	//Set the value of the address at the private variable BASE
	this->BASE = base_addr;

	//Set the value of the address at the private variable INDEX_BASE
	this->INDEX_BASE = 0x800; //Can only be 0x800

	//Ask permission to use the registers
	if (eops->ioperm(this->BASE, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}

	if (eops->ioperm(this->INDEX_BASE, 2, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}


}

//Reset the channel determined by the input valuev
void EmbeddedDevice::MSIP404::resetChannel(uint8_t channel) {
	
	//Check if channel in range
	if (channel > 7) {
		std::cout << "Fail reset - channel not in range (0-7)" << std::endl;
		exit(0);
	}
	else {

		eops->outb(0, this->BASE + channel); //Reset channel by outputting 0
	}

}

//Read the channel determined by the input value
int32_t EmbeddedDevice::MSIP404::readChannel(uint8_t channel) {

	//Check if channel in range
	if (channel > 7) {
		std::cout << "Fail read - channel not in range" << std::endl;
		exit(0);
	}

	union encoder {
		int32_t i;
		char c[4];
	};
	
	//Read the four bytes of the channel into the encoder
	encoder e;
	e.c[3] = int(eops->inb(this->BASE + channel*4+3));
	e.c[2] = int(eops->inb(this->BASE + channel*4+2));
	e.c[1] = int(eops->inb(this->BASE + channel*4+1));
	e.c[0] = int(eops->inb(this->BASE));

	return e.i; //Return the 32bit value of the channel
}

//Read the index bit for the channel determined by the input value
bool EmbeddedDevice::MSIP404::readIndex(uint8_t channel) {

	//Check if channel in range
	if (channel > 2) {
		std::cout << "fail read - channel not between 0-2" << std::endl;
		exit(0);
	}


	if (channel == 0) {
		return (eops->inb(this->INDEX_BASE) & 0x80); //Index Pulse if Bit 7 is on 
	}
	else if (channel == 1) {
		return (eops->inb(this->INDEX_BASE) & 0x20); //Index Pulse if Bit 5 is on
	}
	else {
		return (eops->inb(this->INDEX_BASE+1) & 0x80); //Index Pulse if Bit 7 is on
	}


}

//Reset all (8) channels
bool EmbeddedDevice::MSIP404::operator!() {
	int i = 0;

	while (i < 8) {
		eops->outb(0, this->BASE + i); //Reset channels by outputting 0 to register
		i++;
	}
	return true;
}

EmbeddedDevice::MSIP404::~MSIP404() {

}


#endif // MSIP404_MODULE_CPP
