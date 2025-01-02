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

#include <ctime>
#include "Global.h"
#include "Server.h" // if needed

extern Global *globalObject;
FILE *logFile;
int logMemoryAllocations;

char *ctypes[] = {"ComponentTypeUnknown", "ComponentTypeBrain", "ComponentTypeRegion", "ComponentTypeNucleus", "ComponentTypeColumn", "ComponentTypeLayer",
                  "ComponentTypeCluster", "ComponentTypeNeuron", "ComponentTypeAxon", "ComponentTypeDendrite", "ComponentTypeSynapse",
                  "ComponentTypeActionPotential", "ComponentTypeTimedEvent"};


long getGlobalTimestep()
{
	return globalObject->getCurrentTimestamp();
}

Global::Global(std::string inDBPath, std::string inModelName) : 
    neuronDB(30000,inDBPath, inModelName), 
    clusterDB(1000,inDBPath, inModelName), 
    dendriteDB(500000,inDBPath, inModelName), 
    synapseDB(500000,inDBPath, inModelName), 
    axonDB(30000,inDBPath, inModelName),
    layerDB(1000,inDBPath, inModelName),
    columnDB(1000,inDBPath, inModelName),
    nucleusDB(1000,inDBPath, inModelName),
    regionDB(100,inDBPath, inModelName),
    brainDB(10,inDBPath, inModelName)
{
    setDBPath(inDBPath);
    setModelName(inModelName);
    std::stringstream ss;
    for (size_t i = 0; i < CTYPE_COUNT; i++)
    {
        long index = (long)i;
        componentBase[i] = 100000000 * index;
        componentCounter[i] = componentBase[i];
        componentCounter2[i] = 0;

        ss << "Component " << ctypes[i] << " assigned values starting at  " << componentCounter[i] << std::endl;
        log(ss);
        ss.str(""); ss.clear();
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

    workers = new boost::asio::thread_pool(MAX_THREADPOOL_SIZE);
    firefile = NULL;
    firelogfile = NULL;
    debugfile = NULL;
    eventlogfile = NULL;
    structurelogfile = NULL;
    firePatternlogfile = NULL;

    setLogFiring = false;
    keepWebserverRunning = true;
    logEvents = false;
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
    return (size_t)-1;
}

char *Global::allocClearedMemory(size_t count)
{
    if (logFile == NULL)
    {
        if (logMemoryAllocations)
            logFile = fopen((std::string(getDBPath()) + getModelName() + std::string("/") + std::string("DebugLogFile.txt")).c_str(), "w");
    }

    if (logMemoryAllocations)
        fprintf(logFile, "Allocating %d bytes...\r", (int)count);

    char *mem = new char[count];
    memset(mem, '\0', count);

    if (logMemoryAllocations)
        fprintf(logFile, "%d (0x%llx) bytes Allocated\n", (int)count, (unsigned long long)mem);

    return mem;
}

void Global::freeMemory(char *mem)
{
    unsigned long long ptr = (unsigned long long)mem;

    if (logMemoryAllocations && logFile == NULL)
        logFile = fopen((std::string(getDBPath()) + getModelName() + std::string("/") + std::string("DebugLogFile.txt")).c_str(), "w");

    if (logMemoryAllocations)
        fprintf(logFile, "Deallocating 0x%llx\r", ptr);

    delete[] mem;

    if (logMemoryAllocations)
        fprintf(logFile, "0x%llx deallocated\n", ptr);
}

void Global::step(void)
{
    brain->step();
}

long Global::getTotalEvents() 
{
    long cts = getCurrentTimestamp();
    long maxLookahead = cts + MAX_TIMEINTERVAL_OFFSET;
    long totalEvents = 0;
    for(long i=cts;i<maxLookahead;i++)
    {
        size_t intervalOffsetValue = (size_t) i % MAX_TIMEINTERVAL_BUFFER_SIZE;
        totalEvents += timeIntervalEvents[intervalOffsetValue].size();
    }
    return totalEvents;
}

long Global::getAllTotalEvents() 
{
    long totalEvents = 0;
    for(long i=0;i<MAX_TIMEINTERVAL_BUFFER_SIZE;i++)
    {
        totalEvents += timeIntervalEvents[i].size();
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
    this->brain = brain;
}

size_t Global::brainSize(void)
{
    return brainDB.size();
}

void Global::insert(Region *region)
{
    regionDB.insert(region);
    bool newRegion = true;
    for(size_t i=0;i<brain->regions.size();i++)
    {
        if(brain->regions[i] == region->id)
        {
            newRegion = false;
        } 
    }
    if(newRegion)
        brain->regions.push_back(region->id);
}

size_t Global::regionsSize(void)
{
    return regionDB.size();
}

void Global::insert(Nucleus *nucleus)
{
    nucleusDB.insert(nucleus);
}

size_t Global::nucleiSize(void)
{
    return nucleusDB.size();
}

void Global::insert(Column *column)
{
    columnDB.insert(column);
}

size_t Global::columnsSize(void)
{
    return columnDB.size();
}

void Global::insert(Layer *layer)
{
    layerDB.insert(layer);
}

size_t Global::layersSize(void)
{
    return layerDB.size();
}

void Global::insert(Cluster *cluster)
{
    clusterDB.insert(cluster);
}

size_t Global::clustersSize(void)
{
    return clusterDB.size();
}

void Global::insert(Neuron *neuron)
{
    neuronDB.insert(neuron);
}

size_t Global::neuronsSize(void)
{
    return neuronDB.size();
}

void Global::insert(Axon *axon)
{
    axonDB.insert(axon);
}

size_t Global::axonsSize(void)
{
    return axonDB.size();
}

void Global::insert(Dendrite *dendrite)
{
    dendriteDB.insert(dendrite);
}

size_t Global::dendritesSize(void)
{
    return dendriteDB.size();
}

void Global::insert(Synapse *synapse)
{
    synapseDB.insert(synapse);
}

size_t Global::synapsesSize(void)
{
    return synapseDB.size();
}

void Global::insertFiring(Neuron *neuron)
{
    boost::mutex::scoped_lock amx(firingNeurons_mutex);
    firingNeurons.insert(std::make_pair(neuron->id, neuron));
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
    size_t intervalOffsetValue = (size_t)intervalOffset % MAX_TIMEINTERVAL_BUFFER_SIZE;
    boost::mutex::scoped_lock amx(*teVector_mutex[intervalOffsetValue]);
    timeIntervalEvents[intervalOffsetValue].push_back(timedEvent);
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
    // Simple K-Means clustering (as placeholder)
    std::vector<std::vector<float>> centroids(numClusters, std::vector<float>(vectorLength, 0.0));
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(0, (int)patterns.size() - 1);

    for (int i = 0; i < numClusters; ++i)
    {
        centroids[i] = patterns[dis(gen)];
    }

    std::vector<int> assignments(patterns.size(), -1);
    bool changed;

    do
    {
        changed = false;

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

    std::map<int, float> patternProbabilities;
    for (size_t i = 0; i < patterns.size(); ++i)
    {
        int cluster = assignments[i];
        float dist = euclideanDistance(patterns[i], centroids[cluster]);
        float probability = std::exp(-dist);
        patternProbabilities[(int)i] = probability;
    }

    return patternProbabilities;
}

void Global::cycle(void)
{
    runInference();
}

long Global::getCurrentTimestamp(void)
{
    return current_timestep;
}

Range Global::batchFire(std::vector<Neuron *> *firingNeurons, long waitPeriod)
{
    boost::mutex::scoped_lock amx1(batchfire_mutex);
    std::stringstream ss;
    if(logEvents) 
    {
        ss << "batch_fire: neurons=";
    }
    std::string sep = "";

    long lowestOffset = MAX_TIMEINTERVAL_OFFSET;
    long highestOffset = -MAX_TIMEINTERVAL_OFFSET;
    for (size_t i = 0; i < firingNeurons->size(); i++)
    {
        Neuron *neuron = (*firingNeurons)[i];
		neuron->setMembranePotential(neuron->threshold+1.0f);
        Range offset = neuron->fire();

        if(lowestOffset > offset.low)
            lowestOffset = offset.low;
        if(highestOffset < offset.high)
            highestOffset = offset.high;

        if(logEvents) 
        {
            ss << sep << neuron->id;
            sep = ",";
        }
    }

    if(logEvents) 
    {
        writeEventLog(ss.str().c_str());
    }

    globalObject->lastFiredNeuron = NULL;
    // (Here you might wait or do other logic if needed)
    Range range;
    range.low = lowestOffset;
    range.high = highestOffset;    
    return range;
}

Range Global::batchLearn(std::vector<Neuron *> *inputNeurons, std::vector<Neuron *> *outputNeurons, std::string nucleusName)
{
    Range range;
    std::vector<Neuron *> firingNeurons;
    for(size_t i=0;i<inputNeurons->size();i++)
    {
        firingNeurons.push_back((*inputNeurons)[i]);
    }
    for(size_t i=0;i<outputNeurons->size();i++)
    {
        firingNeurons.push_back((*outputNeurons)[i]);
    }

    batchFire(&firingNeurons);
    range.low = 0;
    range.high = 0;
    return range;
}

float Global::evaluateResponse(std::vector<Neuron *> *outputNeurons,std::vector<Neuron *> *nucleusNeurons)
{
    size_t totalSize = nucleusNeurons->size();
    size_t totalCorrectFiring = 0;
    size_t totalCorrectNotFiring = 0;
    size_t totalIncorrectFiring = 0;
    size_t totalIncorrectNotFiring = 0;
    std::unordered_map<unsigned long, Neuron *> outputNeuronMap;
    for(size_t i=0;i<outputNeurons->size();i++)
    {
        Neuron *n = (*outputNeurons)[i];
        outputNeuronMap.insert(std::make_pair(n->id, n));
    }

    for(size_t i=0;i<nucleusNeurons->size();i++)
    {
        Neuron *thisNeuron =(*nucleusNeurons)[i];
        auto it = outputNeuronMap.find(thisNeuron->id);
        if (it != outputNeuronMap.end())
        {
            Neuron *outputNeuron = it->second;
            if(outputNeuron->isFiring())
                totalCorrectFiring++;
            else 
                totalIncorrectNotFiring++;
        }
        else
        {
            if(thisNeuron->isFiring())
                totalIncorrectFiring++;
            else 
                totalCorrectNotFiring++;
        }
    }

    size_t correct = (totalCorrectFiring + totalCorrectNotFiring);
    float pct = (float)correct / (float)totalSize;
    return pct *  100.0;
}

void Global::runInference(void)
{
    boost::mutex::scoped_lock amx1(inference_mutex);
    size_t intervalOffsetValue = (size_t)getCurrentTimestamp() % MAX_TIMEINTERVAL_BUFFER_SIZE;
    boost::mutex::scoped_lock amx(*teVector_mutex[intervalOffsetValue]);
    std::vector<TimedEvent *> *teVector = &timeIntervalEvents[intervalOffsetValue];
    size_t tevSize = teVector->size();

    if (tevSize > 0)
    {
        if (tevSize > THREADPOOL_SLICE_THRESHOLD)
        {
            size_t sizeper = tevSize / (size_t)MAX_THREADPOOL_SIZE;
            for (size_t n = 0; n < (size_t)MAX_THREADPOOL_SIZE; ++n)
            {
                size_t per = sizeper;
                size_t startper = n * sizeper;
                if(startper + per > tevSize)
                    per = tevSize - startper;

                boost::asio::post(*workers, [=] {
                    inferenceSlice(teVector, startper, per, false, intervalOffsetValue);
                });
            }
            workers->join();
        }
        else
        {
            inferenceSlice(teVector, 0, tevSize, false, intervalOffsetValue);
        }

        // Clean up processed events
        for (auto &te : *teVector)
        {
            delete te;
        }
        teVector->clear();
    }
}

void Global::runLearning(void)
{
    long learningWindowSize = 100L;
    long learningInterval = getCurrentTimestamp()  - learningWindowSize;
    std::pair<std::vector<Neuron *> *, std::vector<Neuron *> *> *neurons = getNeurons(learningInterval, learningWindowSize);

    std::vector<Neuron *> *preNeurons = neurons->first;
    std::vector<Neuron *> *postNeurons = neurons->second;

    if (preNeurons->empty() && postNeurons->empty()) {
        delete preNeurons;
        delete postNeurons;
        delete neurons;
        return;
    }

    long neuronIdStart = componentBase[ComponentTypeNeuron];
    long neuronIdEnd = componentCounter[ComponentTypeNeuron];
    std::vector<Neuron *> affectedNeurons;

    for(long neuronIndex = neuronIdStart; neuronIndex < neuronIdEnd; neuronIndex++)
    {
        Neuron *thisNeuron = neuronDB.getComponent(neuronIndex);
        if(thisNeuron && thisNeuron->lastfired > (unsigned long)(learningInterval - learningWindowSize))
        {
            thisNeuron->applySTDP(neurons, learningInterval);
            affectedNeurons.push_back(thisNeuron);
        }
    }

    // Normalize potentials after STDP
    size_t nCount = affectedNeurons.size();
    if(nCount>0)
    {
        Neuron *thisNeuron = nullptr;
        float totalPotential = 0;
        for(size_t i = 0; i < nCount; i++)
        {
            thisNeuron = affectedNeurons[i];
            totalPotential += thisNeuron->getMembranePotential();
        }
        float mean = totalPotential / (float)nCount;

        if(!std::isnan(mean))
        {
            for(size_t i = 0; i < nCount; i++)
            {
                thisNeuron = affectedNeurons[i];
                thisNeuron->setMembranePotential(thisNeuron->getMembranePotential() - mean);
            }
        }
        for(size_t i = 0; i < nCount; i++)
        {
            thisNeuron = affectedNeurons[i];
			thisNeuron->fire();
        }

    }
	globalObject->applyLocalHomeostasis(preNeurons); // run homeostasis
	globalObject->applyLocalHomeostasis(postNeurons); // run homeostasis

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
        if (ap->owningProcessId >= componentBase[ComponentTypeAxon] && ap->owningProcessId < componentBase[ComponentTypeSynapse])
        {
            if (ap->owningProcessId >= componentBase[ComponentTypeDendrite])
            {
                Synapse *s = synapseDB.getComponent(te->synapseId);
                if(s) s->receiveAP(ap);
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

    strftime(buffer, 80, "%m-%d-%Y %I:%M:%S", timeinfo);
    std::string timestr(buffer);

    std::string s(str);
    if (debugfile == NULL)
    {
        std::string filename(std::string(getDBPath()) + getModelName() + std::string("/") + std::string("debug.txt"));
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

    strftime(buffer, 80, "%m-%d-%Y %I:%M:%S", timeinfo);
    std::string timestr(buffer);

    std::string s(str);
    std::cout << timestr << " : " << s;

    std::ofstream logfile;
    logfile.open(dbPath + modelName + std::string("/") + "logfile.txt", std::ios::app);
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

    strftime(buffer, 80, "%m-%d-%Y %I:%M:%S", timeinfo);
    std::string timestr(buffer);

    std::ofstream spfile;
    spfile.open(std::string(getDBPath()) + getModelName() + std::string("/") + "syncpoint.txt", std::ios::trunc);
    spfile << timestr << " | syncpoint = " << sp << std::endl;

    spfile.close();
}

void Global::writeCounters(void)
{


    std::ofstream ctrfile;
    ctrfile.open(std::string(getDBPath()) + getModelName() + std::string("/") + "counters.txt", std::ios::trunc);
	for(size_t i=0; i<CTYPE_COUNT;i++) {
		long ctr = globalObject->componentCounter[i];
        ctrfile << i << " = " << ctr << std::endl;
	}

    ctrfile.close();
}

void Global::readCounters(void)
{


    std::ifstream ctrfile;
    ctrfile.open(std::string(getDBPath()) + getModelName() + std::string("/") + "counters.txt");
	for(size_t i=0; i<CTYPE_COUNT;i++) {
        std::string line;
        size_t seq=0;
        long val=0;
        char eq;
        std::getline(ctrfile,line);
        std::istringstream iss(line);
        iss >> seq >> eq >> val;

		globalObject->componentCounter[seq] = val;
        globalObject->componentCounter2[seq] = val - globalObject->componentBase[seq];
	}

    ctrfile.close();
}

int Global::readSyncpoint(void)
{
    std::string line;
    std::string delimiter1 = "|";
    std::string delimiter2 = "=";
    std::ifstream spfile;
    spfile.open(std::string(getDBPath()) + getModelName() + std::string("/") + "syncpoint.txt", std::ios::in);
    while (std::getline(spfile, line))
    {
        std::string temp = line.substr(line.find(delimiter1) + 1);
        std::string sptoken1 = trim(temp);
        size_t pos = sptoken1.find(delimiter2);
        temp = sptoken1.substr(0, pos);
        std::string sptokenKey = trim(temp);
        temp = sptoken1.substr(pos + 1);
        std::string sptokenValue = trim(temp);
        int value = atoi(sptokenValue.c_str());

        if (sptokenKey == "syncpoint")
            syncpoint = value;
    }

    spfile.close();
    return syncpoint;
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

void Global::writeFireLog(std::string msg)
{
    if (firelogfile == NULL)
    {
        firelogfile = new std::ofstream(std::string(LOGGING_PATH) + getModelName() + std::string("/") + "fireLog.txt", std::ios::app);
    }
    *firelogfile << std::setw(10) << std::setfill('0') <<  getCurrentTimestamp() << " : " << msg << std::endl;
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
        std::string filename = std::string(LOGGING_PATH) + getModelName() + std::string("/") + "eventLog.txt";
        eventlogfile = new std::ofstream(filename, std::ios::app);
    }

    *eventlogfile << std::setw(10) << std::setfill('0') <<  getCurrentTimestamp() << " : " << msg << std::endl;
    eventlogfile->flush();
}

void Global::closeEventLog(void)
{
    if (eventlogfile != NULL)
    {
        eventlogfile->flush();
        eventlogfile->close();
        eventlogfile = NULL; 
    }
}

void Global::writeStructureLog(std::string msg)
{
    if (structurelogfile == NULL)
    {
        std::string filename = std::string(LOGGING_PATH) + getModelName() + std::string("/") + "structureLog.txt";
        structurelogfile = new std::ofstream(filename, std::ios::app);
    }

    *structurelogfile << std::setw(10) << std::setfill('0') <<  getCurrentTimestamp() << " : " << msg << std::endl;
    structurelogfile->flush();
}

void Global::closeStructureLog(void)
{
    if (structurelogfile != NULL)
    {
        structurelogfile->flush();
        structurelogfile->close();
        structurelogfile = NULL; 
    }
}

void Global::writeFirePatternLog(std::string msg)
{
    boost::mutex::scoped_lock amx(firingPatterns_mutex);
    if (firePatternlogfile == NULL)
    {
        std::string filename = std::string(LOGGING_PATH) + getModelName() + std::string("/") + "firePatterLog.txt";
        firePatternlogfile = new std::ofstream(filename, std::ios::app);
    }

    *firePatternlogfile << std::setw(10) << std::setfill('0') <<  getCurrentTimestamp() << " : " << msg << std::endl;
    firePatternlogfile->flush();
}

void Global::closeFirePatternLog(void)
{
    if (firePatternlogfile != NULL)
    {
        firePatternlogfile->flush();
        firePatternlogfile->close();
        firePatternlogfile = NULL; 
    }
}

void Global::logStructure(void)
{
    long nucleusIdStart = componentBase[ComponentTypeNucleus];
    long nucleusIdEnd = componentCounter[ComponentTypeNucleus];
    for(long nucleusId=nucleusIdStart;nucleusId<nucleusIdEnd;nucleusId++)
    {
        Nucleus *nucleus = nucleusDB.getComponent(nucleusId);
        if(nucleus)
            logStructure(nucleus);
    }

    std::stringstream ss2;
    ss2 << "structure_complete: nucleusCount=" << nucleusIdEnd - nucleusIdStart;
    writeStructureLog(ss2.str().c_str());

    closeStructureLog();
}

void Global::logStructure(Nucleus *nucleus)
{
    long neuron_count = 0;
    long noDendriteNeuronCount = 0;
    size_t numColumns = nucleus->columns.size();
    for(size_t i=0;i<numColumns;i++)
    {
        long col_id = nucleus->columns[i];
        Column *column = columnDB.getComponent(col_id);
        size_t numLayers = column->layers.size();
        for(size_t j=0;j<numLayers;j++)
        {
            long layer_id = column->layers[j];
            Layer *layer = layerDB.getComponent(layer_id);
            size_t numClusters = layer->clusters.size();
            for(size_t k=0;k<numClusters;k++)
            {
                long cluster_id = layer->clusters[k];
                Cluster *cluster = clusterDB.getComponent(cluster_id);
                size_t numNeurons = cluster->neurons.size();
                for(size_t l=0;l<numNeurons;l++) 
                {
                    std::stringstream ss;
                    long neuron_id = cluster->neurons[l];
                    Neuron *neuron = neuronDB.getComponent(neuron_id);
                    if(!neuron) continue;
                    neuron_count++;

                    ss << "neuron_structure: neuron=" << neuron->id << ", nucleus=" << nucleus->name << ", axons=[";
                    size_t numAxons = neuron->getAxons()->size();
                    std::string sep="";
                    for(size_t m=0;m<numAxons;m++)
                    {
                        long axon_id = (*neuron->getAxons())[m];
                        Axon *ax = axonDB.getComponent(axon_id);

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
                    }
                    sep="";
                    for(size_t m=0;m<numDendrites;m++)
                    {
                        long dendrite_id = (*neuron->getDendrites())[m];
                        Dendrite *den = dendriteDB.getComponent(dendrite_id);

                        ss << sep << den->id;
                        long synapse_id = den->getSynapseId();
                        long associate_neuron_id = den->getPreSynapticNeuronId();

                        ss << "/" << synapse_id << "-" << associate_neuron_id;
                        sep=";";
                    }
                    ss << "]";
                    writeStructureLog(ss.str().c_str());
                }
            }
        }
    }

    long synapse_start = componentBase[ComponentTypeSynapse];
    long synapse_end = componentCounter[ComponentTypeSynapse];
    for(long i=synapse_start;i<synapse_end;i++)
    {
        Synapse *synapse = synapseDB.getComponent(i);
        if(!synapse) continue;
        std::stringstream ss;
        ss << "synapse_structure: synapse=" << synapse->id << ", neuron=" << synapse->postSynapticNeuronId;
        writeStructureLog(ss.str().c_str());
    }

    long synapse_count = synapse_end - synapse_start;
    std::stringstream ss2;
    ss2 << "nucleus_counts: neuron_count=" << neuron_count << ", synapse_count=" << synapse_count;
    writeStructureLog(ss2.str().c_str());
}

std::vector<float> Global::softmax(const std::vector<float> &input)
{
    std::vector<float> exponentials(input.size());
    std::transform(input.begin(), input.end(), exponentials.begin(), [](float value)
    { return std::exp(value); });

    float sum = std::accumulate(exponentials.begin(), exponentials.end(), 0.0f);
    std::vector<float> softmaxVector(input.size());

    if (sum != 0)
    {
        std::transform(exponentials.begin(), exponentials.end(), softmaxVector.begin(), [sum](float value)
        { return value / sum; });
    }

    return softmaxVector;
}

Dendrite *Global::findConnectingDendrite(Neuron *neuronA, Neuron *neuronB)
{
    Dendrite *returnDendrite = NULL;
    auto it = neuronA->dendriteMap.find(neuronB->id);
    if (it != neuronA->dendriteMap.end())
    {
        long dendriteId = it->second;
        returnDendrite = dendriteDB.getComponent(dendriteId);
    }
    return returnDendrite;
}

std::pair<std::vector<Tuple*>*, std::vector<Tuple*>*> *Global::getSpikes(long now, long interval)
{
    std::vector<Tuple*> *preSpikeTimes = new std::vector<Tuple*>();
    std::vector<Tuple*> *postSpikeTimes = new std::vector<Tuple*>();

    int beginning_timestamp = (int)(now - interval);
    int ending_timestamp = (int)(now + interval);

    for(int thisInterval=beginning_timestamp;thisInterval<ending_timestamp;thisInterval++)
    {
        int thisIndex = thisInterval % MAX_TIMEINTERVAL_BUFFER_SIZE;
        boost::mutex::scoped_lock amx(*teVector_mutex[thisIndex]);
        int numEvents = (int)timeIntervalEvents[thisIndex].size();
        if (numEvents > 0)
        {
            for (int teIndex = 0; teIndex < numEvents; teIndex++)
            {
                TimedEvent *te = timeIntervalEvents[thisIndex][teIndex];
                if (thisInterval <= now)
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

    std::pair<std::vector<Tuple*>*, std::vector<Tuple*>*> *retpair = new std::pair<std::vector<Tuple*>*, std::vector<Tuple*>*>();
    retpair->first = preSpikeTimes;
    retpair->second = postSpikeTimes;

    return retpair;
}

std::pair<std::vector<Neuron *>*, std::vector<Neuron *>* > *Global::getNeurons(long now, long interval)
{
    std::vector<Neuron *> *preNeurons = new std::vector<Neuron *>();
    std::vector<Neuron *> *postNeurons = new std::vector<Neuron *>();

    unsigned long beginning_timestamp = (unsigned long)(now - interval);

    long neuronStart = componentBase[ComponentTypeNeuron];
    long neuronEnd = componentCounter[ComponentTypeNeuron];
    for(long neuronId = neuronStart; neuronId<neuronEnd; neuronId++)
    {
        Neuron *neuron = neuronDB.getComponent(neuronId);
        if(neuron && neuron->lastfired > beginning_timestamp)
        {
            if (neuron->lastfired <= (unsigned long) now && neuron->lastfired >= beginning_timestamp)
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

void Global::logFiring(Neuron *n,bool firestatus)
{
    int intStat = (firestatus) ? 1 : 0;
    std::stringstream msg;
    if (setLogFiring)
    {
        msg << "Neuron " << n->id << ", state " << intStat << ", threshold " << n->threshold << ", potential " << n->getMembranePotential();
        writeFireLog(msg.str().c_str());
    }
}




// New method to apply local homeostasis
void Global::applyLocalHomeostasis(std::vector<Neuron *> *neurons)
{
	for(int idx=0;idx<neurons->size();idx++) 
	{
		Neuron *neuron = (*neurons)[idx];
        float recentRate = neuron->getEstimatedFiringRate(); 
        float diff = HOMEOSTASIS_TARGET_RATE - recentRate;

        if(std::fabs(diff) > 0.001f)
        {
            neuron->threshold += diff * HOMEOSTASIS_ADJUST_FACTOR; 
            if (neuron->threshold < 0.001f) {
                neuron->threshold = 0.001f; 
            }
            // Scale all synaptic weights of this neuron
            neuron->applySynapticScaling(SYNAPTIC_SCALING_FACTOR);
        }
    }
}

// New method to apply global homeostasis
void Global::applyGlobalHomeostasis()
{
    long neuronStart = componentBase[ComponentTypeNeuron];
    long neuronEnd = componentCounter[ComponentTypeNeuron];

    for(long neuronId = neuronStart; neuronId < neuronEnd; neuronId++)
    {
        Neuron *neuron = neuronDB.getComponent(neuronId);
        if(!neuron) continue;

        float recentRate = neuron->getEstimatedFiringRate(); 
        float diff = HOMEOSTASIS_TARGET_RATE - recentRate;

        if(std::fabs(diff) > 0.001f)
        {
            neuron->threshold += diff * HOMEOSTASIS_ADJUST_FACTOR; 
            if (neuron->threshold < 0.001f) {
                neuron->threshold = 0.001f; 
            }
            // Scale all synaptic weights of this neuron
            neuron->applySynapticScaling(SYNAPTIC_SCALING_FACTOR);
        }
    }
}
