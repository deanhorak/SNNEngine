#pragma once

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>
#include <vector>


class NeuronProcessor
{
public:
	NeuronProcessor(void);
	~NeuronProcessor(void);
	int waitThread(void);
	int waitThreadWorker(void);
	void doWork(void);
	void start(void);
	void stop(void);

   	bool keepRunning;

};
