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

#include "NNComponent.h"
#include <map>
#include "Region.h"
#include "Server.h"
#include "NeuronProcessor.h"
#include "boost/date_time/posix_time/posix_time.hpp"


class Brain: public NNComponent
{
	Brain(void);
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
		for(unsigned int i=0;i<regions.size();i++)
		{
			ar & regions[i];
		}
	}
public:
	virtual ~Brain(void);
	static Brain *create(bool setToDirty=true);
	static Brain *load(void);
	static Brain *loadFromJSON(void);
	void initializeRandom(void);
	void shutdown(void);

	static Brain *instantiate(long key, size_t len, void *data);
	Tuple *getImage(void);

	void step(void);
	void adjustSynapses(void);
//	void removeDeadAPs(void);
	void removePreviousTimedEvents(void);

	void add(Region *reg);

	void syncpoint(void);
	bool restartpoint(void);
	void report(void);
	void longReport(void);
	std::string getLongReport(void);
	std::string getReport(void);
	void startServer(void);
	void stopServer(void);
	void save(void);
	void exportJSON(void);
	void toJSON(std::ofstream& outstream);
	void excite(int num);
	void startNeuronProcessing(void);
	void stopNeuronProcessing(void);
	void validateAndFormatJSON(void);


	Server networkServer;
	NeuronProcessor neuronProcessor;
	std::vector<long> regions;
	int current_syncpoint;

	long timeAdjust;


};
