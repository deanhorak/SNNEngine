#pragma once
#include "Location3D.h"
#include "Size3D.h"

class SpatialDetails
{
public:
	//SpatialDetails(void) {} // default constructor
	SpatialDetails(const Location3D& location, const Size3D& area) {
		this->location = location;
		this->area = area;
	};

	SpatialDetails(float x, float y, float z, float h, float w, float d) {
		Location3D loc(x, y, z);
		Size3D sz(h, w, d);
		this->location = loc;
		this->area = sz;
	};

	void randomizeLocation(void);

	Location3D location;
	Size3D area;

};

