#pragma once
#include "Nucleus.h"
#include "Event.h"

class Encoder
{
	Nucleus *nucleus;
public:
	Encoder(Nucleus *nucleus);
	~Encoder(void);
	void post(Event *e);
	Event *get(void);
};
