#pragma once
class Location3D
{
public:
	Location3D(float x, float y, float z) { this->x = x; this->y = y; this->z = z; };
		
	Location3D(void) { this->x = 0; this->y = 0; this->z = 0; };

	void operator = (const Location3D& L) {
		x = L.x;
		y = L.y;
		z = L.z;
	}

	float x;
	float y;
	float z;
};

