#pragma once
#include "NNComponent.h"
#include <map>
#include "Layer.h"
#include "Location3D.h"
#include "Size3D.h"
#include "SpatialDetails.h"

class Column: public NNComponent
{
	Column(bool createLayers, unsigned long parentId);
	Column(bool createLayers, size_t layerCount, unsigned long parentId);
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
		for(unsigned int i=0;i<layers.size();i++)
		{
			ar & layers[i];
		}
	}
public:
	virtual ~Column(void);
	static Column *create(SpatialDetails details, unsigned long parentId);
	static Column *create(SpatialDetails details, size_t layerCount, unsigned long parentId);
	void initializeRandom(unsigned long parentId);
//	void initializeLayers(unsigned long parentId);
	static Column *instantiate(long key, size_t len, void *data);
	Tuple *getImage(void);

	void projectTo(Column *column, float sparsity=100.0f);
	void cycle(void);

	void toJSON(std::ofstream& outstream);


	std::vector<long> layers;

	Location3D location;
	Size3D area;

	int inputLayer = 1;
	int outputLayer = 6;

private:
	void save(void);
	void commit(void);


};
