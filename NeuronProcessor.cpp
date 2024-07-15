#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <cstring> // For strerror() and related functions
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "Global.h"

#include "NeuronProcessor.h"

extern boost::mutex timestep_mutex;


// Settable externs
extern long FIRING_WINDOW;
extern long PROPAGATION_DELAY_MICROSECONDS;
extern float DECAY_FACTOR;
extern long REFACTORY_PERIOD;
extern float WEIGHT_GRADATION;
extern float RATE_GRADATION;

NeuronProcessor::NeuronProcessor(void)
{
    keepRunning = true;
}

NeuronProcessor::~NeuronProcessor(void)
{
}

void NeuronProcessor::doWork(void)
{

    long start = globalObject->componentBase[ComponentTypeNeuron];
    long end = globalObject->componentCounter[ComponentTypeNeuron];
    for(long x = start;x<end;x++)
    {
        Neuron *thisNeuron = globalObject->neuronDB.getComponent(x);
        if (thisNeuron != NULL)
        {
            if (thisNeuron->firing)
            { // If we are firing, check to see if refactory period is exceeded
                if (globalObject->current_timestep - thisNeuron->lastfired > FIRING_WINDOW)
                {
                    thisNeuron->setFiring(false);
                    thisNeuron->potential = RESTING_POTENTIAL;
                }
            }
        }
    }
/*
   	if(globalObject->logEvents) 
	{	
		std::stringstream ss;
		ss << "Brain_step: timestep=" << globalObject->current_timestep << ", cycle=runLearning";
		globalObject->writeEventLog(ss.str().c_str());
	}
*/
	globalObject->runLearning();
/*
   	if(globalObject->logEvents) 
	{	
		std::stringstream ss;
		ss << "Brain_step: timestep=" << globalObject->current_timestep << ", cycle=runLearning_complete";
		globalObject->writeEventLog(ss.str().c_str());
	}
*/
    //    globalObject->cycleNeurons();
/*
   	if(globalObject->logEvents) 
	{	
		std::stringstream ss;
		ss << "Brain_step: timestep=" << globalObject->current_timestep << ", cycle=cycleNeurons_complete";
		globalObject->writeEventLog(ss.str().c_str());
	}
*/
}

int NeuronProcessor::waitThread(void)
{
    int ret = waitThreadWorker();
    return ret;
}

int NeuronProcessor::waitThreadWorker(void)
{

    pid_t tid = syscall(SYS_gettid);
    std::cout << "NeuronProcessor thread is " << tid << std::endl;

    while (keepRunning)
    {
        doWork();
    }
}
void NeuronProcessor::start(void)
{
    boost::thread t(&NeuronProcessor::waitThread, this);
    t.detach(); // Don't Wait for the new thread to finish execution
}

void NeuronProcessor::stop(void)
{
    keepRunning = false;
}
