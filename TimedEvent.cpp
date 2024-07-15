#include "TimedEvent.h"
#include "Global.h"

TimedEvent::TimedEvent(long timeSlice):
	NNComponent(ComponentTypeTimedEvent)
{
	slice = timeSlice;
	ap = NULL;
	id = globalObject->nextComponent(ComponentTypeTimedEvent);
}

TimedEvent::~TimedEvent(void)
{
	if(ap!=NULL)
		delete ap;
	//std::cout << "destructing timed event " << id << std::endl;
}

TimedEvent *TimedEvent::create(long timeSlice, Process *p, long synapseId)
{

	TimedEvent *te = new TimedEvent(timeSlice);
	te->ap = ActionPotential::create(p);
	te->synapseId = synapseId;
	globalObject->insert(te,timeSlice);
	return te;
}
