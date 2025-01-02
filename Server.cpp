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

#include <boost/thread.hpp>
#include <iostream>
#include <string>
#include <cstring> // For strerror() and related functions
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <errno.h>
#include "Global.h"
#include "Server.h"

#include "nlohmann/json.hpp"

#define DEFAULT_BUFLEN 32768
#define DEFAULT_PORT "8123"


int Server::waitThread(void)
{
	int ret = waitThreadWorker();
	return ret;
}

int Server::waitThreadWorker(void)
{
    int ListenSocket = -1;
    int ClientSocket = -1;
    struct sockaddr_in serverAddress = {};
    ssize_t iResult, iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    size_t recvbuflen = DEFAULT_BUFLEN;
    //char sendbuf[DEFAULT_BUFLEN];
    //size_t sendbuflen = DEFAULT_BUFLEN;
    
    std::stringstream ss;
    
    ListenSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (ListenSocket == -1) {
        LOGSTREAM(ss) << "socket failed with error: " << strerror(errno) << "\n" << std::endl;
        globalObject->log(ss);
        return 1;
    }

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(atoi(DEFAULT_PORT));

    if (bind(ListenSocket, (struct sockaddr*)&serverAddress, sizeof(serverAddress)) == -1) {
        LOGSTREAM(ss) << "bind failed with error: " << strerror(errno) << "\n" << std::endl;
        globalObject->log(ss);
        close(ListenSocket);
        return 1;
    }

    if (listen(ListenSocket, SOMAXCONN) == -1) {
        LOGSTREAM(ss) << "listen failed with error: " << strerror(errno) << "\n" << std::endl;
        globalObject->log(ss);
        close(ListenSocket);
        return 1;
    }

    ClientSocket = accept(ListenSocket, NULL, NULL);
    if (ClientSocket == -1) {
        LOGSTREAM(ss) << "accept failed with error: " << strerror(errno) << "\n" << std::endl;
        globalObject->log(ss);
        close(ListenSocket);
        return 1;
    }

    close(ListenSocket);

    if(keepRunning)
        boost::thread t(&Server::waitThread, this);

    do {
        memset(recvbuf, 0, recvbuflen);
        iResult = recv(ClientSocket, recvbuf, recvbuflen - 1, 0);
        
        if (iResult > 0) {
            std::string resultString = parseAndRespond(recvbuf, iResult);
            ssize_t bytesToSend = resultString.size();
            iSendResult = send(ClientSocket, resultString.c_str(), bytesToSend, 0);
            
            if (iSendResult == -1) {
                LOGSTREAM(ss) << "send failed with error: " << strerror(errno) << "\n" << std::endl;
                globalObject->log(ss);
                close(ClientSocket);
                return 1;
            }
        }
        else if (iResult == 0) {
            // Connection closed by client
        }
        else {
            LOGSTREAM(ss) << "recv failed with error: " << strerror(errno) << "\n" << std::endl;
            globalObject->log(ss);
            close(ClientSocket);
            return 1;
        }
    } while (iResult > 0);

    close(ClientSocket);
    return 0;
}

Server::Server(void)
{
    keepRunning = true;
}

Server::~Server(void)
{
}

void Server::start(void)
{
    boost::thread t(&Server::waitThread,this);
}

void Server::stop(void)
{
    keepRunning = false;
}

// DHorak 7/31/2018 Recommend replacing with XML message passing structure - JSON or XML (or both) web services
std::string Server::parseAndRespond(char *buffer, int length)
{
	return parseAndRespondText(buffer, length);
}

// Parse, process and reply using JSON
std::string Server::parseAndRespondJSON(char *buffer, int length)
{
    // Get the buffer into a string object
    std::string b(buffer, length);

    // Parse the string into a JSON object
    nlohmann::json cmdObject = nlohmann::json::parse(b);

    // ... Your code to process the cmdObject and generate a response ...

    return "";
}

// Parse, process and reply using XML
std::string Server::parseAndRespondXML(char *buffer, int length)
{
	(void)buffer;
	(void)length;
	return "";
}

std::string Server::parseAndRespondText(char *buffer, int length)
{
	(void)length;
	std::stringstream ss;
	// command parser
	// format of recognized messages is 
	// COMMAND ARG[,ARG][,ARG]
	// WHERE COMMAND is:
	//	GETNEURONS nucleus,layer
	//	SETACTIVATION neuron1[,neuron2]
	//	GETACTIVATION neuron1[,neuron2]
	//	GETACTIVENEURONS nucleus,layer
	//	REPORT dummy
	// 
	// 
	char *space = strchr(buffer,' ');
	if(space==NULL) {
		std::string resultString = "ERROR001: Invalid command format";
		LOGSTREAM(ss) << resultString << std::endl;
		globalObject->log(ss);
		return resultString;
	} else {
		__int64 length = space - buffer;
		char *tempbuffer = (char *)globalObject->allocClearedMemory(length+1);
		strncpy(tempbuffer,buffer,length);
		std::string command(tempbuffer);
		globalObject->freeMemory(tempbuffer);
		trim(command);

//		LOGSTREAM(ss) << "Command: " << command << " received." << std::endl;
//		globalObject->log(ss);

		// Depending on command, parse the arguments
		if(command == "GETNEURONS") {
			LOGSTREAM(ss) << "GETNEURONS Command recognized: " << std::endl;
			globalObject->log(ss);
			// get the region and nucleus names
			std::string returnString("NEURONS=");
			std::string returnStringSeparator;
			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);
			size_t commapos = workString.find(",");
			if (commapos==std::string::npos) 
			{
				returnString = "ERROR004: GETNEURONS parameters missing. ";
				LOGSTREAM(ss) << returnString << std::endl;
				globalObject->log(ss);
				return returnString;
			}
			std::string lookupNucleusName = workString.substr(0,commapos);
			std::string lookupLayer = workString.substr(commapos+1);

			long layerNumber = (long) atoi(lookupLayer.c_str());

			LOGSTREAM(ss) << "GETNEURONS parameters are nucleus [" << lookupNucleusName << "] and layer [" << lookupLayer << "] " << std::endl;
			globalObject->log(ss);

			// Get the nucleus
			for (globalObject->nucleusDB.begin(); globalObject->nucleusDB.more(); globalObject->nucleusDB.next())
			{
				Nucleus *nuc = globalObject->nucleusDB.getValue();
				if(nuc->name == lookupNucleusName) 
				{
					LOGSTREAM(ss) << "GETNEURONS nucleus [" << lookupNucleusName << "] found" << std::endl;
					globalObject->log(ss);

					// Interate through all neurons in this nucleus
					size_t cSize = nuc->columns.size();
					LOGSTREAM(ss) << "GETNEURONS nucleus [" << lookupNucleusName << "] contains " << cSize << " columns " << std::endl;
					globalObject->log(ss);
					for (unsigned int i=0;i<cSize;i++)
					{
						long c = nuc->columns[i];
						Column *column = globalObject->columnDB.getComponent(c);
						//long colSize = (long)column->layers.size();
//						LOGSTREAM(ss) << "column [" << c << "] contains " << colSize << " layers " << std::endl;
//						globalObject->log(ss);

						long layerId = column->layers[layerNumber-1];
						Layer *layer = globalObject->layerDB.getComponent(layerId);


						unsigned int lcSize = (unsigned int) layer->clusters.size();
//						LOGSTREAM(ss) << "layer [" << layerId << "] contains " << lcSize << " clusters " << std::endl;
//						globalObject->log(ss);
						for (unsigned int j=0;j<lcSize;j++)
						{
							long clusterId = layer->clusters[j];
							Cluster *cluster = globalObject->clusterDB.getComponent(clusterId);
							unsigned int nSize = (unsigned int) cluster->getNeurons().size();
//							LOGSTREAM(ss) << "cluster [" << clusterId << "] contains " << nSize << " neurons " << std::endl;
//							globalObject->log(ss);
							for (unsigned int k=0;k<nSize;k++)
							{
								long neuronId = cluster->getNeurons()[k];
								// Got the neuronId
								// Convert neuronId and append it to return string
								std::stringstream ns;
								ns << neuronId;
								std::string neuronIdString = ns.str();
								returnString += returnStringSeparator+neuronIdString;
								returnStringSeparator = ","; // Set returnStringSeparator for next time
							}
						}
					}
//					LOGSTREAM(ss) << "Sending Response [" << returnString << "], " << returnString.size() << " bytes." << std::endl;
					LOGSTREAM(ss) << "Sending Response of " << returnString.size() << " bytes." << std::endl;
					globalObject->log(ss);
//					strncpy(sendbuf,returnString.c_str(),sendbuflength);
					return returnString;
				}
			}
			LOGSTREAM(ss) << "ERROR005: Nucleus " << lookupNucleusName << " not found: " << std::endl;
			globalObject->log(ss);
			returnString = "ERROR005: Nucleus ";
			returnString += lookupNucleusName+" not found";
			return returnString;
		} else if(command == "SETACTIVATION") {
			LOGSTREAM(ss) << "SETACTIVATION Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
			globalObject->log(ss);

			std::string returnString;

			// parameter string consists of list of neurons to activate (spike).
			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);

			std::vector<long> neuronList;
			split(workString,neuronList); // split command delimited workString and return vector of neuronIds (longs)
			//
			// Let's lock the timestamps during our activiation function
			boost::mutex::scoped_lock  amx(globalObject->timestep_mutex);
			for(unsigned int ix=0;ix<neuronList.size();ix++) 
			{
				long neuronId = neuronList[ix];
				Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);

				if(globalObject->logEvents) 
				{
					ss << "server_parseandrespondtext_SETACTIVATION: neuron=" << neuronId;
					globalObject->writeEventLog(ss.str().c_str());
				}
				neuron->fire();
			}

			returnString = "SETACTIVATION Command complete";
			return returnString;
		} else if(command == "GETACTIVATION") {
			LOGSTREAM(ss) << "GETACTIVATION Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
			globalObject->log(ss);

			std::string returnString;

			// parameter string consists of list of neurons to get activation status.
			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);

			returnString = "";
			std::vector<long> neuronList;
			split(workString,neuronList); // split command delimited workString and return vector of neuronIds (longs)

//			LOGSTREAM(ss) << "GETACTIVATION neuronlist contains  [" << neuronList.size() << "] neurons" << std::endl;
//			globalObject->log(ss);
			for(unsigned int ix=0;ix<neuronList.size();ix++) 
			{
				long neuronId = neuronList[ix];
//				LOGSTREAM(ss) << "GETACTIVATION get status of neuron [" << neuronId << "]" << std::endl;
//				globalObject->log(ss);
				Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
//				if(neuron->isFiring())
				if(neuron->hasFired())
				{
					neuron->resetLatch();
					returnString += "1";
				} else {
					returnString += "0";
				}
			}

			LOGSTREAM(ss) << "GETACTIVATION return string [" << returnString << "] at timestep " << globalObject->getCurrentTimestamp() << "." << std::endl;
			globalObject->log(ss);

			return returnString;
		} else if(command == "GETACTIVENEURONS") {
			LOGSTREAM(ss) << "GETACTIVENEURONS Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
			globalObject->log(ss);
			// get the region and nucleus names
			std::string returnString("NEURONS=");
			std::string returnStringSeparator;
			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);
			size_t commapos = workString.find(",");
			if (commapos==std::string::npos) 
			{
				returnString = "ERROR014: GETACTIVENEURONS parameters missing. ";
				LOGSTREAM(ss) << returnString << std::endl;
				globalObject->log(ss);
				return returnString;
			}
			std::string lookupNucleusName = workString.substr(0,commapos);
			std::string lookupLayer = workString.substr(commapos+1);

			long layerNumber = (long) atoi(lookupLayer.c_str());

			LOGSTREAM(ss) << "GETACTIVENEURONS parameters are nucleus [" << lookupNucleusName << "] and layer [" << lookupLayer << "] " << std::endl;
			globalObject->log(ss);

			// Get the nucleus
			for (globalObject->nucleusDB.begin(); globalObject->nucleusDB.more(); globalObject->nucleusDB.next())
			{
				Nucleus *nuc = globalObject->nucleusDB.getValue();
				if(nuc->name == lookupNucleusName) 
				{
					LOGSTREAM(ss) << "GETACTIVENEURONS nucleus [" << lookupNucleusName << "] found" << std::endl;
					globalObject->log(ss);

					// Interate through all neurons in this nucleus
					unsigned int cSize = (unsigned int) nuc->columns.size();
					LOGSTREAM(ss) << "GETACTIVENEURONS nucleus [" << lookupNucleusName << "] contains " << cSize << " columns " << std::endl;
					globalObject->log(ss);
					for (unsigned int i=0;i<cSize;i++)
					{
						long c = nuc->columns[i];
						Column *column = globalObject->columnDB.getComponent(c);
						//long colSize = (long) column->layers.size();
//						LOGSTREAM(ss) << "column [" << c << "] contains " << colSize << " layers " << std::endl;
//						globalObject->log(ss);

						long layerId = column->layers[layerNumber-1];
						Layer *layer = globalObject->layerDB.getComponent(layerId);


						unsigned int lcSize = (unsigned int) layer->clusters.size();
//						LOGSTREAM(ss) << "layer [" << layerId << "] contains " << lcSize << " clusters " << std::endl;
//						globalObject->log(ss);
						for (unsigned int j=0;j<lcSize;j++)
						{
							long clusterId = layer->clusters[j];
							Cluster *cluster = globalObject->clusterDB.getComponent(clusterId);
							unsigned int nSize = (unsigned int) cluster->getNeurons().size();
//							LOGSTREAM(ss) << "cluster [" << clusterId << "] contains " << nSize << " neurons " << std::endl;
//							globalObject->log(ss);
							for (unsigned int k=0;k<nSize;k++)
							{
								long neuronId = cluster->getNeurons()[k];
								// Got the neuronId
								// Convert neuronId and append it to return string
								// See if this neuron is active
								Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
//								if(neuron->isFiring())  // Only include this neuron if it is currently firing
								if(neuron->hasFired())  // Only include this neuron if it is currently firing
								{
									std::stringstream ns;
									ns << neuronId;
									std::string neuronIdString = ns.str();
									returnString += returnStringSeparator+neuronIdString;
									returnStringSeparator = ","; // Set returnStringSeparator for next time
									neuron->resetLatch();
								}
							}
						}
					}
//					LOGSTREAM(ss) << "Sending Response [" << returnString << "], " << returnString.size() << " bytes." << std::endl;
					LOGSTREAM(ss) << "Sending Response of " << returnString.size() << " bytes at timestep " << globalObject->getCurrentTimestamp() << "." << std::endl;
					globalObject->log(ss);
//					strncpy(sendbuf,returnString.c_str(),sendbuflength);
					return returnString;
				}
			}
			LOGSTREAM(ss) << "ERROR015: Nucleus " << lookupNucleusName << " not found: " << std::endl;
			globalObject->log(ss);
			returnString = "ERROR015: Nucleus ";
			returnString += lookupNucleusName+" not found";
			return returnString;
		} else if(command == "SETACTIVATIONPATTERN") {
//			LOGSTREAM(ss) << "SETACTIVATIONPATTERN Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
//			globalObject->log(ss);

			std::string returnString;

			// parameter string consists of list of neurons to activate (spike).
			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);

			std::string nucleus;
			int columns;
			int rows;

			std::vector<long> neurons;
			unsigned char *data = parseSetActivationPattern(workString,&nucleus, &columns, &rows, &neurons);

			size_t totalSize = columns * rows;

//			LOGSTREAM(ss) << "SETACTIVATIONPATTERN total rows*columns " << totalSize << ", of " << neurons.size() << " neurons " << std::endl;
//			globalObject->log(ss);

			for(size_t i=0;i<totalSize;i++)
			{
				if(i<neurons.size()) 
				{
					if(data[i]=='\1') 
					{
						long neuronId = neurons[i];

						Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
						if(neuron!=NULL) {
							if(globalObject->logEvents) 
							{
								ss << "server_parseandrespondtext_SETACTIVATIONPATTERN: neuron=" << neuronId;
								globalObject->writeEventLog(ss.str().c_str());
							}
							//neuron->fire();
							neuron->setMembranePotential(3.0);
						}
					} 
				}
			}

			returnString = "SETACTIVATIONPATTERN Command complete";
			return returnString;
		}
		else if (command == "GETACTIVATIONPATTERN") {
//			LOGSTREAM(ss) << "GETACTIVATIONPATTERN Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
//			globalObject->log(ss);

			std::string returnString("");

			// parameter string consists of list of neurons to activate (spike).
			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len + 1);
			strncpy(workbuffer, bufStart, len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);

			std::string nucleus(workString);

			std::vector<long> neurons = getNeurons(nucleus,6); // Layer 6 = output

			size_t totalSize = neurons.size();

			//			LOGSTREAM(ss) << "SETACTIVATIONPATTERN total rows*columns " << totalSize << ", of " << neurons.size() << " neurons " << std::endl;
			//			globalObject->log(ss);

			for (size_t i = 0; i<totalSize; i++)
			{
				long neuronId = neurons[i];
				Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
				if (neuron != NULL) {
					if (neuron->isFiring()) {
						returnString += "1";
					} else {
						returnString += "0";
					}
				} else {
						returnString += "0";
				}
			}

//			returnString = "SETACTIVATIONPATTERN Command complete";
			return returnString;
		} else if(command == "STARTATOMIC") { // STARTATOMIC command requires an argument (TRUE or FALSE)...
			LOGSTREAM(ss) << "STARTATOMIC Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
			globalObject->log(ss);

			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);

			std::string startAtomicParameter = workString; // STARTATOMIC parameter - TRUE OR FALSE 

			if(startAtomicParameter=="TRUE") 
			{
				LOGSTREAM(ss) << "STARTATOMIC 'true' option processed. " << std::endl;
				globalObject->log(ss);
			}
			else 
			{
				LOGSTREAM(ss) << "STARTATOMIC 'false' option processed. " << std::endl;
				globalObject->log(ss);
			}


			// STARTATOMIC functionality is not currently implemented - it is intended to pause the simulation until an ENDATOMIC command is received

			// get the region and nucleus names
			std::string returnString = "OK";

			return returnString;
		}
		else if (command == "FLUSH") { // FLUSH command requires no argument
		LOGSTREAM(ss) << "FLUSH Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
		globalObject->log(ss);

		globalObject->flush(); // flush all databases - persist all Databases to disc

		// get the region and nucleus names
		std::string returnString = "OK";

		return returnString;
		} else if(command == "REPORT") { // REPORT command requires a dummy argument which is ignored...
			LOGSTREAM(ss) << "REPORT Command recognized at timestep " << globalObject->getCurrentTimestamp() << ": " << std::endl;
			globalObject->log(ss);

			char *bufStart = space;
			size_t len = strlen(bufStart);
			char *workbuffer = (char *)globalObject->allocClearedMemory(len+1);
			strncpy(workbuffer,bufStart,len);
			std::string workString(workbuffer);
			globalObject->freeMemory(workbuffer);
			trim(workString);

			std::string reportParameter = workString; // Report parameter - currently ignored - could be used as report type in future implementations

			// get the region and nucleus names
			std::string returnString;
			Brain *brain = globalObject->brainDB.getValue();
			returnString = brain->getReport();

			return returnString;
		} else {
			LOGSTREAM(ss) << "Unrecognized command [" << command << "]" << std::endl;
			globalObject->log(ss);
			std::string returnString = "ERROR002: Unrecognized command";
			return returnString;
		}

	}

}

void Server::split(std::string workString,std::vector<long> &neuronList)
{
	// parse comma delimted string, convert each token to a long and add to neuronList
//	std::stringstream ss;

	std::string delimitedString;
	delimitedString = workString + ","; // Add final terminator
	size_t offset = 0;
	size_t pos = delimitedString.find(",",offset);
	while(pos != std::string::npos)
	{
		std::string strNeuronID;
		strNeuronID = delimitedString.substr(offset,pos-offset);
		trim(strNeuronID);
		if(strNeuronID.length()>0) 
		{
			long id = atol(strNeuronID.c_str());
			neuronList.push_back(id);

//			LOGSTREAM(ss) << " split neuron [" << id << "] added to returned vector " << std::endl;
//			globalObject->log(ss);
		}
		offset = pos + 1; // skip comma
		pos = delimitedString.find(",",offset);
//		LOGSTREAM(ss) << " pos [" << pos << "], offset [" << offset << "] " << std::endl;
//		globalObject->log(ss);
	}

}

unsigned char *Server::parseSetActivationPattern(std::string workString,std::string *nucleus, int *columns, int *rows, std::vector<long> *neurons)
{
	// parse comma delimted string, convert each token to a long and add to neuronList
	std::stringstream ss;

	std::string delimitedString;
	delimitedString = workString + ","; // Add final terminator
	size_t offset = 0;
	size_t pos = delimitedString.find(",",offset);
	std::vector<std::string> tokens;
	while(pos != std::string::npos)
	{
		std::string token;
		token = delimitedString.substr(offset,pos-offset);
		trim(token);
		tokens.push_back(token);
		offset = pos + 1; // skip comma
		pos = delimitedString.find(",",offset);
	}
	if(tokens.size()<4) 
	{
			LOGSTREAM(ss) << " parseSetActivationPattern unable to parse string [" << workString << "]. Only " << tokens.size() << " tokens found (4 required)." << std::endl;
			globalObject->log(ss);
			return NULL;
	}
	std::string nuc(tokens[0]);
	*nucleus = nuc;

	*columns = atoi(tokens[1].c_str());
	*rows = atoi(tokens[2].c_str());
	size_t totalSize = (*rows)*(*columns);
	unsigned char *data = (unsigned char *)globalObject->allocClearedMemory((int)totalSize);
	setActivationBytes((unsigned char *)tokens[3].c_str(), data, totalSize);
	std::vector<long> rneurons = getNeurons(nuc,4); // input layer
	*neurons = rneurons;
	return data;
}

std::vector<long> Server::getNeurons(std::string lookupNucleusName, int layerNumber) 
{
	std::stringstream ss;

	std::vector<long> neurons;

//	LOGSTREAM(ss) << "Server::getNeurons parameters are nucleus [" << lookupNucleusName << "] and layer [" << layerNumber << "] " << std::endl;
//	globalObject->log(ss);

	// Get the nucleus
//	for (globalObject->nucleusDB.begin(); globalObject->nucleusDB.more(); globalObject->nucleusDB.next())
//	{
//		Nucleus *nuc = globalObject->nucleusDB.getValue();
	long startNuclei = globalObject->componentBase[ComponentTypeNucleus];
	long endNuclei = globalObject->componentCounter[ComponentTypeNucleus];
//	for (globalObject->nucleusDB.begin(); globalObject->nucleusDB.more(); globalObject->nucleusDB.next())
	for(long nucIndex = startNuclei;nucIndex<endNuclei;nucIndex++)
	{
		Nucleus *nuc = globalObject->nucleusDB.getComponent(nucIndex);
		if(nuc->name == lookupNucleusName) 
		{
//			LOGSTREAM(ss) << "Server::getNeurons nucleus [" << lookupNucleusName << "] found" << std::endl;
//			globalObject->log(ss);

			// Interate through all neurons in this nucleus
			size_t cSize = nuc->columns.size();
//			LOGSTREAM(ss) << "Server::getNeurons nucleus [" << lookupNucleusName << "] contains " << cSize << " columns " << std::endl;
//			globalObject->log(ss);
			for (unsigned int i=0;i<cSize;i++)
			{
				long c = nuc->columns[i];
				Column *column = globalObject->columnDB.getComponent(c);
				//long colSize = (long) column->layers.size();
//				LOGSTREAM(ss) << "column [" << c << "] contains " << colSize << " layers " << std::endl;
//				globalObject->log(ss);

				long layerId = column->layers[layerNumber];
				Layer *layer = globalObject->layerDB.getComponent(layerId);


				size_t lcSize = layer->clusters.size();
//				LOGSTREAM(ss) << "layer [" << layerId << "] contains " << lcSize << " clusters " << std::endl;
//				globalObject->log(ss);
				for (unsigned int j=0;j<lcSize;j++)
				{
					long clusterId = layer->clusters[j];
					Cluster *cluster = globalObject->clusterDB.getComponent(clusterId);
					size_t nSize = cluster->getNeurons().size();
//					LOGSTREAM(ss) << "cluster [" << clusterId << "] contains " << nSize << " neurons " << std::endl;
//					globalObject->log(ss);
					for (unsigned int k=0;k<nSize;k++)
					{
						long neuronId = cluster->getNeurons()[k];
						// Got the neuronId
						neurons.push_back(neuronId);
					}
				}
			}
			return neurons;
		}
	}

//	LOGSTREAM(ss) << "Server::getNeurons " << neurons.size() << " neurons returned " << std::endl;
//	globalObject->log(ss);
	return neurons;
}

std::vector<long> Server::getNeurons(std::string lookupNucleusName, LayerType layerType) 
{
	std::stringstream ss;

	std::vector<long> neurons;



//	LOGSTREAM(ss) << "Server::getNeurons parameters are nucleus [" << lookupNucleusName << "] and layer [" << layerNumber << "] " << std::endl;
//	globalObject->log(ss);

	// Get the nucleus
	long startNuclei = globalObject->componentBase[ComponentTypeNucleus];
	long endNuclei = globalObject->componentCounter[ComponentTypeNucleus];
//	for (globalObject->nucleusDB.begin(); globalObject->nucleusDB.more(); globalObject->nucleusDB.next())
	for(long nucIndex = startNuclei;nucIndex<endNuclei;nucIndex++)
	{
		Nucleus *nuc = globalObject->nucleusDB.getComponent(nucIndex);
		if(nuc->name == lookupNucleusName) 
		{
			long c = nuc->columns[0];
			Column *column = globalObject->columnDB.getComponent(c);
			if(layerType==LayerType::input)
				return getNeurons(lookupNucleusName,column->inputLayer - 1);
			else 
				return getNeurons(lookupNucleusName,column->outputLayer - 1);
		}
	}
	return getNeurons(lookupNucleusName,0); // default to input layer
}


unsigned char Server::getActivationStatus(long neuronId) 
{
	Neuron *neuron = globalObject->neuronDB.getComponent(neuronId);
	if(neuron->hasFired())  // Only include this neuron if it is currently firing
		return '\1';
	else
		return '\0';
}

void Server::setActivationBytes(unsigned char *bitstring, unsigned char *data, size_t length)
{
	for(size_t i=0;i<length;i++) 
	{
		if(bitstring[i]=='1')
			data[i] = '\1';
		else
			data[i] = '\0';
	}
}
