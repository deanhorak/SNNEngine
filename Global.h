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

#pragma once

//
// You can define your MODEL_IMPLEMENTATION_CLASS here if needed.
// #define MODEL_IMPLEMENTATION_CLASS BrainDemoTiny

#define MODEL_IMPLEMENTATION_CLASS BrainDemoTiny

#define DB_PATH globalObject->getDBPath()
#define LOGGING_PATH "/mnt/nas8t/SNNLogs/"

enum LayerType {input, output};

// Default simulation parameters
#define MAX_TIMEINTERVAL_BUFFER_SIZE 100000
#define MAX_TIMEINTERVAL_OFFSET 5000
#define MAX_THREADPOOL_SIZE 20
#define THREADPOOL_SLICE_THRESHOLD 1000 

// cap on number of APs before slowing down - twice as many as synapses
#define MAX_ACTIVE_ACTIONPOTENTIALS (globalObject->synapsesSize()*2) 

// rate for computing offset offset = position + (long)((position * rate) * AP_OFFSET_RATE);
#define AP_OFFSET_RATE 0.01f


// Conversions
#define MICROSECONDS 1
#define MILLISECONDS 1000
#define SECONDS 1000000

// Neural parameters
#define RESTING_POTENTIAL -65.0f
#define INITIAL_THRESHOLD -50.0f
#define INITIAL_LEARNING_RATE 10.0f
#define MAXIMUM_SYNAPSE_WEIGHT 65.0f
#define MINIMUM_SYNAPSE_WEIGHT -65.0f
#define MAXIMUM_AXON_RATE 1.0f
#define MAXIMUM_DENDRITE_RATE 1.0f
#define MINIMUM_MEMBRANE_POTENTIAL -65.f
#define MAXIMUM_MEMBRANE_POTENTIAL 130.f

// Default rates and distances
#define DEFAULT_AXON_RATE 1000.0f
#define DEFAULT_AXON_DISTANCE 1000.0f
#define DEFAULT_AXON_DISTANCE_STEP 0.2f
#define DEFAULT_DENDRITE_RATE 1.0f
#define DEFAULT_DENDRITE_DISTANCE 1000.0f
#define DEFAULT_DENDRITE_DISTANCE_STEP 0.2f
#define DEFAULT_STDP_RATE 1.25f
#define DEFAULT_SYNAPSE_POSITION 500.0

// STDP tuning constants (new)
#define A_PLUS 1.0f
#define A_MINUS -0.8f
#define TAU_PLUS 10.0f
#define TAU_MINUS 10.0f

// Homeostatic plasticity parameters (new)
#define HOMEOSTASIS_TARGET_RATE 0.1f
#define HOMEOSTASIS_ADJUST_FACTOR 0.001f

// Membrane potential decay (leaky integrate-and-fire) (new)
#define LEAK_FACTOR 0.99f

// Synaptic scaling factor applied after STDP (new)
#define SYNAPTIC_SCALING_FACTOR 1.0f

// Growth of dendrites disabled for now
#define GROW_DENDRITES false

#define EXCITATORY 1.0f
#define INHIBITORY -1.0f

#include <stdio.h>
#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <queue>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <cmath>
#include <fstream>

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
#include "Range.h"
#include "FloatTuple.h"

#define STALL_OVERHEAD 100

extern class Global *globalObject;


class Global
{
private:
    long timeEventsCounter = 0L;
    std::map<unsigned long,Neuron *> firedNeurons;

public:
    boost::mutex xThreadQueue_mutex;
    boost::mutex log_event_mutex;
    boost::mutex neuron_cycle_mutex;
    boost::mutex inference_mutex;
    boost::mutex timestamp_mutex;
    boost::mutex firedneuron_mutex;

    std::string modelName;
    std::string dbPath;

    std::queue<std::string> xThreadQueue;

    bool logResponseMode = false;

    std::string lastImageString;

    Global(std::string inDBPath, std::string inModelName);
    virtual ~Global(void);

    std::string getModelName(void) { return modelName; };
    void setModelName(std::string inModelName) { modelName = inModelName; };
    std::string getDBPath() { return dbPath; };
    void setDBPath(std::string inDBPath) { dbPath = inDBPath; };

    void step(void);
    void increment(void);
    void addTimedEvent(TimedEvent* te);
    long nextComponent(ComponentType type);
    void runInference(void);
    void runLearning(void);
    void cycle(void);
    void inferenceSlice(std::vector<TimedEvent*>* teVector, size_t start, size_t count, bool display, size_t intervalOffsetValue);
    void flush(void);
    void flushAll(void);
    void shutdown(void);
    void log(char *str);
    void log(std::stringstream &ss);
    void logFiring(Neuron *n,bool firestatus);
    void writeCounters(void);
    void readCounters(void);

    Range batchFire(std::vector<Neuron *> *firingNeurons, long waitPeriod=0);
    Range batchLearn(std::vector<Neuron *> *inputNeurons, std::vector<Neuron *> *outputNeurons, std::string nucleusName);
    float evaluateResponse(std::vector<Neuron *> *outputNeurons,std::vector<Neuron *> *nucleusNeurons);
    long getCurrentTimestamp(void);
    float euclideanDistance(const std::vector<float>& a, const std::vector<float>& b);
    std::map<int, float> clusterPatterns(const std::vector<std::vector<float>> &patterns, int numClusters, int vectorLength);

    Dendrite *findConnectingDendrite(Neuron *neuronA, Neuron *neuronB);
    long getTotalEvents(void);
    long getAllTotalEvents(void);

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

    // Insert methods
    void insert(Brain *brain);
    size_t brainSize(void);
    void insert(Region *region);
    size_t regionsSize(void);
    void insert(Nucleus *nucleus);
    size_t nucleiSize(void);
    void insert(Column *column);
    size_t columnsSize(void);
    void insert(Layer *layer);
    size_t layersSize(void);
    void insert(Cluster *cluster);
    size_t clustersSize(void);
    void insert(Neuron *neuron);
    size_t neuronsSize(void);
    void insert(Axon *axon);
    size_t axonsSize(void);
    void insert(Dendrite *dendrite);
    size_t dendritesSize(void);
    void insert(Synapse *synapse);
    size_t synapsesSize(void);

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

    void writeFirePatternLog(std::string msg);
    void closeFirePatternLog(void);

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

    inline void incrementTimedEventsCounter(void) {
        boost::mutex::scoped_lock amx1(timeeventscounter_mutex);
        timeEventsCounter++;
    }

    inline void decrementTimedEventsCounter(void) {
        boost::mutex::scoped_lock amx1(timeeventscounter_mutex);
        timeEventsCounter--;
    }

    inline long getTimedEventsCounter(void) {
        boost::mutex::scoped_lock amx1(timeeventscounter_mutex);
        return timeEventsCounter;
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

    boost::asio::thread_pool *workers;

    int syncpoint;

    Location3D nullLocation;

    std::ofstream *firefile;
    std::ofstream *firelogfile;
    std::ofstream *debugfile;
    std::ofstream *eventlogfile;
    std::ofstream *structurelogfile;
    std::ofstream *firePatternlogfile;

    bool setLogFiring;
    bool keepWebserverRunning;
    bool logEvents;

    boost::mutex timestep_mutex;
    boost::mutex actionpotential_mutex;
    boost::mutex firingNeurons_mutex;
    boost::mutex timedevents_mutex;
    boost::mutex debug_mutex;
    boost::mutex firingPatterns_mutex;
    boost::mutex stdp_mutex;
    boost::mutex timeeventscounter_mutex;
    boost::mutex batchfire_mutex;

    std::vector <boost::mutex *> teVector_mutex;

    std::string latestOutputTarget;
    unsigned char lastBuffer[28*28];
    Neuron *lastFiredNeuron;
    Brain* brain;

    // New method to apply global homeostasis
    void applyGlobalHomeostasis();
    void applyLocalHomeostasis(std::vector<Neuron *> *neurons);
};



