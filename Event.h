#pragma once

#include <stddef.h>
class Event
{
public:
	Event(size_t size);
	virtual ~Event(void);
	size_t sampleSize;
	char *samples;
};
