#include <fstream>

// include headers that implement a archive in simple text format
#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include "NNComponent.h"
#include "Axon.h"
#include "Neuron.h"
#include "Synapse.h"
#include "ActionPotential.h"

//BOOST_SERIALIZATION_ASSUME_ABSTRACT(NNComponent)
/*
std::ostream & operator<<(std::ostream &os, const NNComponent &nnc)
{
	return os << "\ncomponentType=" << nnc.componentType << ",componentId=" << nnc.id;
}
*/
/*
std::ostream & operator<<(std::ostream &os, const Axon &ax)
{
	os << "\nneuron = " << ax.neuron->id;
	return os;
}

std::ostream & operator<<(std::ostream &os, const Synapse &synapse)
{
	os << "\nsynapse = " << synapse.id;
	return os;
}

std::ostream & operator<<(std::ostream &os, const ActionPotential &ap)
{
	os << "\nactionpotential = " << ap.id;
	return os;
}
*/