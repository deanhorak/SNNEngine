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

#include "NeuronProcessor.h"

extern boost::mutex timestep_mutex;


// Settable externs
extern long FIRING_WINDOW;
extern long PROPAGATION_DELAY_MICROSECONDS;
extern double DECAY_FACTOR;
extern long REFACTORY_PERIOD;
extern float WEIGHT_GRADATION;
extern float RATE_GRADATION;

TimerProcessor::TimerProcessor(void)
{
    keepRunning = true;
}

TimerProcessor::~TimerProcessor(void)
{
}

void TimerProcessor::doWork(void)
{

//	globalObject->increment();
	globalObject->step();


//	startRealTime = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::ptime thisTime = boost::posix_time::microsec_clock::local_time();
    boost::posix_time::time_duration duration = thisTime - globalObject->startRealTime;

	long milliseconds = duration.total_milliseconds(); // + timeAdjust;
	long waittime = (globalObject->getCurrentTimestamp()  - milliseconds);  

	if(globalObject->getCurrentTimestamp() > milliseconds) 
	{
		//printf("wait %ld\n", waittime);
		if (waittime > STALL_OVERHEAD) {
/*
			if(globalObject->logEvents) 
			{	
				std::stringstream ss;
				ss << "Brain_step: waittime=" << waittime;
				globalObject->writeEventLog(ss.str().c_str());
			}
*/
			boost::this_thread::sleep(boost::posix_time::milliseconds(waittime));
/*
			if(globalObject->logEvents) 
			{	
				std::stringstream ss;
				ss << "Brain_step: waiting complete";
				globalObject->writeEventLog(ss.str().c_str());
			}
*/
		}
	}


}

int TimerProcessor::waitThread(void)
{
    int ret = waitThreadWorker();
    return ret;
}

int TimerProcessor::waitThreadWorker(void)
{

    pid_t tid = syscall(SYS_gettid);
    std::cout << "TimerProcessor thread is " << tid << std::endl;

    while (keepRunning)
    {
        doWork();
    }
    return 0;
}
void TimerProcessor::start(void)
{
    boost::thread t(&TimerProcessor::waitThread, this);
    t.detach(); // Don't Wait for the new thread to finish execution
}

void TimerProcessor::stop(void)
{
    keepRunning = false;
}
