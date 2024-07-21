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
#include "Column.h"
#include "ColumnNeuronProfile.h"
#include "Location3D.h"
#include "Size3D.h"
#include "SpatialDetails.h"


class Nucleus: public NNComponent
{
	Nucleus(int nucleusType);
    friend class boost::serialization::access;
    // When the class Archive corresponds to an output archive, the
    // & operator is defined similar to <<.  Likewise, when the class Archive
    // is a type of input archive the & operator is defined similar to >>.
    template<class Archive>
    void serialize(Archive & ar, const size_t version)
	{
		ar & boost::serialization::base_object<NNComponent>(*this);
		for(unsigned int i=0;i<columns.size();i++)
		{
			ar & columns[i];
		}
/*
		std::map<long,Column *>::iterator itColumn = columns.begin();
		for (itColumn=columns.begin(); itColumn!=columns.end(); ++itColumn)
		{
			ar & itColumn->second;
		}
*/
	}
public:
	virtual ~Nucleus(void);
	static Nucleus *create(std::string name, SpatialDetails details, int nucleusType= INTER_NUCLEUS, bool setToDirty=true);
	void initializeRandom(void);

	static Nucleus *instantiate(long key, size_t len, void *data);
	Tuple *getImage(void);

//	void connectTo(Nucleus *nucleus);
	void projectTo(Nucleus *nucleus, float sparsity=100.0f);
	void cycle(void);

	void addColumns(size_t colCount, size_t clusterCount, size_t neuronCount);
	void addColumns(size_t colCount, size_t layerCount, size_t clusterCount, size_t neuronCount);
	void addColumns(size_t colCount, ColumnNeuronProfile &cProfile);

	void toJSON(std::ofstream& outstream);


	std::string name;

	std::vector<long> columns;

	Location3D location;
	Size3D area;

	int nucleusType;

private:
	void save(void);
	void commit(void);

};
