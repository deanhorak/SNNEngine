#pragma once

class BrainDemo6
{
public:
	BrainDemo6(void);
	~BrainDemo6(void);
	static Brain *create(bool rebuild = true);
	static void step(Brain *brain);

};
