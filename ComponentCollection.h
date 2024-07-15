#pragma once
#include <string>

#include <iostream> 
#include <stdlib.h> 
#include <memory.h> 
#include <fstream>

using namespace std;

#include "Global.h"

template <class a_Type> class ComponentCollection
{
public:
	ComponentCollection(std::string componentname, size_t cacheSize=1000)
	{
		cache = 0L;
		cursorPosition = 0;
		elementSize = sizeof(a_Type);
		this->cacheSize = cacheSize;
		this->componentname = componentname+".dat";
	};

	~ComponentCollection(void){};

	void begin(void)
	{
		cursorPosition = 0;
	};

	void next(void)
	{
		cursorPosition++;
	};

	void open(void)
	{
		begin();
		if(cache!=0L)
			delete cache;
		cache = new char[cacheSize * elementSize];
		cacheIndex = new unsigned long[cacheSize];
		unsigned long offset = 0;
		file.open(componentname.c_str(), ios::binary | ios::out | ios::in);
		for(size_t i=0;i<cacheSize;i++)
		{
			cacheIndex[i] = i;
			cacheTimestamp[i] = globalObject->current_timestep;
//			file.seekp(offset,ios::beg);
			file.read(cache+(offset*elementSize), elementSize);
		}
//		file.close();
	};

	bool isInCache(long id)
	{
		return false;
	}

	void insert(a_Type *component)
	{
		long id = component->id;
		if(isInCache(id))
		{
//			long offset = id - globalObject->componentBase[a_type->componentType];
//			file.write(buff, elementSize); // sizeof can take a type
		} 
		else 
		{
		}

	};

	void format(size_t numberOfElements)
	{
		cout << "Formatting " << componentname << ", " << numberOfElements << " elements at " << elementSize << " bytes each. " << endl;
		char *buff = new char[elementSize];
		memset(buff,0,elementSize);
		file.open(componentname.c_str(), ios::binary | ios::out);
		for(size_t i=0;i<numberOfElements;i++)
		{
			file.write(buff, elementSize); // sizeof can take a type
		}
		file.close();
	};

	std::string componentname;
	size_t elementSize;
	size_t cacheSize;
	size_t cursorPosition;
	char *cache;
	unsigned long *cacheIndex;
	unsigned long *cacheTimestamp;
	fstream file;
};
