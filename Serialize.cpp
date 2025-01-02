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