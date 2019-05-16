#ifndef DAC06_MODULE_CPP
#define DAC06_MODULE_CPP

#include <stdint.h>
#include <sys/io.h>
#include <iostream>

#include "DAC06.h"

//Paramterized Constructor
EmbeddedDevice::DAC06::DAC06(EmbeddedOperations *eops, uint32_t base_addr) {

	//Set the value of the address at the private variable eops
	this->eops = eops;

	//Set the value of the address at the private variable BASE
	this->BASE = base_addr;

	//Ask permission to use the registers
	if (eops->ioperm(this->BASE, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}
}

void EmbeddedDevice::DAC06::analogOutputRaw(uint8_t channel, uint16_t value) {
	
	//Check if channel in range
	if (channel > 5) {
		std::cout << "fail output - Channel not in range (0-5)" << std::endl;
		exit(0);
	}

	unsigned char LowByte = value;
	eops->outb(LowByte, this->BASE + (channel*2)); //Output to lower register

	unsigned char HiByte = (value >> 8) & 0x0F; //Shift and remove garbage
	eops->outb(HiByte, this->BASE + (channel*2+1)); //Output to higher register


	//Simulatenous intiation
	eops->inb(this->BASE + (channel*2)); //Read anything from either register

}

void EmbeddedDevice::DAC06::analogOutputVoltage(uint8_t channel, double desired_voltage) {
	
	//Convert voltage to CODE value
	uint16_t value = static_cast<uint16_t>((desired_voltage + 5)/10.0 * 4096); //Static cast to convert from double to unisgned short int


	analogOutputRaw(channel, value); 


}

EmbeddedDevice::DAC06::~DAC06() {

}

#endif // DAC06_MODULE_CPP
