#pragma once
#include <db_cxx.h>
#include <map>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/identity.hpp>
#include <boost/multi_index/member.hpp>
#include <string>
#include <typeinfo>
//#include "Global.h"
#include "CachedComponent.h"
#include "Neuron.h"
#include "Axon.h"
#include "Dendrite.h"
#include "Tuple.h"

extern char *ctypes[];

// 100 MB buffer for database
#define USER_BUFFER_SIZE 100000000 
#define LOGSTREAM(s) s.str(""); s.clear(); s 

extern long getGlobalTimestep();

using namespace boost::multi_index;

struct id{};
struct referenceTimestamp{};

class Global;

extern Global *globalObject;

template <class a_Type> class ComponentDB
{
	boost::mutex cache_mutex;
	boost::mutex db_mutex;
//	boost::mutex db_putmutex;

public:
	ComponentDB(const size_t cacheSize=5000)
   : db_(NULL, 0),               // Instantiate Db object
	maxCacheSize(cacheSize),
	cFlags_(DB_CREATE)          // If the database doesn't yet exist, allow it to be created.
	{
		for(size_t i=0;i<sizeof(cursorp);i++)
			cursorp[i] = NULL;
		componentCount = 0;
		reimage_count = 0;
		save_count = 0;
		cacheMiss_count = 0;

		try
		{
			// Redirect debugging information to std::cerr
			db_.set_error_stream(&std::cerr);
			db_.set_cachesize(0, 8 * 1024*1024,4); // Set cachesize to 8MB (4 caches of 2MB each - 8MB total)
			std::string typeidName(lastToken(typeid(a_Type).name()));
			std::string demoname(BRAINDEMONAME);

			std::string directoryPath(std::string(DB_PATH) +  BRAINDEMONAME);
			// Test for the presence of the directory and create it if not present. 

			// Check if the directory exists
    		if (access(directoryPath.c_str(), 0) == 0) {
        		struct stat status;
        		stat(directoryPath.c_str(), &status);
        		if (status.st_mode & S_IFDIR) {
            		//std::cout << "The directory " <<  directoryPath.c_str() << " exists." << std::endl;
        		} else {
            		std::cout << "The path " << directoryPath.c_str() << " is a file." << std::endl;
        		}
    		} else {
        		// Create the directory
        		if (mkdir(directoryPath.c_str(),0777) == 0) {
            		std::cout << "Directory " << directoryPath.c_str() << "created successfully." << std::endl;
        		} else {
            		std::cout << "Failed to create directory " <<  directoryPath.c_str() << std::endl;
        		}
    		}

			dbFileName_ = std::string(DB_PATH) +  BRAINDEMONAME + std::string("/") + demoname + std::string("_") + typeidName + std::string(".db"); 

			// Open the database
			db_.open(NULL, dbFileName_.c_str(), NULL, DB_BTREE, cFlags_, 0);
		}
		// DbException is not a subclass of std::exception, so we
		// need to catch them both.
		catch(DbException &e)
		{
			std::cerr << "Error opening database: " << dbFileName_ << "\n";
			std::cerr << e.what() << std::endl;
		}
		catch(std::exception &e)
		{
			std::cerr << "Error opening database: " << dbFileName_ << "\n";
			std::cerr << e.what() << std::endl;
		}

		buffer = (char *)globalObject->allocClearedMemory(USER_BUFFER_SIZE);
		if(buffer==NULL)
		{
			std::cout << "Unable to allocate working buffer! Press <enter> to continue." << std::endl;
			std::string response;
			std::cin >> response;
		}

	}

	~ComponentDB(void) { globalObject->freeMemory(buffer); close(); }

	inline Db &getDb() { return db_; }

	void insert(a_Type *component)
	{
		if (!globalObject->componentKeyInRange(component->id))
		{
			std::cerr << "insert():Error item: " << std::hex << component->id << "(" << std::dec << component->id << ") not found in " << dbFileName_ << " for insert().\n";
			return;
		}

		componentCount++;
		addToCache(component);
		save(component);
	}

	size_t getCacheCount(unsigned long cId) {
// DSH: mutex relaxation - experimental and dangerous 
//		boost::mutex::scoped_lock  amx(cache_mutex);
		return cache.count(cId);
	}

	bool isInCache(unsigned long cId) {
// DSH: mutex relaxation - experimental and dangerous 
//		boost::mutex::scoped_lock  amx(cache_mutex);
		try {
			auto ptr = cache.find(cId);
			if(ptr != cache.end())
				return true;
			else
				return false;
		} catch(...)
		{
			return false;
		}
	}

	CachedComponent<a_Type>* getCacheComponent(unsigned long cId) {
// DSH: mutex relaxation - experimental and dangerous 
//		boost::mutex::scoped_lock  amx(cache_mutex);
		return cache[cId];;
	}

	void cacheInsert(std::pair<long, CachedComponent<a_Type>*> pair) {
		boost::mutex::scoped_lock  amx(cache_mutex);
		cache.insert(pair);
	}

	void loadCache(void)
	{
		std::string typeidName(lastToken(typeid(a_Type).name()));
		std::cerr << "Loading: " << typeidName << " ...." << std::endl;

		typeidName = "ComponentType" + typeidName;
		size_t index = globalObject->getTypeIndex(typeidName);
		long base = globalObject->componentBase[index];
		long end = globalObject->componentCounter[index];
		if(base == 300000000) { // Nuclei
			long test = end;
		}
		for (long i = base; i < end; i++)
		{ 
			a_Type *component = getComponent(i); // this should cause the object to load
		}
		std::cerr << (end - base) << " " << typeidName << " items loaded." << std::endl;
	}

	void addToCache(a_Type *component)
	{

		if (!globalObject->componentKeyInRange(component->id))
		{
			std::cerr << "addToCache():Error item: " << std::hex << component->id << "(" << std::dec << component->id << ")  not found in " << dbFileName_ << " for addToCache().\n";
			return;
		}

		if (isInCache(component->id)) {
			CachedComponent<a_Type>* cc = cache[component->id];
			a_Type* comp = cc->component;
			if (comp == component) {
				cc->referenceTimestamp = getGlobalTimestep();
			}
			else {
				std::cout << "Duplicate cache item " << component->id << " attempted insert " << std::endl;
			}
			return;
		}

		CachedComponent<a_Type> *cc = new CachedComponent<a_Type>(component,getGlobalTimestep());

		std::pair<long, CachedComponent<a_Type>*> pair(component->id, cc);

		cacheInsert(pair);
/*
		size_t cSize = cache.size();
		while(cSize > maxCacheSize) 
		{
			//trimCache();
			removeFirstCache();
			cSize = cache.size();
		}
*/
	}

	void removeFirstCache(void)
	{
		boost::mutex::scoped_lock  amx(cache_mutex);
		typename std::map<long,CachedComponent<a_Type> *>::iterator itCache = cache.begin();
		CachedComponent<a_Type> *item = itCache->second;
		if (item != NULL) {
			if (item->component != NULL) {
				if (!item->component->isDirty() || ALWAYS_SAVE) {
					try {
						destroy(itCache->first);
					}
					catch (...) {}
				}
			}

		}

	}

	void trimCache(void)
	{
		boost::mutex::scoped_lock  amx(cache_mutex);
		// find oldest component, save if dirty, then remove from cache
		unsigned long oldestItem = 0;
		unsigned long oldestTimestamp = getGlobalTimestep();

		typename std::map<long,CachedComponent<a_Type> *>::iterator itCache;
		for (itCache = cache.begin(); itCache!=cache.end(); ++itCache)
		{
			CachedComponent<a_Type> *item = itCache->second;
			if(item->referenceTimestamp <= oldestTimestamp )
			{
				oldestTimestamp = item->referenceTimestamp;
				oldestItem = item->component->id;
			}
		}
		if(oldestItem!=0)
		{
//			destroy(oldestItem);
			destroyBulk(oldestTimestamp,100); // Bulk destroy 1000 at a time
		}

	}

	size_t getCacheMissCount() {
		return cacheMiss_count;
	}

	void destroyBulk(long timestamp, size_t count)
	{
		size_t delcount = 0;
		std::vector<long> removeIds;

		for (typename std::map<long,CachedComponent<a_Type> *>::iterator itCache = cache.begin(); itCache!=cache.end(); ++itCache)
		{
			CachedComponent<a_Type> *item = itCache->second;
			if(item->referenceTimestamp == timestamp ) 
			{
				removeIds.push_back(item->component->id);
//				destroy(item->component->id);
				delcount++;
				if(delcount >= count) 
					break;
			}
		}
		for(size_t i=0;i<removeIds.size();i++)
		{
			try {
				destroy(removeIds[i]);
			}
			catch (...) {}
		}

	}

	void destroy(long itemId)
	{
/*
		if(itemId==1000500000L) 
		{
			std::cout << "Synapse 1000500000 being destroyed" << std::endl;
		}
*/
		try {
			size_t c = cache.count(itemId);
			if (c == 0) {
				std::cout << "Unable to find item " << std::hex << itemId << " in cache vector for destroy()." << std::endl;
				return;
			}

			CachedComponent<a_Type>* cacheItem = cache[itemId];

			a_Type* thisComponent = cacheItem->component;
			if (thisComponent->isDirty() || ALWAYS_SAVE)
			{
				save(thisComponent);
			}


			eraseCache(itemId);
			delete cacheItem;
			delete thisComponent;
		}
		catch (...) 
		{
			std::cout << "Error destroying " << std::hex << itemId << " from cache for destroy()." << std::endl;

		}
	}

	void eraseCache(long itemId)
	{
		boost::mutex::scoped_lock  amx(cache_mutex);

		try {
			size_t c = cache.count(itemId);

			if (c > 0) {
				cache.erase(itemId);
			}
			else {
				std::cout << "Unable to find item " << std::hex << itemId << " in cache (eraseCache) " << std::endl;
			}
		}
		catch (...) {

		}
	}
/*
	int doDbPut(DbTxn *a,Dbt *b,Dbt *c,u_int32_t d) {
		boost::mutex::scoped_lock  amx(db_putmutex);
		return db_.put(a, b, c, d); // put a wrapper on the put to ensure it's single threaded
	}
*/
	void save(a_Type *component)
	{
//		boost::mutex::scoped_lock  amx(db_mutex);

		if (!globalObject->componentKeyInRange(component->id))
		{
			std::cerr << "save():Error item: " << std::hex << component->id << "(" << std::dec << component->id << ") not found in " << dbFileName_ << " for save().\n";
			return;
		}

		try {

			save_count++;

			Tuple *tuple = component->getImage();


			Dbt ky, dat;

			long thiskey = component->id;

			char *dataPointer = tuple->objectPtr;
			u_int32_t dataLen = tuple->value;

			ky.set_data(&thiskey);
			ky.set_size(sizeof(long));
			ky.set_ulen(sizeof(long));
			ky.set_flags(DB_DBT_USERMEM);

			dat.set_data(dataPointer);
			dat.set_size(dataLen);
			dat.set_ulen(dataLen);
			dat.set_flags(DB_DBT_USERMEM);

			//Dbt mykey(&(component->id), sizeof(component->id)); // Assuming component->id is a long

			// Dbt data(tuple->objectPtr, tuple->value);

//			boost::mutex::scoped_lock  amx(db_putmutex);
			boost::mutex::scoped_lock  amx(db_mutex);

			int ret = db_.put((DbTxn *)NULL, &ky, &dat, 0); // put a wrapper on the put to ensure it's single threaded
			if (ret != 0) 
			{
     		   std::cerr << "ERROR: db_.put failed: " << db_strerror(ret) << std::endl;
        		// You can add more error handling logic here, such as throwing an exception
        		// or handling specific error codes differently
        		// throw std::runtime_error(db_strerror(ret));
    		}
			else 
			{
				component->setDirty(false);
			}
			globalObject->freeMemory(dataPointer);
			delete tuple;
		} catch (DbException &ex) {
			std::cout << "DB Exception encountered: " << ex.what() << " !!! " << std::endl;
		}
	}

	size_t size(void)
	{
		return componentCount;
	}

	size_t cacheSize(void)
	{
		return cache.size();
	}

	size_t saves(void)
	{
		return save_count;
	}

	size_t reimages(void)
	{
		return reimage_count;
	}


	void begin(int cursorno=0)
	{
		if (cursorp[cursorno] != NULL) 
			(cursorp[cursorno])->close();
		db_.cursor(NULL, &cursorp[cursorno], 0);
		int ret = (cursorp[cursorno])->get(&keyValue[cursorno], &data[cursorno], DB_NEXT);
		if (ret == DB_NOTFOUND) 
			moreData[cursorno] = false;
		else
			moreData[cursorno] = true;
	}

	void end(int cursorno=0)
	{
		if (cursorp[cursorno] != NULL) 
			(cursorp[cursorno])->close();
		cursorp[cursorno] = NULL;
	}

	bool more(int cursorno=0)
	{
		return moreData[cursorno];
	}


	void next(int cursorno=0)
	{
		if (cursorp[cursorno] == NULL) 
			begin();
		int ret = (cursorp[cursorno])->get(&keyValue[cursorno], &data[cursorno], DB_NEXT);
		if (ret == DB_NOTFOUND) 
			moreData[cursorno] = false;
		else
			moreData[cursorno] = true;
	}

	a_Type *getValue(int cursorno=0)
	{
		//Dbt *thisdata = &data[cursorno];
		Dbt *thiskey = &keyValue[cursorno];

		long mykey;
		memcpy(&mykey,(char *)thiskey->get_data(),sizeof(long));

		return getComponent(mykey);
	}

	long key(int cursorno=0)
	{
		long *data = (long *)keyValue[cursorno].get_data();
		long keyVal = *data;
		return keyVal;
	}

	
	a_Type* getDBComponent(long key) {


		a_Type* object = NULL;

		if(!globalObject->componentKeyInRange(key))
		{
			std::cerr << "getDBComponent(1):Error item: " << std::hex << key << "(" << std::dec << key << ") not found in " << dbFileName_ << " in getDBComponent()\n";
			return object;
		}

		Dbt ky, dat;

		long thiskey = key;

		ky.set_data(&thiskey);
		ky.set_size(sizeof(long));
		ky.set_ulen(sizeof(long));
		ky.set_flags(DB_DBT_USERMEM);

		dat.set_data(buffer);
		dat.set_size(USER_BUFFER_SIZE);
		dat.set_ulen(USER_BUFFER_SIZE);
		dat.set_flags(DB_DBT_USERMEM);


		boost::mutex::scoped_lock  amx(db_mutex);
		int ret = db_.get(NULL, &ky, &dat, 0);

		if (ret == DB_NOTFOUND)
		{
			//				printf("getComponent NOT found key %d in database\n",key);
			std::cerr << "getDBComponent(2):Error item: " << std::hex << key << "(" << std::dec << key << ") not found in " << dbFileName_ << "\n";
		}
		else
		{
			//				printf("getComponent found key %d in database\n",key);
			reimage_count++;
			u_int32_t len = dat.get_ulen();
			char *data = dat.get_data();
			object = a_Type::instantiate(key, len, data);
			object->setDirty(false);
			/*
							if(object->id==1001200000L)
							{
								Synapse *s = (Synapse *)object;
								std::cout << "Synapse 1001200000L found in database in ::getComponent and owneing process is " << s->getOwningProcessComponentId() << std::endl;
							}
			*/
		}
		return object;
	}

	a_Type* getDBComponentNolock(long key) {


		a_Type* object = NULL;

		if(!globalObject->componentKeyInRange(key))
		{
			std::cerr << "getDBComponent(1):Error item: " << std::hex << key << "(" << std::dec << key << ") not found in " << dbFileName_ << " in getDBComponent()\n";
			return object;
		}

		if(key == 800000000)
		{
			std::cout << "here" << std::endl;
		}

		Dbt ky, dat;

		long thiskey = key;

		ky.set_data(&thiskey);
		ky.set_size(0);
		ky.set_ulen(sizeof(long));
		ky.set_flags(DB_DBT_USERMEM);

		dat.set_data(buffer);
		dat.set_size(0);
		dat.set_ulen(USER_BUFFER_SIZE);
		dat.set_flags(DB_DBT_USERMEM);


		// nolock boost::mutex::scoped_lock  amx(db_mutex);
		int ret = db_.get(NULL, &ky, &dat, 0);

		if (ret == DB_NOTFOUND)
		{
			//				printf("getComponent NOT found key %d in database\n",key);
			std::cerr << "getDBComponent(2):Error item: " << std::hex << key << "(" << std::dec << key << ") not found in " << dbFileName_ << "\n";
		}
		else
		{
			//				printf("getComponent found key %d in database\n",key);
			reimage_count++;
//			if (key == 800000256) {
//				int breakhere = 0;
//			}
			u_int32_t len = dat.get_ulen();
			char *data = dat.get_data();
			object = a_Type::instantiate(key, len, data);
			object->setDirty(false);
			/*
							if(object->id==1001200000L)
							{
								Synapse *s = (Synapse *)object;
								std::cout << "Synapse 1001200000L found in database in ::getComponent and owneing process is " << s->getOwningProcessComponentId() << std::endl;
							}
			*/
		}
		return object;
	}


	a_Type* getComponent(long key)
	{
		a_Type* object = NULL;

		if (!globalObject->componentKeyInRange(key))
		{
			std::cerr << "getComponent():Error item: " << std::hex << key << "(" << std::dec << key << ") not found in " << dbFileName_ << " or cache.\n";
			return object;
		}


		if (isInCache(key)) {
			CachedComponent<a_Type>* cc = getCacheComponent(key); //cache[key];
			cc->referenceTimestamp = getGlobalTimestep();
			object = cc->component;
		}
		else 
		{
			cacheMiss_count++;

			object = getDBComponent(key);
			if(object != NULL) {
				addToCache(object);
			}
		}

		return object;
	}

	void flush(void)
	{
		std::stringstream ss;
		long itemId = 0;

		// Flush any dirty objects in cache
//		for (typename std::map<long,CachedComponent<a_Type> *>::iterator itCache = cache.begin(); itCache != cache.end(); ++itCache)
//		{
		for (const auto& pair : cache) {
			itemId = pair.first;
			CachedComponent<a_Type> *item = pair.second;
			if(item->component->isDirty())
			{
				save(item->component);
			}
		}

		db_.sync(0);

	//	LOGSTREAM(ss) << "flush():Last item of flush:  " << itemId  << std::endl;
	//	globalObject->log(ss);
	}

	void flushAll(void)
	{
		// Flush any dirty objects in cache
		std::stringstream ss;
		LOGSTREAM(ss) << "Flushall for  " << dbFileName_ << " beginning..." << std::endl;
		globalObject->log(ss);

		size_t sz = cache.size();
		size_t ix = 0;
		size_t lastPct = 1000;

		long itemId = 0;

//		for (typename std::map<long, CachedComponent<a_Type>*>::iterator itCache = cache.begin(); itCache != cache.end(); ++itCache)
//		{
		for (const auto& pair : cache) {
			itemId = pair.first;
			CachedComponent<a_Type>* item = pair.second;
			save(item->component);
			ix++;
			size_t pct = (ix*100) / sz;
			if (pct != lastPct) {
				lastPct = pct;
				printf("    %d %% complete...\r", (int)pct);
			}
		}

		db_.sync(0);

		LOGSTREAM(ss) << "Flushall():Last item of flush:  " << itemId  << std::endl;
		globalObject->log(ss);

		LOGSTREAM(ss) << "Flushall for  " << dbFileName_ << " Complete." << std::endl;
		globalObject->log(ss);
	}

	void shutdown(void)
	{
		std::stringstream ss;
		long itemId = 0;
		std::vector<long> removeIds;

//		for (typename std::map<long,CachedComponent<a_Type> *>::iterator itCache = cache.begin(); itCache != cache.end(); ++itCache)
//		{
		for (const auto& pair : cache) {
    		itemId = pair.first;
			removeIds.push_back(itemId);
			CachedComponent<a_Type> *item = pair.second;
			if(item->component->isDirty())
			{
				save(item->component);
			}
			a_Type *object = item->component;
			delete item;
			delete object;
		}
//		for(size_t i=0;i<removeIds.size();i++)
//		{
//			eraseCache(removeIds[i]);
//		}
		std::string typeidName(lastToken(typeid(a_Type).name()));
		LOGSTREAM(ss) << "shutdown(): Last " << typeidName << " item in cache to flush:  " << itemId << std::endl;
		globalObject->log(ss);

	}

	void close()
	{
		// Close the db
		try
		{
			db_.close(0);
			std::cout << "Database " << dbFileName_
				      << " is closed." << std::endl;
		}
		catch(DbException &e)
		{
			std::cerr << "Error closing database: " << dbFileName_ << "\n";
			std::cerr << e.what() << std::endl;
		}
		catch(std::exception &e)
		{
			std::cerr << "Error closing database: " << dbFileName_ << "\n";
			std::cerr << e.what() << std::endl;
		}
	} 

	char *lastToken(const char *in) 
	{
		char *ptr = (char *) (in + strlen(in));
		while (ptr > in && *ptr != ' ')
			ptr--;
		if (*ptr == ' ') 
			ptr++;
		return ++ptr;
	}

	size_t componentCount;

private:

    Db db_;
    std::string dbFileName_;
    u_int32_t cFlags_;
	Dbc *cursorp[10];
	bool moreData[10];
    Dbt keyValue[10], data[10];
	std::map<long,CachedComponent<a_Type> *> cache;
/*
	typedef multi_index_container<
		CachedComponent<a_Type> *,
		indexed_by<
		ordered_unique<tag<id>, BOOST_MULTI_INDEX_MEMBER(CachedComponent<a_Type> *, unsigned long, CachedComponent<a_Type>::id)>,
			ordered_non_unique<tag<referenceTimestamp>, BOOST_MULTI_INDEX_MEMBER(CachedComponent<a_Type> *, unsigned long, CachedComponent<a_Type>::referenceTimestamp)>
				>
		> cache;
*/
	size_t maxCacheSize;
	char *buffer;
    // We put our database close activity here.
    // This is called from our destructor. In
    // a more complicated example, we might want
    // to make this method public, but a private
    // method is more appropriate for this example.
//    void close();
	size_t reimage_count;
	size_t save_count;
	size_t cacheMiss_count;

};
