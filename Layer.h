#pragma once
#include <map>
#include "Cluster.h"
#include "NNComponent.h"

class Layer: public NNComponent
{
	Layer(unsigned long parentId);
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
		for(unsigned int i=0;i<clusters.size();i++)
		{
			ar & clusters[i];
		}
/*
		std::map<long,Cluster *>::iterator itCluster = clusters.begin();
		for (itCluster=clusters.begin(); itCluster!=clusters.end(); ++itCluster)
		{
			ar & itCluster->second;
		}
*/
	}

public:
	virtual ~Layer(void);
	static Layer *create(unsigned long parentId);
	void initializeRandom(unsigned long parentId);
	static Layer *instantiate(long key, size_t len, void *data);
	Tuple *getImage(void);

//	void connectTo(Layer *layer);
	void projectTo(Layer *layer, float sparsity=100.0f);
	void cycle(void);
	void toJSON(std::ofstream& outstream);

	std::vector<long> clusters;

	Location3D location;
	Size3D area;

private:
	void save(void);
	void commit(void);


};
