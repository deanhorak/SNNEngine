#pragma once

class BrainDemo4
{
public:
	BrainDemo4(void);
	~BrainDemo4(void);
	static Brain *create(bool rebuild=true);
	static void step(Brain *brain);

};
