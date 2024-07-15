#pragma once

#define ALWAYS_SAVE 1

template <class a_Type> class CachedComponent
{
public:
	CachedComponent(a_Type *component, unsigned long referenceTimestamp)
	{
		this->component = component;
		this->referenceTimestamp = referenceTimestamp;
	}

	virtual ~CachedComponent(void) { }

	a_Type *component;
	unsigned long referenceTimestamp;
};
