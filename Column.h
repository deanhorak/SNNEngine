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
