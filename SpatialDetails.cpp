#include "TR1Random.h"
#include "SpatialDetails.h"


void SpatialDetails::randomizeLocation() {
	// TODO:
	// using the area as the dimensions geneate a random location somewhere within those dimensions
	// convert the dimensions into integers required by the randomizer, by multiplying by 1000, giving us 1/1000 resolution
	location.x = ((float)(tr1random->generate(1, (int)(area.w * 1000))) / 1000.0f);
	location.y = ((float)(tr1random->generate(1, (int)(area.h * 1000))) / 1000.0f);
	location.z = ((float)(tr1random->generate(1, (int)(area.d * 1000))) / 1000.0f);

}