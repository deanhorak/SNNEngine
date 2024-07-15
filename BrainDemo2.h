#pragma once

class BrainDemo2
{
public:
	BrainDemo2(void);
	~BrainDemo2(void);
	static Brain *create(bool rebuild=true);
	static void step(Brain *brain);
};
