#pragma once

#include <map>

template <class a_Type> class CollectionIterator
{ 
private:
	std::map<long,a_Type> *coll;
	typename std::map<long,a_Type>::iterator it;
public:
	CollectionIterator(std::map<long,a_Type> *collection)
	{
		coll = collection;
		it = coll->begin();
	};
	~CollectionIterator(void)
	{
	};
	void begin(void) 
	{
		it = coll->begin();
	}
	bool more(void)
	{
		return it!=coll->end();
	}
	void next(void)
	{
		it++;
	}
	long key(void)
	{
		return it->first;
	}
	a_Type value(void)
	{
		return it->second;
	}


};
