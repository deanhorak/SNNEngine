#pragma once

class BrainDemo
{
public:
	BrainDemo(void);
	~BrainDemo(void);
	static Brain* create(bool rebuild = true);
	static Brain* createFromJSON(void);
	static void step(Brain* brain);
	static std::string formatNumber(unsigned long long number);
	static void insertSynapses(Nucleus* nuc);
	static void insertSynapses(Nucleus* nucA, Nucleus* nucB);
	static void finalAdjustments(std::stringstream& ss);
};
