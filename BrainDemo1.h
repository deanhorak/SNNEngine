#pragma once

class BrainDemo1
{
public:
	BrainDemo1(void);
	~BrainDemo1(void);
	static Brain *create(bool rebuild=true);
	static void step(Brain *brain);
};
