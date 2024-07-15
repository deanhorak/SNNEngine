#pragma once
#include "NNComponent.h"
#include "ActionPotential.h"
#include "Synapse.h"

class TimedEvent: public NNComponent
{
private:
	TimedEvent(long timeSlice);

public:
	virtual ~TimedEvent(void);
	static TimedEvent* create(long timeSlice, Process *p, long synapseId);
	ActionPotential *ap;
	long synapseId;
	long slice;
};
