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

//
//#define BRAINDEMO BrainDemoSimple
//#define BRAINDEMONAME "BrainDemoSimple"
//#define BRAINDEMO BrainDemo
//#define BRAINDEMONAME "BrainDemo"
//#define DB_PATH "../database/"
//#define BRAINDEMO BrainDemo6
//#define BRAINDEMONAME "BrainDemo6"

#define BRAINDEMO BrainDemoTiny
#define BRAINDEMONAME "BrainDemoTiny"

#define DB_PATH "../../../database/"
#define LOGGING_PATH "/mnt/nas8t/SNNLogs/"


enum LayerType {input, output};


//#define BRAINDEMO DetailTest
//#define BRAINDEMONAME "DetailTest"

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
//#include <boost/json.hpp>

//#include <vld.h>


#include "NNComponent.h"
#include "Brain.h"
#include "Region.h"
#include "Nucleus.h"
#include "Column.h"
#include "Layer.h"
#include "Cluster.h"
#include "Neuron.h"
#include "Axon.h"
#include "Dendrite.h"
#include "Synapse.h"
#include "ActionPotential.h"
#include "TimedEvent.h"
#include "CollectionIterator.h"
#include "ComponentDB.h"
#include "Location3D.h"
// Size of timeinterval (# of ms slots)
#define MAX_TIMEINTERVAL_BUFFER_SIZE 10000

// Max offset for new APs 
#define MAX_TIMEINTERVAL_OFFSET 500
// number of threads in pool
#define MAX_THREADPOOL_SIZE 20
// threshold at which we'll divide the work up among MAX_THREADPOOL_SIZE threads
#define THREADPOOL_SLICE_THRESHOLD 400 
// cap on number of APs before slowing down
#define MAX_ACTIVE_ACTIONPOTENTIALS 20000000 // 20 million
// initial axon rate
#define DEFAULT_AXON_RATE 0.01f
// axon rate change step
#define DEFAULT_AXON_RATE_STEP 0.001f
// initial dendrite rate
#define DEFAULT_DENDRITE_RATE 0.01f
// dendrite rate change step
#define DEFAULT_DENDRITE_RATE_STEP 0.001f
// Default beginning dendrite distance
#define DEFAULT_DENDRITE_DISTANCE 100
// Default dendrite distance step
#define DEFAULT_DENDRITE_DISTANCE_STEP 0.3
// weight change rate for STDP
#define DEFAULT_STDP_RATE 1.25f

// time conversion factors for calls requiring microseconds
// Microsecond is 1/1,000,000th of a second
#define MICROSECONDS 1
// Millisecond is 1,000 microseconds
#define MILLISECONDS 1000
// Second is 1,000,000 microseconds 
#define SECONDS 1000000
// resting potential
#define RESTING_POTENTIAL -65.0f
// initial learning rate
#define INITIAL_LEARNING_RATE 10.0f
// maximum synapse weight
#define MAXIMUM_SYNAPSE_WEIGHT 65.0f
// maximum rate for axon
#define MAXIMUM_AXON_RATE 1.0f
// maximum rate for dendrite
#define MAXIMUM_DENDRITE_RATE 1.0f
// enable the growth of new dendrites
#define GROW_DENDRITES false
// polarity
#define EXCITATORY 1.0f
#define INHIBITORY -1.0f


#define LOGSTREAM(s) s.str(""); s.clear(); s 


// #define u_int32_t uint32_t

class Global
{
private:

public:

	boost::mutex xThreadQueue_mutex;
	boost::mutex log_event_mutex;
	boost::mutex neuron_cycle_mutex;
	boost::mutex inference_mutex;

	std::queue<std::string> xThreadQueue;

	bool logResponseMode = false;

	Global(void);
	virtual ~Global(void);
	void increment(void);
	void addTimedEvent(TimedEvent* te);
	long nextComponent(ComponentType type);
	void runInference(void);
	void runLearning(void);
	void cycle(void);
	void cycleNeurons(void);
	void inferenceSlice(std::vector<TimedEvent*>* teVector, size_t start, size_t count, bool display, size_t intervalOffsetValue);
	void flush(void);
	void flushAll(void);
	void shutdown(void);
	void log(char *str);
	void log(std::stringstream &ss);
	void logFiring(Neuron *n,bool firestatus);

	long batchFire(std::vector<Neuron *> *firingNeurons);

	float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
	std::map<int, float> clusterPatterns(const std::vector<std::vector<float>> &patterns, int numClusters, int vectorLength);

	Dendrite *findConnectingDendrite(Neuron *neuronA, Neuron *neuronB);
	long getTotalEvents(void);

	void debug(char *str);
	void debug(std::stringstream &ss);

	std::map<long,Neuron *> firingNeurons;

	std::vector<float> softmax(const std::vector<float>& input);
	std::pair<std::vector<Tuple*>*, std::vector<Tuple*>* > *getSpikes(long now, long interval);
	std::pair<std::vector<Neuron *>*, std::vector<Neuron *>* > *getNeurons(long now, long interval);

	char* allocClearedMemory(size_t count);
	void freeMemory(char *ptr);

	bool componentKeyInRange(unsigned long key);
	bool validTimedEvent(unsigned long id);
	bool validActionPotential(unsigned long id);

	void loadAll(void);

	size_t getTypeIndex(std::string  name);

	// Brain
	void insert(Brain *brain);
	size_t brainSize(void);

	// Region
	void insert(Region *region);
	size_t regionsSize(void);

	// Nucleus
	void insert(Nucleus *nucleus);
	size_t nucleiSize(void);

	// Column
	void insert(Column *column);
	size_t columnsSize(void);

	// Layer
	void insert(Layer *layer);
	size_t layersSize(void);

	// Cluster
	void insert(Cluster *cluster);
	size_t clustersSize(void);

	// Neuron
	void insert(Neuron *neuron);
	size_t neuronsSize(void);

	// Axon
	void insert(Axon *axon);
	size_t axonsSize(void);

	// Dendrite
	void insert(Dendrite *dendrite);
	size_t dendritesSize(void);

	// Synapse
	void insert(Synapse *synapse);
	size_t synapsesSize(void);

	// TimedEvent
	void insert(TimedEvent *timedEvent,int intervalOffset);

	void insertFiring(Neuron *neuron);

	void writeSyncpoint(int sp);
	int readSyncpoint(void);

	std::string getMessage(void);
	void putMessage(std::string);

	void writeFireLog(std::string msg);
	void closeFireLog(void);
	void closeDebugLog(void);

	void writeEventLog(std::string msg);
	void closeEventLog(void);

	void writeStructureLog(std::string msg);
	void closeStructureLog(void);

	void logStructure(void);
	void logStructure(Nucleus *nucleus);

	
// trim from start (in place)
	static inline std::string &ltrim(std::string& s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
			return !std::isspace(ch);
			}));
		return s;
	}

	// trim from end (in place)
	static inline std::string &rtrim(std::string& s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
			return !std::isspace(ch);
			}).base(), s.end());
		return s;
	}

	// trim from both ends (in place)
	static inline std::string &trim(std::string& s) {
		ltrim(s);
		rtrim(s);
		return s;
	}

	// trim from start (copying)
	static inline std::string ltrim_copy(std::string s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrim_copy(std::string s) {

		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static inline std::string trim_copy(std::string s) {
		trim(s);
		return s;
	}


	unsigned long componentBase[CTYPE_COUNT];
	long componentCounter[CTYPE_COUNT];
	long componentCounter2[CTYPE_COUNT];
	long current_timestep;
	boost::posix_time::ptime startRealTime;

	std::map<long, TimedEvent*> allTimedEvents;
	
	std::vector<TimedEvent *> timeIntervalEvents[MAX_TIMEINTERVAL_BUFFER_SIZE];

	ComponentDB<Layer> layerDB;
	ComponentDB<Cluster> clusterDB;
	ComponentDB<Column> columnDB;
	ComponentDB<Nucleus> nucleusDB;
	ComponentDB<Region> regionDB;
	ComponentDB<Brain> brainDB;
	ComponentDB<Neuron> neuronDB;
	ComponentDB<Axon> axonDB;
	ComponentDB<Dendrite> dendriteDB;
	ComponentDB<Synapse> synapseDB;

	std::vector<NNComponent *> deletedComponents;

	boost::asio::thread_pool *workers = NULL;


	int syncpoint;

	Location3D nullLocation;

	std::ofstream *firefile = NULL;
	std::ofstream *firelogfile = NULL;
	std::ofstream *debugfile = NULL;
	std::ofstream *eventlogfile = NULL;
	std::ofstream *structurelogfile = NULL;

	bool setLogFiring = false;
	bool keepWebserverRunning = true;
	bool logEvents = false;

	boost::mutex timestep_mutex;
	boost::mutex actionpotential_mutex;
	boost::mutex firingNeurons_mutex;
	boost::mutex timedevents_mutex;
	boost::mutex debug_mutex;

	std::vector <boost::mutex *> teVector_mutex;

	boost::mutex stdp_mutex;


};

#ifndef noexternglobal
#define noexternglobal 1
#endif

