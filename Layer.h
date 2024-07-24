/*
 * Proprietary License
 * 
 * Copyright (c) 2024 Dean S Horak
 * All rights reserved.
 * 
 * This software is the confidential and proprietary information of Dean S Horak ("Proprietary Information").
 * You shall not disclose such Proprietary Information and shall use it only in accordance with the terms
 * of the license agreement you entered into with Dean S Horak.
 * 
 * Redistribution and use in source and binary forms, with or without modification, are not permitted
 * without express written permission from Dean S Horak.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 * 
 * If you have any questions about this license, please contact Your Name at dean.horak@gmail.com.
 */

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
	void projectTo(Layer *layer, float sparsity=100.0f, float polarity=1.0);
	void cycle(void);
	void toJSON(std::ofstream& outstream);

	std::vector<long> clusters;

	Location3D location;
	Size3D area;

private:
	void save(void);
	void commit(void);


};
