#pragma once
#include "NNComponent.h"
#include <vector>
#include "Neuron.h"

class Cluster: public NNComponent
{
    friend class boost::serialization::access;
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);

		for(unsigned int i=0;i<neurons.size();i++)
		{
			ar & neurons[i];
		}
	}

public:
	virtual ~Cluster(void);
	static Cluster *create(SpatialDetails details, unsigned long parentId);
	void initializeRandom(void);
	static Cluster *instantiate(long key, size_t len, void *data);
	Tuple *getImage(void);

//	void connectTo(Cluster *cluster);
	void projectTo(Cluster *cluster, float sparsity=100.f);
	void cycle(void);
	std::vector<long> &getNeurons() { return this->neurons; }

	void toJSON(std::ofstream& outstream);

	std::vector<long> neurons;

private:
	Cluster(unsigned long parentId);

	void save(void);
	void commit(void);


	Location3D location;
	Size3D area;

};
