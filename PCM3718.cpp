#ifndef PCM3718_MODULE_CPP
#define PCM3718_MODULE_CPP

#include <stdint.h>
#include <sys/io.h>
#include <iostream>
#include <iomanip>

#include "PCM3718.h"


//Parameterized Constructor (1)
EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations *eops, uint32_t base_addr) {

	//Set the value of the address at the private variable eops
	this->eops = eops;

	//Set the value of the address at the private variable BASE
	this->BASE = base_addr;

	//Set the value of the address at the private variable RANGE
	this->RANGE = 0; //Default Range +-5V

	//Ask permission to use the registers
	if (eops->ioperm(this->BASE, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}
}

//Parameterized Constructor (2)
EmbeddedDevice::PCM3718::PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint32_t analog_range) {
	
	//Set the value of the address at the private variable eops
	this->eops = eops;

	//Set the value of the address at the private variable BASE
	this->BASE = base_addr;
	
	//Set the value of the address at the private variable RANGE
	this->RANGE = analog_range;

	//Ask permission to use the registers
	if (eops->ioperm(this->BASE, 16, 1) != 0) {
		std::cout << "fail perm" << std::endl;
	}
}

//read in the high and low byte input, combine them and return the result
uint16_t EmbeddedDevice::PCM3718::digitalInput() {
	uint16_t input;
	
	input = eops->inb(this->BASE + 11); //Readig HighByte
	input = ((input << 8) & 0xFF00) | eops->inb(this->BASE + 3); //Concatenate with LowByte

	return input;

}

//return the byte as determined by the input argument (1 = high byte, 0 = low byte)
uint8_t EmbeddedDevice::PCM3718::digitalByteInput(bool high_byte) {

	if (high_byte) {
		return eops->inb(this->BASE + 11);
	}
	else {
		return eops->inb(this->BASE + 3);
	}

}

//Return the value of the bit as determined by the input argument (value between 0-15)
bool EmbeddedDevice::PCM3718::digitalBitInput(uint8_t bit) {

	//Check whether bit is in range
	if (bit > 15) {
		std::cout << "Error - Bit Value not between 0-15" << std::endl;
		exit(0);
	}
	
	//Check & return if bit is on
	return ((digitalInput() & (1<<bit)) != 0);
}


//Output the provided value to both low and high channels
void EmbeddedDevice::PCM3718::digitalOutput(uint16_t value) {

	eops->outb(value, this->BASE + 3); //Output to low
	eops->outb((value >> 8) & 0x00FF, this->BASE + 11); //Remove unwanted garbage & Output  to High 
}

//Output the provided value to the channel determined by the boolean input (1 = high byte, 0 = low byte)
void EmbeddedDevice::PCM3718::digitalByteOutput(bool high_byte, uint8_t value) {

	if (high_byte) {
		eops->outb(value, this->BASE + 11);
	}
	else {
		eops->outb(value, this->BASE + 3);
	}

}

//Change the range for analog input to be the provided rangeCode (look in manual for range definitions)
void EmbeddedDevice::PCM3718::setRange(uint32_t new_analog_range) {

	this->RANGE = new_analog_range;

}

//Receive the input in the analog channel provided, convert it to a voltage (determined by the setRange) and return it
double EmbeddedDevice::PCM3718::analogInput(uint8_t channel) const {

	uint8_t LoByte, HiByte;
	uint16_t value;

	eops->outb(channel | (channel<<4),this->BASE + 2);

	//Remove unwanted garbage & set Range (only lower 4 bits used in register)
	eops->outb(this->RANGE & 0x0F, this->BASE + 1);

	//Set Channel - start channel is low nibble and stop channel is high nible
	//eops->outb(channel | (channel<<4),this->BASE + 2);
	//usleep(100);

	//clear interrupts
	eops->outb(0x00, this->BASE + 0x08);

	//Trigger Conversion - write any value to BASE
	eops->outb(0x00, this->BASE);

	//Wait for conversion to complete
	//8th bit in register BASE+8 is 1 if it is still converting
	while (eops->inb(this->BASE + 0x08) & 0x80);

	//Read data
	LoByte = eops->inb(this->BASE);
	HiByte = eops->inb(this->BASE + 0x01);
	
	value = (LoByte >> 8)|(HiByte << 4);

	//Convert to voltage
	/*switch (this->RANGE) {
	case 0:
		return value*(10.0/4096.0) - 5; break;
	case 1:
		return value*(5.0 / 4096.0) - 2.5; break;
	case 2:
		return value*(2.5 / 4096.0) - 1.25; break;
	case 3:
		return value*(1.25 / 4096.0) - 0.625; break;
	case 4:
		return value*(10.0 / 4096.0); break;
	case 5:
		return value*(5.0 / 4096.0); break;
	case 6:
		return value*(2.5 / 4096.0); break;
	case 7:
		return value*(1.25 / 4096.0); break;
	case 8:
		return value*(20.0 / 4096.0)-10.0; break;
	}*/

	switch (this->RANGE) {
	case 0:
		return value*(10.0 / 4096.0) - 5; break;
	case 1:
		return value*(1.0 / 4096.0) - 0.5; break;
	case 2:
		return value*(0.1 / 4096.0) - 0.05; break;
	case 3:
		return value*(0.01 / 4096.0) - 0.0005; break;
	case 4:
		return value*(10.0 / 4096.0); break;
	case 5:
		return value*(1.0 / 4096.0); break;
	case 6:
		return value*(0.1 / 4096.0); break;
	case 7:
		return value*(0.01 / 4096.0); break;
	case 8:
		return value*(20.0 / 4096.0) - 10.0; break;
	case 9:
		return value*(2.0 / 4096.0) - 1.0; break;
	case 10:
		return value*(0.2 / 4096.0) - 0.1; break;
	case 11:
		return value*(0.02 / 4096.0) - 0.01; break;
	}

}

std::ostream& EmbeddedDevice::operator<<(std::ostream& output, const PCM3718& pcm) {
	std::output << "channel 0:" << std::setprecision(2) << pcm.analogInput(0) << "\tchannel 1:" << std::setprecision(2) << pcm.analogInput(1);
	return output;
}


//Destructor
EmbeddedDevice::PCM3718::~PCM3718() {

}




#endif // PCM3718_MODULE_CPP
