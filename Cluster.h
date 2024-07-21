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
