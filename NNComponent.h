/*
 * Proprietary License
 * 
 * Copyright (c) 2024 Dean S Horak
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

#pragma once

#include <fstream>
#include <iostream>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/xml_iarchive.hpp>
#include <boost/archive/xml_oarchive.hpp>
#include "Tuple.h"


enum ComponentType {ComponentTypeUnknown=0, ComponentTypeBrain, ComponentTypeRegion, ComponentTypeNucleus, ComponentTypeColumn, ComponentTypeLayer, 
					ComponentTypeCluster, ComponentTypeNeuron, ComponentTypeAxon, ComponentTypeDendrite, ComponentTypeSynapse, 
					ComponentTypeActionPotential, ComponentTypeTimedEvent};
#define CTYPE_COUNT 13

#define SENSORY_NUCLEUS 1
#define INTER_NUCLEUS 0


class NNComponent
{
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
        ar & componentType;
        ar & id;
	}
public:
	NNComponent(ComponentType type=ComponentTypeUnknown);
	virtual ~NNComponent(void);

	inline bool isDirty(void) {return dirty; };
	inline void setDirty(bool value=true) {dirty = value; };
	virtual void save(void);
	virtual void commit(void);


	ComponentType componentType;
	unsigned long id;
	unsigned long parentId;

private:
	bool dirty;
};

