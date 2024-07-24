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

#include <ctime>
#include "Global.h"

FILE *logFile;
int logMemoryAllocations;

char *ctypes[] = {"ComponentTypeUnknown", "ComponentTypeBrain", "ComponentTypeRegion", "ComponentTypeNucleus", "ComponentTypeColumn", "ComponentTypeLayer",
				  "ComponentTypeCluster", "ComponentTypeNeuron", "ComponentTypeAxon", "ComponentTypeDendrite", "ComponentTypeSynapse",
				  "ComponentTypeActionPotential", "ComponentTypeTimedEvent"};

boost::mutex xThreadQueue_mutex;
std::queue<std::string> threadQueue;

Global::Global(void) : neuronDB(30000), clusterDB(1000), dendriteDB(500000), synapseDB(500000), axonDB(30000)
{
	// Initialize component counters with 100 million gap between
	std::stringstream ss;
	for (size_t i = 0; i < CTYPE_COUNT; i++)
	{
		long index = (long)i;
		componentBase[i] = 100000000 * index;
		componentCounter[i] = componentBase[i];
		componentCounter2[i] = 0;

		LOGSTREAM(ss) << "Component " << ctypes[i] << " assigned values starting at  " << componentCounter[i] << std::endl;
		log(ss);
	}

	current_timestep = 0;
	logMemoryAllocations = 0;
	startRealTime = boost::posix_time::microsec_clock::local_time();

	for (size_t i = 0; i < MAX_TIMEINTERVAL_BUFFER_SIZE; i++)
	{
		timeIntervalEvents[i] = std::vector<TimedEvent *>();
		teVector_mutex.push_back(new boost::mutex());
	}

	syncpoint = -1;

	logFile = NULL;

	workers = new boost::asio::thread_pool(MAX_THREADPOOL_SIZE); // MAX_THREADPOOL_SIZE threads for now
}

Global::~Global(void)
{
}

size_t Global::getTypeIndex(std::string name)
{
	for (int i = 0; i < CTYPE_COUNT; i++)
	{
		if (name == std::string(ctypes[i]))
		{
			return i;
		}
	}
	return -1;
}

char *Global::allocClearedMemory(size_t count)
{
	if (logFile == NULL)
	{
		if (logMemoryAllocations)
			logFile = fopen((std::string(DB_PATH) + BRAINDEMONAME + std::string("/") + std::string("DebugLogFile.txt")).c_str(), "w");
	}

	if (logMemoryAllocations)
		fprintf(logFile, "Allocating %d bytes...\r", (int)count);

	char *mem = new char[count]; // calloc(count, 1);
	memset(mem, '\0', count);

	if (logMemoryAllocations)
		fprintf(logFile, "%d (0x%llx) bytes Allocated\n", (int)count, (unsigned __int64)mem);

	return mem;
}

void Global::freeMemory(char *mem)
{
	unsigned __int64 ptr = (unsigned __int64)mem;
	char *memPtr = (char *)mem;

	if (logFile == NULL)
	{
		if (logMemoryAllocations)
			logFile = fopen((std::string(DB_PATH) + BRAINDEMONAME + std::string("/") + std::string("DebugLogFile.txt")).c_str(), "w");
	}

	if (logMemoryAllocations)
		fprintf(logFile, "Deallocating 0x%llx\r", ptr);

	delete[] memPtr;

	if (logMemoryAllocations)
		fprintf(logFile, "0x%llx deallocated\n", ptr);
}

long Global::getTotalEvents() 
{
	long totalEvents = 0;
	for(size_t i=0;i<MAX_TIMEINTERVAL_BUFFER_SIZE;i++)
	{
		totalEvents += globalObject->timeIntervalEvents[i].size();
	}
	return totalEvents;
}

void Global::increment(void)
{
	boost::mutex::scoped_lock amx(timestep_mutex);
	current_timestep++;
}

long Global::nextComponent(ComponentType type)
{
	return componentCounter[type]++;
}


bool Global::validTimedEvent(unsigned long id)
{
	if (id >= componentBase[ComponentTypeTimedEvent] && id < componentBase[ComponentTypeTimedEvent] + 100000000)
		return true;
	return false;
}

bool Global::validActionPotential(unsigned long id)
{
	if (id >= componentBase[ComponentTypeActionPotential] && id < componentBase[ComponentTypeTimedEvent])
		return true;
	return false;
}

void Global::insert(Brain *brain)
{
	brainDB.insert(brain);
}

size_t Global::brainSize(void)
{
	return brainDB.size();
};


void Global::insert(Region *region)
{
	regionDB.insert(region);
}

size_t Global::regionsSize(void)
{
	return regionDB.size();
};


void Global::insert(Nucleus *nucleus)
{
	nucleusDB.insert(nucleus);
}

size_t Global::nucleiSize(void)
{
	return nucleusDB.size();
};

void Global::insert(Column *column)
{
	columnDB.insert(column);
}

size_t Global::columnsSize(void)
{
	return columnDB.size();
};

void Global::insert(Layer *layer)
{
	layerDB.insert(layer);
}

size_t Global::layersSize(void)
{
	return layerDB.size();
};

void Global::insert(Cluster *cluster)
{
	clusterDB.insert(cluster);
}

size_t Global::clustersSize(void)
{
	return clusterDB.size();
};

void Global::insert(Neuron *neuron)
{
	neuronDB.insert(neuron);
}

size_t Global::neuronsSize(void)
{
	return neuronDB.size();
};


void Global::insert(Axon *axon)
{
	axonDB.insert(axon);
}

size_t Global::axonsSize(void)
{
	return axonDB.size();
};

void Global::insert(Dendrite *dendrite)
{
	dendriteDB.insert(dendrite);
}

size_t Global::dendritesSize(void)
{
	return dendriteDB.size();
};

void Global::insert(Synapse *synapse)
{
	synapseDB.insert(synapse);
}

size_t Global::synapsesSize(void)
{
	return synapseDB.size();
};

void Global::insertFiring(Neuron *neuron)
{
	boost::mutex::scoped_lock amx(firingNeurons_mutex);
	firingNeurons.insert(std::pair<long, Neuron *>(neuron->id, neuron));
}

bool Global::componentKeyInRange(unsigned long key)
{

	if (key >= componentBase[ComponentTypeBrain] && key <= (componentBase[ComponentTypeTimedEvent] + 100000000))
	{
		return true;
	}
	return false;
}

void Global::insert(TimedEvent *timedEvent, int intervalOffset)
{
	size_t intervalOffsetValue = intervalOffset % MAX_TIMEINTERVAL_BUFFER_SIZE;

	boost::mutex::scoped_lock amx(*teVector_mutex[intervalOffsetValue]);
	std::vector<TimedEvent *> *teVector = &globalObject->timeIntervalEvents[intervalOffsetValue];

	teVector->push_back(timedEvent);
}

void Global::cycleNeurons(void)
{
	boost::mutex::scoped_lock amx(neuron_cycle_mutex);

	long start= globalObject->componentBase[ComponentTypeNeuron];
	long end= globalObject->componentCounter[ComponentTypeNeuron];
	for (long i=start;i<end;i++)
	{
		Neuron *n = globalObject->neuronDB.getComponent(i);
		n->cycle();
	}
}

float Global::euclideanDistance(const std::vector<float>& a, const std::vector<float>& b) {
    float sum = 0.0;
    for (size_t i = 0; i < a.size(); ++i) {
        float diff = a[i] - b[i];
        sum += diff * diff;
    }
    return std::sqrt(sum);
}

std::map<int, float> Global::clusterPatterns(const std::vector<std::vector<float>> &patterns, int numClusters, int vectorLength)
{
	// Initialize centroids randomly
	std::vector<std::vector<float>> centroids(numClusters, std::vector<float>(vectorLength, 0.0));
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, patterns.size() - 1);

	for (int i = 0; i < numClusters; ++i)
	{
		centroids[i] = patterns[dis(gen)];
	}

	std::vector<int> assignments(patterns.size(), -1);
	bool changed;

	do
	{
		changed = false;

		// Assign patterns to the nearest centroid
		for (size_t i = 0; i < patterns.size(); ++i)
		{
			float minDist = std::numeric_limits<float>::max();
			int bestCluster = -1;
			for (int j = 0; j < numClusters; ++j)
			{
				float dist = euclideanDistance(patterns[i], centroids[j]);
				if (dist < minDist)
				{
					minDist = dist;
					bestCluster = j;
				}
			}
			if (assignments[i] != bestCluster)
			{
				assignments[i] = bestCluster;
				changed = true;
			}
		}

		// Update centroids
		std::vector<std::vector<float>> newCentroids(numClusters, std::vector<float>(vectorLength, 0.0));
		std::vector<int> counts(numClusters, 0);

		for (size_t i = 0; i < patterns.size(); ++i)
		{
			int cluster = assignments[i];
			for (int j = 0; j < vectorLength; ++j)
			{
				newCentroids[cluster][j] += patterns[i][j];
			}
			counts[cluster]++;
		}

		for (int j = 0; j < numClusters; ++j)
		{
			if (counts[j] > 0)
			{
				for (int k = 0; k < vectorLength; ++k)
				{
					newCentroids[j][k] /= counts[j];
				}
			}
		}

		centroids = newCentroids;

	} while (changed);

	// Calculate pattern probabilities based on distance to centroids
	std::map<int, float> patternProbabilities;
	for (size_t i = 0; i < patterns.size(); ++i)
	{
		int cluster = assignments[i];
		float dist = euclideanDistance(patterns[i], centroids[cluster]);
		float probability = std::exp(-dist); // Convert distance to probability
		patternProbabilities[i] = probability;
	}

	return patternProbabilities;
}

void Global::cycle(void)
{
	runInference();
}

long Global::batchFire(std::vector<Neuron *> *firingNeurons)
{
	boost::mutex::scoped_lock amx1(inference_mutex); // pause inference while we batch fire all neurons
	std::stringstream ss;
	if(globalObject->logEvents) 
	{
		ss << "batch_fire: neurons=";
	}
	std::string sep = "";
	/*
    for (size_t i = 0; i < firingNeurons->size(); i++)
    {
    	Neuron *neuron = (*firingNeurons)[i];
		if(globalObject->logEvents) 
		{
	  		ss << sep << neuron->id;
			sep = ",";
		}
    }
	*/


	long lowestOffset = MAX_TIMEINTERVAL_BUFFER_SIZE;
    for (size_t i = 0; i < firingNeurons->size(); i++)
    {
		
    	Neuron *neuron = (*firingNeurons)[i];
      	long offset = neuron->fire();
		if(lowestOffset > offset) lowestOffset = offset;


		if(globalObject->logEvents) 
		{
		  	ss << sep << neuron->id;
			sep = ",";
		}
    }

	if(globalObject->logEvents) 
	{
		globalObject->writeEventLog(ss.str().c_str());
	}
	return lowestOffset;
}

void Global::runInference(void)
{
	boost::mutex::scoped_lock amx1(inference_mutex);

	size_t intervalOffsetValue = current_timestep % MAX_TIMEINTERVAL_BUFFER_SIZE;


	boost::mutex::scoped_lock amx(*teVector_mutex[intervalOffsetValue]);
	
	std::vector<TimedEvent *> *teVector = &globalObject->timeIntervalEvents[intervalOffsetValue];

	size_t tevSize = (int)teVector->size();

	if (tevSize > 0)
	{
		
		// We now have a vector of a nonzero number of events to process.
		if (tevSize > THREADPOOL_SLICE_THRESHOLD)
		{ // process in slices if it exceeds threshold
			size_t sizeper = tevSize / (size_t)MAX_THREADPOOL_SIZE;

			for (size_t n = 0; n < (size_t)MAX_THREADPOOL_SIZE; ++n)
			{
				size_t per = sizeper;
				size_t startper = n * sizeper;

				if(startper + per > tevSize) 	// If last division not a full slice
					per = tevSize - startper;		

				boost::asio::post(*workers, [&]
								  { inferenceSlice(teVector, startper, per, false, intervalOffsetValue); });
			}
			workers->join();

		}
		else
		{ // if less than or equal to THREADPOOL_SLICE_THRESHOLD, just do the whole thing at once

			inferenceSlice(teVector, 0, tevSize, false, intervalOffsetValue);

		}

	}

}

void Global::runLearning(void)
{

	long learningWindowSize = 100L;

	long learningInterval = current_timestep  - learningWindowSize;
	//long learningIntervalOffsetValue = learningInterval % MAX_TIMEINTERVAL_BUFFER_SIZE;

	std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *> *neurons = getNeurons(learningInterval, learningWindowSize); // Get Neurons firing  within 10 ms
	std::vector<Neuron *> *preNeurons = neurons->first;
	size_t numPre = preNeurons->size();

	std::vector<Neuron *> *postNeurons = neurons->second;
	size_t numPost = postNeurons->size();

	if(numPre == 0 && numPost == 0) // if no pre or post firings, just return
	{
		return;
	}

	long neuronIdStart = globalObject->componentBase[ComponentTypeNeuron];
	long neuronIdEnd = globalObject->componentCounter[ComponentTypeNeuron];
	std::vector<Neuron *> affectedNeurons;
	for(long neuronIndex = neuronIdStart; neuronIndex < neuronIdEnd; neuronIndex++)
	{
		long thisNeuronId = neuronIndex;
		Neuron *thisNeuron = globalObject->neuronDB.getComponent(thisNeuronId);
		if(thisNeuron->lastfired > (unsigned long)(learningInterval - learningWindowSize) ) // pick up all activity of windowsize before and after this learningInterval
		{
			thisNeuron->applySTDP(neurons,learningInterval);
			affectedNeurons.push_back(thisNeuron);
		}
	}

// normalize the potential of all firing neurons
	size_t nCount = affectedNeurons.size();
	if(nCount>0)
	{
		float totalPotential = 0;
		for(size_t i = 0; i < nCount; i++)
		{
			Neuron *thisNeuron = affectedNeurons[i];
			totalPotential += thisNeuron->potential;
		}

		// compute mean
		float mean = totalPotential / (float)nCount;

		// now, subtract the mean from all potentials
		for(size_t i = 0; i < nCount; i++)
		{
			Neuron *thisNeuron = affectedNeurons[i];
			thisNeuron->potential -= mean;
		}
	}


	delete preNeurons;
	delete postNeurons;
	delete neurons;

}

void Global::inferenceSlice(std::vector<TimedEvent *> *teVector, size_t start, size_t count, bool display, size_t intervalOffsetValue)
{
	(void)display;
	(void)intervalOffsetValue;

	for (size_t i = start; i < start + count; i++)
	{
		TimedEvent *te = (*teVector)[i];
		ActionPotential *ap = te->ap;

		if (ap->owningProcessId >= globalObject->componentBase[ComponentTypeAxon] && ap->owningProcessId < globalObject->componentBase[ComponentTypeSynapse])
		{
			if (ap->owningProcessId >= globalObject->componentBase[ComponentTypeDendrite]) // if dendrite range, apply input to the neuron
			{

				// Axon owned synapse
				// An AP on an axon owned synapse decides whether or not to fire the dendrite 
				// (which potentially causes the neuron to fire if the sum of it's synaptic weights > threshold)
				Synapse *s = globalObject->synapseDB.getComponent(te->synapseId);
				s->receiveAP(ap);

//    std::map<int, double> patternProbabilities = neuron.clusterPatterns(patterns, numClusters);				
			}
		}
	}

}

void Global::flush(void)
{
	layerDB.flush();
	clusterDB.flush();
	columnDB.flush();
	nucleusDB.flush();
	regionDB.flush();
	brainDB.flush();
	neuronDB.flush();
	axonDB.flush();
	dendriteDB.flush();
	synapseDB.flush();
}

void Global::flushAll(void)
{
	layerDB.flushAll();
	clusterDB.flushAll();
	columnDB.flushAll();
	nucleusDB.flushAll();
	regionDB.flushAll();
	brainDB.flushAll();
	neuronDB.flushAll();
	axonDB.flushAll();
	dendriteDB.flushAll();
	synapseDB.flushAll();
}

void Global::loadAll(void)
{
	layerDB.loadCache();
	clusterDB.loadCache();
	columnDB.loadCache();
	nucleusDB.loadCache();
	regionDB.loadCache();
	brainDB.loadCache();
	neuronDB.loadCache();
	axonDB.loadCache();
	dendriteDB.loadCache();
	synapseDB.loadCache();
}

void Global::shutdown(void)
{


	std::cerr << "Shutting down: Stopping databases" << std::endl;

	layerDB.shutdown();
	clusterDB.shutdown();
	columnDB.shutdown();
	nucleusDB.shutdown();
	regionDB.shutdown();
	brainDB.shutdown();
	neuronDB.shutdown();
	axonDB.shutdown();
	dendriteDB.shutdown();
	synapseDB.shutdown();

	
}

void Global::debug(char *str)
{
	boost::mutex::scoped_lock amx(debug_mutex);

	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
	std::string timestr(buffer);

	std::string s(str);

	if (debugfile == NULL)
	{
		std::string filename(std::string(DB_PATH) + BRAINDEMONAME + std::string("/") + std::string("debug.txt"));
		debugfile = new std::ofstream(filename, std::ios::out);
	}
	*debugfile << timestr << " : " << s << std::endl;
}

void Global::debug(std::stringstream &ss)
{
	debug((char *)ss.str().c_str());
}

void Global::log(char *str)
{
	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
	std::string timestr(buffer);

	std::string s(str);
	std::cout << timestr << " : " << s;

	std::ofstream logfile;
	logfile.open(std::string(DB_PATH) + BRAINDEMONAME + std::string("/") + "logfile.txt", std::ios::app);
	logfile << timestr << " : " << s;
	logfile.close();
}

void Global::log(std::stringstream &ss)
{
	log((char *)ss.str().c_str());
}

void Global::writeSyncpoint(int sp)
{

	time_t rawtime;
	struct tm *timeinfo;
	char buffer[80];

	time(&rawtime);
	timeinfo = localtime(&rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", timeinfo);
	std::string timestr(buffer);

	std::ofstream spfile;
	spfile.open(std::string(DB_PATH) + BRAINDEMONAME + std::string("/") + "syncpoint.txt", std::ios::trunc);
	spfile << timestr << " | syncpoint " << " = " << sp << std::endl;

	for (size_t i = 0; i < CTYPE_COUNT; i++)
	{
		long thisCount = 0;
		switch (i)
		{
			//	1 = ComponentDB<Brain> brainDB;
		case 1:
			thisCount = (long)globalObject->brainDB.size();
			spfile << timestr << " | brain " << " = " << thisCount << std::endl;
			break;
			//	2 = ComponentDB<Region> regionDB;
		case 2:
			thisCount = (long)globalObject->regionDB.size();
			spfile << timestr << " | region " << " = " << thisCount << std::endl;
			break;
			//	3 = ComponentDB<Nucleus> nucleusDB;
		case 3:
			thisCount = (long)globalObject->nucleusDB.size();
			spfile << timestr << " | nucleus " << " = " << thisCount << std::endl;
			break;
			//	4 = ComponentDB<Column> columnDB;
		case 4:
			thisCount = (long)globalObject->columnDB.size();
			spfile << timestr << " | column " << " = " << thisCount << std::endl;
			break;
			//	5 = ComponentDB<Layer> layerDB;
		case 5:
			thisCount = (long)globalObject->layerDB.size();
			spfile << timestr << " | layer " << " = " << thisCount << std::endl;
			break;
			//	6 = ComponentDB<Cluster> clusterDB;
		case 6:
			thisCount = (long)globalObject->clusterDB.size();
			spfile << timestr << " | cluster " << " = " << thisCount << std::endl;
			break;
			//	7 = ComponentDB<Neuron> neuronDB;
		case 7:
			thisCount = (long)globalObject->neuronDB.size();
			spfile << timestr << " | neuron " << " = " << thisCount << std::endl;
			break;
			//	8 = ComponentDB<Axon> axonDB;
		case 8:
			thisCount = (long)globalObject->axonDB.size();
			spfile << timestr << " | axon " << " = " << thisCount << std::endl;
			break;
			//	9 = ComponentDB<Dendrite> dendriteDB;
		case 9:
			thisCount = (long)globalObject->dendriteDB.size();
			spfile << timestr << " | dendrite " << " = " << thisCount << std::endl;
			break;
			//	10 = ComponentDB<Synapse> synapseDB;
		case 10:
			thisCount = (long)globalObject->synapseDB.size();
			spfile << timestr << " | synapse " << " = " << thisCount << std::endl;
			break;
		default:
			break;
		}
	}
	spfile.close();
}

int Global::readSyncpoint(void)
{
	std::string line;
	std::string delimiter1 = "|";
	std::string delimiter2 = "=";
	std::ifstream spfile;
	spfile.open(std::string(DB_PATH) + BRAINDEMONAME + std::string("/") + "syncpoint.txt", std::ios::in);
	while (std::getline(spfile, line))
	{
		std::string temp = line.substr(line.find(delimiter1) + 1);
		std::string sptoken1 = Global::trim(temp);
		size_t pos = sptoken1.find(delimiter2);
		temp = sptoken1.substr(0, pos);
		std::string sptokenKey = Global::trim(temp);
		temp = sptoken1.substr(pos + 1);
		std::string sptokenValue = Global::trim(temp);
		int value = (size_t)atoi(sptokenValue.c_str());

		if (sptokenKey == "syncpoint")
			syncpoint = value;
		//	1 = ComponentDB<Brain> brainDB;
		if (sptokenKey == "brain")
			globalObject->brainDB.componentCount = (size_t)value;
		//	2 = ComponentDB<Region> regionDB;
		if (sptokenKey == "region")
			globalObject->regionDB.componentCount = (size_t)value;
		//	3 = ComponentDB<Nucleus> nucleusDB;
		if (sptokenKey == "nucleus")
			globalObject->nucleusDB.componentCount = (size_t)value;
		//	4 = ComponentDB<Column> columnDB;
		if (sptokenKey == "column")
			globalObject->columnDB.componentCount = (size_t)value;
		//	5 = ComponentDB<Layer> layerDB;
		if (sptokenKey == "layer")
			globalObject->layerDB.componentCount = (size_t)value;
		//	6 = ComponentDB<Cluster> clusterDB;
		if (sptokenKey == "cluster")
			globalObject->clusterDB.componentCount = (size_t)value;
		//	7 = ComponentDB<Neuron> neuronDB;
		if (sptokenKey == "neuron")
			globalObject->neuronDB.componentCount = (size_t)value;
		//	8 = ComponentDB<Axon> axonDB;
		if (sptokenKey == "axon")
			globalObject->axonDB.componentCount = (size_t)value;
		//	9 = ComponentDB<Dendrite> dendriteDB;
		if (sptokenKey == "dendrite")
			globalObject->dendriteDB.componentCount = (size_t)value;
		//	10 = ComponentDB<Synapse> synapseDB;
		if (sptokenKey == "synapse")
			globalObject->synapseDB.componentCount = (size_t)value;
	}

	spfile.close();

	return syncpoint;
}

void Global::logFiring(Neuron *n, bool fireStatus)
{

	int intStat = (fireStatus) ? 1 : 0;
	std::stringstream msg;

	if (globalObject->setLogFiring)
	{
		msg << "Neuron" << " " << n->id << ", state " << intStat << ", threshold " << n->threshold << ",  potential" << n->potential;
		writeFireLog(msg.str().c_str());
	}
}

long getGlobalTimestep()
{
	return globalObject->current_timestep;
}


std::string Global::getMessage(void)
{
	boost::mutex::scoped_lock mx(xThreadQueue_mutex);
	std::string msg;
	if (!xThreadQueue.empty())
	{
		msg = xThreadQueue.front();
		xThreadQueue.pop();
	}
	return msg;
}

void Global::putMessage(std::string msg)
{
	boost::mutex::scoped_lock mx(xThreadQueue_mutex);
	xThreadQueue.push(msg);
}


// get all Neurons currently (relateive to "now" parameter) firing 
// Note that now should be some period in the past (current_timestamp - 10) to facilitate neuronal adjustments
std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *> *Global::getNeurons(long now, long interval) 
{
	std::vector<Neuron *> *preNeurons = new std::vector<Neuron *>();
	std::vector<Neuron *> *postNeurons = new std::vector<Neuron *>();
	unsigned long beginning_timestamp = now - interval;
	//unsigned long ending_timestamp = now + interval;

	long neuronStart = globalObject->componentBase[ComponentTypeNeuron];
	long neuronEnd = globalObject->componentCounter[ComponentTypeNeuron];
	for(long neuronId = neuronStart;neuronId<neuronEnd;neuronId++)
	{
		Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
		if(neuron->lastfired > beginning_timestamp)
		{
			if (neuron->lastfired <= (unsigned long) now && neuron->lastfired >= beginning_timestamp) // Treat same interval the same as pre interval
			{
				preNeurons->push_back(neuron);
			}
			else if (neuron->lastfired > (unsigned long) now)
			{
				postNeurons->push_back(neuron);
			}

		}

	}

	std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *> *retpair = new std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *>();
	retpair->first = preNeurons;
	retpair->second = postNeurons;

	return retpair;
}

std::pair<std::vector<Tuple *> *, std::vector<Tuple *> *> *Global::getSpikes(long now, long interval) //  Spikes that have already occured
{
	std::vector<Tuple *> *preSpikeTimes = new std::vector<Tuple *>();
	std::vector<Tuple *> *postSpikeTimes = new std::vector<Tuple *>();

	int beginning_timestamp = now - interval;
	int ending_timestamp = now + interval;

	for(int thisInterval=beginning_timestamp;thisInterval<ending_timestamp;thisInterval++)
	{
		int thisIndex = thisInterval % MAX_TIMEINTERVAL_BUFFER_SIZE;

		boost::mutex::scoped_lock amx(*(globalObject->teVector_mutex[thisIndex]));
		int numEvents = globalObject->timeIntervalEvents[thisIndex].size(); // timeIntervalEvents[thisIndex].size();
		if (numEvents > 0)
		{
			for (int teIndex = 0; teIndex < numEvents; teIndex++)
			{
				TimedEvent *te = globalObject->timeIntervalEvents[thisIndex][teIndex];
				if (thisInterval <= now) // Treat same interval the same as pre interval
				{
					Tuple *t = new Tuple();
					t->objectPtr = (char *)te;
					t->value = te->slice;
					preSpikeTimes->push_back(t);
				}
				else if (thisInterval > now) 
				{
					Tuple *t = new Tuple();
					t->objectPtr = (char *)te;
					t->value = te->slice;
					postSpikeTimes->push_back(t);
				}
			}
		}
	}


	std::pair<std::vector<Tuple *> *, std::vector<Tuple *> *> *retpair = new std::pair<std::vector<Tuple *> *, std::vector<Tuple *> *>();
	retpair->first = preSpikeTimes;
	retpair->second = postSpikeTimes;

	return retpair;
}

void Global::writeFireLog(std::string msg)
{

	if (firelogfile == NULL)
	{
		firelogfile = new std::ofstream(std::string(LOGGING_PATH) + BRAINDEMONAME + std::string("/") + "fireLog.txt", std::ios::app);
	}
	*firelogfile << std::setw(10) << std::setfill('0') <<  current_timestep << " : " << msg << std::endl;
}

void Global::closeFireLog(void)
{

	if (firelogfile != NULL)
	{
		firelogfile->close();
		firelogfile = NULL;
	}
}

void Global::writeEventLog(std::string msg)
{
	boost::mutex::scoped_lock amx(log_event_mutex);

	if (eventlogfile == NULL)
	{
		std::string filename = std::string(LOGGING_PATH) + BRAINDEMONAME + std::string("/") + "eventLog.txt";
		std::cout << "Creating event log " << filename << std::endl;
		eventlogfile = new std::ofstream(filename, std::ios::app);
	}

	*eventlogfile << std::setw(10) << std::setfill('0') <<  current_timestep << " : " << msg << std::endl;
	eventlogfile->flush();
}

void Global::closeEventLog(void)
{

	if (eventlogfile != NULL)
	{
		eventlogfile->flush();
		eventlogfile->close();
		eventlogfile = NULL; 
		std::cout << "Event log closed" << std::endl;
	}
}

void Global::writeStructureLog(std::string msg)
{
	if (structurelogfile == NULL)
	{
		std::string filename = std::string(LOGGING_PATH) + BRAINDEMONAME + std::string("/") + "structureLog.txt";
		std::cout << "Creating structure log " << filename << std::endl;
		structurelogfile = new std::ofstream(filename, std::ios::app);
	}

	*structurelogfile << std::setw(10) << std::setfill('0') <<  current_timestep << " : " << msg << std::endl;
	structurelogfile->flush();
}

void Global::closeStructureLog(void)
{

	if (structurelogfile != NULL)
	{
		structurelogfile->flush();
		structurelogfile->close();
		structurelogfile = NULL; 
		std::cout << "Structure log closed" << std::endl;
	}
}

void Global::closeDebugLog(void)
{

	if (debugfile != NULL)
	{
		debugfile->close();
	}
}

void Global::logStructure(void)
{
		long nucleusIdStart = globalObject->componentBase[ComponentTypeNucleus];
		long nucleusIdEnd = globalObject->componentCounter[ComponentTypeNucleus];
		for(long nucleusId=nucleusIdStart;nucleusId<nucleusIdEnd;nucleusId++)
		{
			Nucleus *nucleus = globalObject->nucleusDB.getComponent(nucleusId);
			logStructure(nucleus);
		}

		std::stringstream ss2;

		ss2 << "structure_complete: nucleusCount=" << nucleusIdEnd - nucleusIdStart;
		globalObject->writeStructureLog(ss2.str().c_str());

		globalObject->closeStructureLog();
}

void Global::logStructure(Nucleus *nucleus)
{
	long neuron_count = 0;
	long noDendriteNeuronCount = 0;
	size_t numColumns = nucleus->columns.size();
	for(size_t i=0;i<numColumns;i++)
	{
		long col_id = nucleus->columns[i];
		Column *column = globalObject->columnDB.getComponent(col_id);
		size_t numLayers = column->layers.size();
		for(size_t j=0;j<numLayers;j++)
		{
			long layer_id = column->layers[j];
			Layer *layer = globalObject->layerDB.getComponent(layer_id);
			size_t numClusters = layer->clusters.size();
			for(size_t k=0;k<numClusters;k++)
			{
				long cluster_id = layer->clusters[k];
				Cluster *cluster = globalObject->clusterDB.getComponent(cluster_id);
				size_t numNeurons = cluster->neurons.size();
				for(size_t l=0;l<numNeurons;l++) 
				{
					std::stringstream ss;
					long neuron_id = cluster->neurons[l];
					Neuron *neuron = globalObject->neuronDB.getComponent(neuron_id);
					// log neuron

					neuron_count++;

					ss << "neuron_structure: neuron=" << neuron->id << ", nucleus=" << nucleus->name << ", axons=[";
					size_t numAxons = neuron->getAxons()->size();
					std::string sep="";
					// log axons
					for(size_t m=0;m<numAxons;m++)
					{
						long axon_id = (*neuron->getAxons())[m];
						Axon *ax = globalObject->axonDB.getComponent(axon_id);

						ss << sep << ax->id;

						std::vector<long> *synapses = ax->getSynapses();
						size_t numSynapses = synapses->size();
						for(size_t n=0;n<numSynapses;n++)
						{
							long synapse_id = (*ax->getSynapses())[n];

							ss << "/" << synapse_id;

						}
						sep=";";
					}
					ss << "], dendrites=[";
					size_t numDendrites = neuron->getDendrites()->size();
					if(numDendrites==0) {
						noDendriteNeuronCount++;
//						std::cout << "Neuron " << neuron->id << " has no dendrites." << std::endl;
					}
					sep="";
					// log dendrites
					for(size_t m=0;m<numDendrites;m++)
					{
						long dendrite_id = (*neuron->getDendrites())[m];
						Dendrite *den = globalObject->dendriteDB.getComponent(dendrite_id);

						ss << sep << den->id;

						long synapse_id = den->getSynapseId();
						long associate_neuron_id = den->getPreSynapticNeuronId();

						ss << "/" << synapse_id << "-" << associate_neuron_id;
						sep=";";
					}
					ss << "]";
					globalObject->writeStructureLog(ss.str().c_str());
				}
			}
		}
	}

	if(noDendriteNeuronCount>0)
	{
		std::cout << noDendriteNeuronCount << " neurons have no dendrites." << std::endl;
	}


	long synapse_start = globalObject->componentBase[ComponentTypeSynapse];
	long synapse_end = globalObject->componentCounter[ComponentTypeSynapse];
	for(long i=synapse_start;i<synapse_end;i++)
	{
		std::stringstream ss;

		Synapse *synapse = globalObject->synapseDB.getComponent(i);
		
		ss << "synapse_structure: synapse=" << synapse->id << ", neuron=" << synapse->postSynapticNeuronId;
		globalObject->writeStructureLog(ss.str().c_str());

	}

		long synapse_count = synapse_end - synapse_start;

		std::stringstream ss2;
		ss2 << "nucleus_counts: neuron_count=" << neuron_count << ", synapse_count=" << synapse_count;
		globalObject->writeStructureLog(ss2.str().c_str());


}


std::vector<float> Global::softmax(const std::vector<float> &input)
{
	std::vector<float> exponentials(input.size());
	std::transform(input.begin(), input.end(), exponentials.begin(), [](float value)
				   { return std::exp(value); });

	float sum = std::accumulate(exponentials.begin(), exponentials.end(), 0.0f);
	std::vector<float> softmaxVector(input.size());

	if (sum != 0)
	{ // Check to prevent division by zero
		std::transform(exponentials.begin(), exponentials.end(), softmaxVector.begin(), [sum](float value)
					   { return value / sum; });
	}

	return softmaxVector;
}

// Find the dendrite which connects neuronA to the axon of neuronB
Dendrite *Global::findConnectingDendrite(Neuron *neuronA, Neuron *neuronB)
{
	Dendrite *returnDendrite = NULL;

	auto it = neuronA->dendriteMap.find(neuronB->id);
	if (it != neuronA->dendriteMap.end())
	{
		long dendriteId = it->second;
		returnDendrite = globalObject->dendriteDB.getComponent(dendriteId);
	}
	return returnDendrite;
}
