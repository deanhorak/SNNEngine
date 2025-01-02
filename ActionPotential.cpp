/*
 * Proprietary License
 * 
 * Copyright (c) 2024-2025 Dean S Horak
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information of Dean S Horak ("Proprietary Information").
 * You shall not disclose such Proprietary Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with Dean S Horak.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are not permitted
 * without express written permission from Dean S Horak.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

#include "Global.h"
#include "ActionPotential.h"
#include "Axon.h"
#include "Dendrite.h"

extern long REFACTORY_PERIOD;

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
long ActionPotential::computeOffset(float position, float rate) 
{
	long offset = position + (long)((position * rate) * AP_OFFSET_RATE);
	if(offset>MAX_TIMEINTERVAL_OFFSET)
		offset = MAX_TIMEINTERVAL_OFFSET;


	// DSH Hack for experiment
	offset = offset / 50;  // cut the response time down a bit

	//std::cout << "Computed offset "	<< offset << " from position " << position << ", rate " << rate << std::endl;
	return offset;
}
