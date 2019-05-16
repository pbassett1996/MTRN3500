#ifndef EMBEDDED_OPERATIONS_H
#define EMBEDDED_OPERATIONS_H

#include <sys/io.h>

/*
* For MTRN3500 Students - These methods and their respective signatures must not be changed. If
* they are and human intervention is required then marks will be deducted. You are more than
* welcome to add private member variables and private methods to the provided classes, or create
* your own underlying classes to provide the requested functionality as long as all of the
* functionality for the interfaces can be accessed using the methods in the provided classes.
*/

class EmbeddedOperations {
public:
	EmbeddedOperations() {}
	~EmbeddedOperations() {}

	virtual unsigned char inb(unsigned short int port) {
		return ::inb(port);
	}

	virtual void outb(unsigned char value, unsigned short int port) {
		::outb(value, port);
	}

	virtual int ioperm(unsigned long from, unsigned long num, int turn_on) {
		return ::ioperm(from, num, turn_on);
	}
};

#endif // EMBEDDED_OPERATIONS_H
