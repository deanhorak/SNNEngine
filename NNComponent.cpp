#include "NNComponent.h"
#include "Global.h"

NNComponent::NNComponent(ComponentType type)
{
	this->componentType = type;
//	id = globalObject->nextComponent(type);
	globalObject->componentCounter2[this->componentType]++;	// Add counter to number of objects of this type
	dirty=false;												// Initial state is set to 'dirty'

	if(componentType == ComponentTypeUnknown) 
	{
		std::stringstream ss;
		LOGSTREAM(ss) << "WTF!" << std::endl;
		globalObject->log(ss);
	}
}

NNComponent::~NNComponent(void)
{
	globalObject->componentCounter2[componentType]--;
}

void NNComponent::save(void) 
{
	// override 
}

void NNComponent::commit(void)
{
	// override 
}



