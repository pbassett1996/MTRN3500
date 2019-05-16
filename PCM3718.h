#ifndef PCM3718_MODULE_H
#define PCM3718_MODULE_H

#include <stdint.h>
#include <sys/io.h>
#include <iostream>
#include <iomanip>

#include "EmbeddedOperations.h"

/*
* For MTRN3500 Students - These methods and their respective signatures must not be changed. If
* they are and human intervention is required then marks will be deducted. You are more than
* welcome to add private member variables and private methods to the provided classes, or create
* your own underlying classes to provide the requested functionality as long as all of the
* functionality for the PCM3718 can be accessed using the methods in the provided classes.
*/

namespace EmbeddedDevice {
	class PCM3718 {
	public:
		PCM3718(EmbeddedOperations *eops, uint32_t base_addr);
		PCM3718(EmbeddedOperations *eops, uint32_t base_addr, uint32_t analog_range);
		~PCM3718();

		uint16_t digitalInput();
		//read in the high and low byte input, combine them and return the result

		uint8_t digitalByteInput(bool high_byte);
		//return the byte as determined by the input argument (1 = high byte, 0 = low byte)

		bool digitalBitInput(uint8_t bit);
		//Return the value of the bit as determined by the input argument (value between 0-15)

		void digitalOutput(uint16_t value);
		//Output the provided value to both low and high channels

		void digitalByteOutput(bool high_byte, uint8_t value);
		//Output the provided value to the channel determined by the boolean input (1 = high byte, 0 = low byte)

		void setRange(uint32_t new_analog_range);
		//Change the range for analog input to be the provided rangeCode (look in manual for range definitions)

		double analogInput(uint8_t channel) const;
		//Receive the input in the analog channel provided, convert it to a voltage (determined by the setRange) and return it

		friend std::ostream& operator<<(std::ostream& output, const PCM3718& pcm);
			//Have it output in the following style, with voltages displayed to 2 d.p. "channel 1: <channel 1 voltage>\tchannel 2: <channel 2 voltage>\n"

	private:
		// NOTE: All sys/io function calls must be made through the EmbeddedOperations class
		EmbeddedOperations *eops;
		int BASE;
		uint32_t RANGE;
	};

}


#endif // PCM3718_MODULE_H
