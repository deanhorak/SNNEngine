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

