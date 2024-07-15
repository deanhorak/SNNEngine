#pragma once
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>

#include "NNComponent.h"

class Process: public NNComponent
{
public:
	Process(ComponentType type=ComponentTypeUnknown);
	virtual ~Process(void);
	inline float getDistance(void) { return distance; };
	inline void setDistance(float value) { 
		if (value < 0) {
			value = 0.1f;  // Kludge to prevent negaive index
		}
		distance = value; 
		setDirty(true); 
	};
	inline float getRate(void) { return rate; };
	 void setRate(float value); 

	float rateUpdateCounter;

private:
	float distance = 0;
	float rate = 0;

	boost::mutex process_mutex;

};
