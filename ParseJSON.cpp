#include <iostream>
#include <iomanip>
#include "Global.h"
#include "ParseJSON.h"
#include "TR1Random.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

Brain* brainPtr = NULL;
Brain* ParseJSON::loadFromJSON(void)
{

	tr1random = new TR1Random();
	globalObject = new Global();
	globalObject->brainDB.begin();
    Brain *brain = Brain::create(false);
    brainPtr = brain;

	// begin loading JSON
	std::string jsonfilename(std::string(DB_PATH) +  BRAINDEMONAME + std::string("/") + "serialized.json");

    // define parser callback
    json::parser_callback_t cb = [](int depth, json::parse_event_t event, json& parsed)
    {
        // skip object elements with key "Thumbnail"
        // Start with Brain level object
        int level = 0;

        std::string lastKey;
        std::string lastValue;

        switch (event) {
        case json::parse_event_t::object_start:
            return false;
            break;
        case json::parse_event_t::key:
            lastKey = parsed;
            return false;
            break;
        case json::parse_event_t::value:
            lastValue = parsed;
            if (lastKey == "id" ) 
            {
                if (level == 0) // Brain Level
                {
                    std::string::size_type sz;   // alias of size_t

                    long li_dec = std::stol(lastValue, &sz);
                    brainPtr->id = li_dec;
                }
            }
            return false;
            break;
        case json::parse_event_t::array_start:
            return false;
            break;
        case json::parse_event_t::array_end:
            return false;
            break;
        case json::parse_event_t::object_end:
            return false;
            break;
        default: 
            return true;
        }

    };

    // parse (with callback) and serialize JSON
    json j_filtered = json::parse(jsonfilename, cb);
    std::cout << std::setw(4) << j_filtered << std::endl;

	return brain;
}
