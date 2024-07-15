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
