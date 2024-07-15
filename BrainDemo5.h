#pragma once

class BrainDemo5
{
public:
	BrainDemo5(void);
	~BrainDemo5(void);
	static Brain *create(bool rebuild=true);
	static void step(Brain *brain);

};
