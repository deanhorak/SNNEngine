#pragma once
class Size3D
{
public:
	Size3D(float h, float w, float d) { this->h = h; this->w = w; this->d = d; };

	Size3D(void) { this->h = 0; this->w = 0; this->d = 0; };

	void operator = (const Size3D& L) {
		h = L.h;
		w = L.w;
		d = L.d;
	}

	float h;
	float w;
	float d;
};

