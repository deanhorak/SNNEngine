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

// SNNEngine.cpp : Defines the entry point for the console application.
//

// Define the BrainDemo code for this run
#include "stdafx.h"
#include <stdlib.h>     
#include <stdio.h>
#include <unistd.h>
#include <map>
#include <iostream>
#include <boost/thread.hpp>
#include <boost/filesystem.hpp>
// #define the two following only in one source file prior to the #includes for Global and TR1Random
#define noexternglobal 1
#define noexterntr1random 1
#include "SNNEngine.h"
#include "Global.h"
#include "TR1Random.h"
#include "SNNVisualizer.h"
Global *globalObject;				// Global object
TR1Random *tr1random;		// TR1 Random object
SNNVisualizer *snnVisualizer;
Brain *brain;
bool keepRunning = true;
bool stillRunning = true;

//


#include "Brain.h"
#include "DetailTest.h"
#include "BrainDemoTiny.h"
#include "BrainUnitTest.h"

#include "ComponentCollection.h"

// The variables below can be changed on the fly via the SETVALUE rest interface command
// example JSON format is { "command": "SETVALUE", "name": "FIRING_WINDOW", "value": 10 }

// set initial FIRING WINDOW to slightly longer than refactory period
long FIRING_WINDOW = 10;

long PROPAGATION_DELAY_MICROSECONDS = 10;

double DECAY_FACTOR = 0.01;

long REFACTORY_PERIOD = 20;

// WEIGHT_GRADATION is used to slow the rate of change for weight over time (defined by n updates)
float WEIGHT_GRADATION = 100000.0f;

// RATE_GRADATION is used to slow the rate of change for rate over time (defined by n updates)
float RATE_GRADATION = 100000.0f;



inline bool exists(const std::string& name) {
	//    ifstream f(name.c_str());
	//	bool doesExist = f.good(); 

	boost::filesystem::path myfile(name);
	bool doesExist = boost::filesystem::exists(myfile);
	return doesExist;
}

void mainRoutine(void)
{

    pid_t tid = syscall(SYS_gettid);
    std::cout << "SNNEngine.mainRoutine thread is " << tid << std::endl;

	std::stringstream ss;
/*	
	bool rebuildFromScratch = false;
	bool rebuildFromJSON = false;
	std::string syncfilename(std::string(globalObject->getDBPath()) +  globalObject->getModelName() + std::string("/") + "syncpoint.txt");
	std::string jsonfilename(std::string(globalObject->getDBPath()) +  globalObject->getModelName() + std::string("/") + "serialized.json");

	rebuildFromScratch = !exists(syncfilename);
	//rebuildFromJSON = exists(jsonfilename);
	rebuildFromJSON = false;
	if (rebuildFromJSON) {
		brain = MODEL_IMPLEMENTATION_CLASS::createFromJSON();
	}
	else {
		brain = MODEL_IMPLEMENTATION_CLASS::create(rebuildFromScratch);
	}
*/
	LOGSTREAM(ss) << "Starting background threads..." << std::endl;
	globalObject->log(ss);

	brain->startServer();
	brain->startNeuronProcessing();
	brain->startTimerProcessing();

	bool showStatus = false;
	//	while(globalObject->getCurrentTimestamp()<10000)

	globalObject->startRealTime = boost::posix_time::microsec_clock::local_time();
	
	while (keepRunning)
	{

//		brain->step();
//		MODEL_IMPLEMENTATION_CLASS::step(brain);

		std::string msg = globalObject->getMessage();
		if (!msg.empty())
		{
			/// begin processing messages
			if (msg == "flush") {
				LOGSTREAM(ss) << "Beginning flush..." << std::endl;
					globalObject->log(ss);
					globalObject->flush();
					LOGSTREAM(ss) << "Flush complete..." << std::endl;
					globalObject->log(ss);
			}
			if (msg == "help") {
				printf("report, longreport, export, exit, shutdown, logfiring, nologfiring, time, showstatus, timesync, excite nnn \n");
			}

			if (msg == "report") {
				brain->report();
			}
			if (msg == "longreport") {
				brain->longReport();
			}
			if (msg == "export") {
				brain->exportJSON();
			}
			if (msg == "exit") {
				keepRunning = false;
			}
			if (msg == "shutdown") {
				keepRunning = false;
			}
			if (msg == "logfiring") {
				globalObject->setLogFiring = true;
				LOGSTREAM(ss) << "Fire logging ON..." << std::endl;
				globalObject->log(ss);
			}
			if (msg == "nologfiring") {
				globalObject->setLogFiring = false;
				LOGSTREAM(ss) << "Fire logging OFF..." << std::endl;
				globalObject->log(ss);
			}
			if (msg == "time" ) {
				// show time
				boost::posix_time::ptime nowTime = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration msdiff = nowTime - globalObject->startRealTime;
				long ct = globalObject->getCurrentTimestamp();
				long elapse = (long)msdiff.total_milliseconds() - brain->timeAdjust;
				float rate = (float)elapse / (float)ct;
				printf("timeslices %ld, elaspsed ms %ld, rate %f \n", ct, elapse, rate);
			}
			if (msg == "showstatus") {
				showStatus = !showStatus;
			}
			if (msg == "timesync") {
				boost::posix_time::ptime nowTime = boost::posix_time::microsec_clock::local_time();
				boost::posix_time::time_duration msdiff = nowTime - globalObject->startRealTime;
				long elapse = (long)msdiff.total_milliseconds();
				brain->timeAdjust = elapse - globalObject->getCurrentTimestamp();
				printf("timeAdjust set to %ld \n", brain->timeAdjust);
			}
			if (msg.substr(0,7) == "excite ") {
    			size_t spacePos = msg.find(' ');
    			if (spacePos == std::string::npos) {
        			std::cerr << "Invalid input format. Expected: \"keyword parameter\"" << std::endl;
    			} else {
					std::string parameterStr = msg.substr(spacePos + 1);
    				try {
        				// Convert the parameter string to an integer
        				int num = std::stoi(parameterStr);
						brain->excite(num);
	    			} catch (const std::invalid_argument& e) {
    	    			std::cerr << "Error converting parameter to int: " << e.what() << std::endl;
    				}
				}
			}
		}

		if (showStatus) {
			unsigned long ts = globalObject->getCurrentTimestamp();
			int intervalOffsetValue = ts % MAX_TIMEINTERVAL_BUFFER_SIZE;
			std::cout << "mainroutine Locking teVector_mutex[" << intervalOffsetValue << "]" << std::endl;
			boost::mutex::scoped_lock amx(*(globalObject->teVector_mutex[intervalOffsetValue]));
			std::vector<TimedEvent *> *teVector = &globalObject->timeIntervalEvents[intervalOffsetValue];
			int aps = teVector[intervalOffsetValue].size();;
			//int aps = (int)globalObject->timedEventSize();
			printf("%ld - [%d]. - %d\r", ts, intervalOffsetValue, aps);
			std::cout << "mainroutine UnLocking teVector_mutex[" << intervalOffsetValue << "]" << std::endl;
		}

	}



	LOGSTREAM(ss) << "Shutdown detected during simulation at timestep " << globalObject->getCurrentTimestamp() << "..." << std::endl;
	globalObject->log(ss);

	brain->stopNeuronProcessing();

	brain->stopServer();

	brain->shutdown();

	LOGSTREAM(ss) << "Background thread ended..." << std::endl;
	globalObject->log(ss);

	stillRunning = false;

}


//int main(int argc, char *argv[])
void SNNEngine::initialize(std::string dbPath, std::string modelName)
{
	std::stringstream ss;

	globalObject = new Global(dbPath,modelName);


	LOGSTREAM(ss) << "Begin execution..." << std::endl;
	globalObject->log(ss);

	bool rebuildFromJSON = false;
	globalObject->setModelName(modelName);
	globalObject->setDBPath(dbPath);
	std::string jsonfilename(dbPath +  modelName + std::string("/") + "serialized.json");

	std::string syncfilename(dbPath +  modelName + std::string("/") + "syncpoint.txt");
	bool rebuildFromScratch = !exists(syncfilename);

	//rebuildFromJSON = exists(jsonfilename);
	rebuildFromJSON = false;
	if (rebuildFromJSON) {
		brain = MODEL_IMPLEMENTATION_CLASS::createFromJSON();
		globalObject->readCounters();
	}
	else {
		brain = MODEL_IMPLEMENTATION_CLASS::create(rebuildFromScratch);
	}

	globalObject->writeCounters();

}

void SNNEngine::startEngine(void)
{
	std::stringstream ss;


	boost::thread t(&mainRoutine);


	std::locale loc;
	std::string command("");
	bool exit = false;

	LOGSTREAM(ss) << "Command line ready for input (x to terminate)..." << std::endl;
	globalObject->log(ss);

	SNNVisualizer *sv = new SNNVisualizer();
	sv->thisBrain = brain;

	LOGSTREAM(ss) << "Starting SNNVisualizer thread..." << std::endl;
	globalObject->log(ss);

	globalObject->brain->startSNNVisualizer();


	while (!exit)
	{
		//cin >> command;
		std::getline(std::cin, command);

		ss.str(""); ss.clear();
		LOGSTREAM(ss) << command << " command presented." << std::endl;
		globalObject->log(ss);


		ss.str(""); ss.clear();
		for (std::string::size_type i = 0; i<command.length(); ++i)
			ss << std::tolower(command[i], loc);
		command = ss.str();
		ss.str(""); ss.clear();
		LOGSTREAM(ss) << "Command [" << command << "] accepted..." << std::endl;
		globalObject->log(ss);

		if (command == "exit" || command == "shutdown")
		{

			ss.str(""); ss.clear();
			LOGSTREAM(ss) << command << " command accepted." << std::endl;
			globalObject->log(ss);

			globalObject->putMessage(command);

			while (stillRunning) {
    			usleep(5000); // usleep takes time in microseconds
			}

			ss.str(""); ss.clear();
			LOGSTREAM(ss) << command << " command processing Complete." << std::endl;
			globalObject->log(ss);
/*
			try {
				globalObject->closeFireLog();
			} catch(...) {}
			try {
				globalObject->closeDebugLog();
			} catch(...) {}
*/
			keepRunning = false;
			exit = true;
		}
		else
			if (command == "help")
			{
				globalObject->putMessage("help");
			}
			else if (command == "report")
			{
				globalObject->putMessage("report");
			}
			else if (command == "export")
			{
				globalObject->putMessage("export");
			}
			else if (command == "longreport")
			{
				globalObject->putMessage("longreport");
			}
			else if (command == "flush")
			{
				globalObject->putMessage("flush");
			}
			else if (command == "showstatus")
			{
				globalObject->putMessage("showstatus");
			}
			else if (command == "time")
			{
				globalObject->putMessage("time");
			}
			else if (command == "timesync")
			{
				globalObject->putMessage("timesync");
			}
			else if (command == "logfiring")
			{
				globalObject->putMessage("logfiring");
			}
			else if (command == "nologfiring")
			{
				globalObject->putMessage("nologfiring");
			}
			else if (command.substr(0,7) == "excite ")
			{
				globalObject->putMessage(command);
			}
			else
			{
				ss.str(""); ss.clear();
				LOGSTREAM(ss) << "Command not recognized." << std::endl;
				globalObject->log(ss);
			}
	}

	LOGSTREAM(ss) << "Termination requested..." << std::endl;
	globalObject->log(ss);

	// Ask thread to stop
	t.interrupt();

	// Join - wait when thread actually exits
	t.join();

	LOGSTREAM(ss) << "Main: thread ended..." << std::endl;
	globalObject->log(ss);

	return;
}

