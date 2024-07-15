#include "Global.h"
#include "ActionPotential.h"
#include "Axon.h"
#include "Dendrite.h"


ActionPotential::ActionPotential(Process *p): 
	NNComponent(ComponentTypeActionPotential)
{
	owningProcessId = p->id;
	this->id = globalObject->nextComponent(ComponentTypeActionPotential);
}

ActionPotential::~ActionPotential(void)
{
}

ActionPotential *ActionPotential::create(Process *p)
{
	ActionPotential *a = new ActionPotential(p);
	return a;
}

// Compute the offset into the future (in ms) from current position along axon and speed (rate). 
int computeOffset(float position, float rate) 
{
	int offset = (int)(position * rate);
	return offset;
}
