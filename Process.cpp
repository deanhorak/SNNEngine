#include "Global.h"
#include "Process.h"

// Settable externs
extern long FIRING_WINDOW;
extern long PROPAGATION_DELAY_MICROSECONDS;
extern float DECAY_FACTOR;
extern long REFACTORY_PERIOD;
extern float WEIGHT_GRADATION;
extern float RATE_GRADATION;


Process::Process(ComponentType type): 
	NNComponent(type)
{
}

Process::~Process(void)
{
}

void Process::setRate(float value) 
{ 
	float oldrate = rate;
	if(rate != value) 
	{
		boost::mutex::scoped_lock  amx(process_mutex);

		rateUpdateCounter++;

		//		std::cout << "Synapse::setWeight Synapse: " << id << " adjusting weight from " << weight << " to " << value << std::endl;

		float diff = rate - value;
		//dampen the actual value so that the weight changes more slowly over time 
		// calculate weightFactor, which should initially be a value of 1, followed by 0.5, followed by 0.333 etc
		float rateFactor = (RATE_GRADATION / rateUpdateCounter) / RATE_GRADATION;
		// take the difference in value and weight it based on weightFactor
		float delta = diff * rateFactor;

		float adjustedRate = rate + delta; // calc the adjusted weight 

		//std::cout << "Process " << this->id << " rate chg from " << this->rate << " to " << value << std::endl;
		rate = adjustedRate;

		setDirty(true); 

//  log upstream rather than here becuasue it has more info (eg pre or post)
		if(globalObject->logEvents) 
		{
			long thisId = 0;
			std::stringstream ss;
			if(this->id >= globalObject->componentBase[ComponentTypeAxon] && this->id < globalObject->componentBase[ComponentTypeDendrite])
			{ // axon
				Axon *a = (Axon *)this;
				thisId = a->id;
				ss << "axon__change_rate_change: component=" << thisId << ", oldrate=" << oldrate << ", newrate=" << rate << ", neuron=" << a->neuronId;
			}
			else
			{ // dendrite
				Dendrite *d = (Dendrite *)this;
				thisId = d->id;

				std::stringstream ss;
				ss << "dendrite_change_rate_change: component=" << thisId << ", oldrate=" << oldrate << ", newrate=" << rate << ", presynapticneuron=" << d->getPreSynapticNeuronId() << ", postsynapticneuron=" << d->getPostSynapticNeuronId();

			}
			globalObject->writeEventLog(ss.str().c_str());
		}
	}
}