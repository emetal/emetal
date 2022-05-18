#ifndef ORS_ALG_INTERFACE_H
#define ORS_ALG_INTERFACE_H

#include "orsBase/orsCoreInc.h"

class orsIHelloWorld: public orsIObject
{
public:
	virtual ors_string say() = 0;
	virtual ors_string getName() const {return "Example.Helloworld";}
	virtual void setHelloString(ors_string &str) = 0;
	virtual ors_string getHelloString() = 0;

	ORS_INTERFACE_DEF(orsIObject,"hello");
};

#endif
