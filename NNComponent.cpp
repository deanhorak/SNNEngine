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



