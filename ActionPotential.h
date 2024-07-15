#pragma once
#include <stdio.h>
#include "stdafx.h"
#include "Process.h"
#include "NNComponent.h"

class Global;
extern Global *globalObject;

class ActionPotential: public NNComponent
{
private:    
	ActionPotential(Process *p);
public:
	~ActionPotential(void);
	static ActionPotential *create(Process *p);
	unsigned long owningProcessId;
};
