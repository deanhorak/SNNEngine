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
 * without express written permission from Your Name.
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
