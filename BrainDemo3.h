#pragma once

class BrainDemo3
{
public:
	BrainDemo3(void);
	~BrainDemo3(void);
	static Brain *create(bool rebuild=true);
	static void step(Brain *brain);

};
