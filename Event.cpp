#include "Event.h"

Event::Event(size_t size)
{
	sampleSize = size;
	samples = new char[size];
}

Event::~Event(void)
{
}
