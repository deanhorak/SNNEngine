#pragma once

class BrainDemoHWRecognition
{
public:
	BrainDemoHWRecognition(void);
	~BrainDemoHWRecognition(void);
	static Brain *create(bool rebuild=true);
	static Brain* createFromJSON(void);
	static void step(Brain *brain);
	static std::string formatNumber(unsigned long long number);
	static void insertSynapses(Nucleus *nuc);
	static void insertSynapses(Nucleus* nucA, Nucleus* nucB);
	static void finalAxonAdjustments(std::stringstream &ss);
	static void finalDendriteAdjustments(std::stringstream &ss);
};
