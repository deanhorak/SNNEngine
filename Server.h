#pragma once

#include <algorithm> 
#include <functional> 
#include <cctype>
#include <locale>
#include <string>
#include <vector>


#include <cpprest/http_client.h>
#include <cpprest/filestream.h>

class Server
{
public:

	Server(void);
	~Server(void);
	void start(void);
	void stop(void);
	int waitThread(void);
	int waitThreadWorker(void);
	std::string parseAndRespond(char *buffer,int length);
	std::string parseAndRespondJSON(char *buffer, int length);
	std::string parseAndRespondXML(char *buffer, int length);
	std::string parseAndRespondText(char *buffer, int length);
	void split(std::string workString,std::vector<long> &neuronList);
	unsigned char *parseSetActivationPattern(std::string workString,std::string *nucleus, int *columns, int *rows, std::vector<long> *neurons);
	static std::vector<long> getNeurons(std::string lookupNucleusName, LayerType layerType);
	static std::vector<long> getNeurons(std::string lookupNucleusName, int layerNumber);
	void setActivationBytes(unsigned char *bitstring, unsigned char *data, size_t length);
	unsigned char Server::getActivationStatus(long neuronId);

	bool keepRunning;
	//
	// Utility methods

	// trim from start (in place)
	static inline void ltrim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not_fn(static_cast<int(*)(int)>(std::isspace))));
//            std::not1(std::ptr_fun<int, int>(std::isspace))));
	}

	// trim from end (in place)
	static inline void rtrim(std::string &s) {
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not_fn(static_cast<int(*)(int)>(std::isspace))).base(), s.end());
//            std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	}

	// trim from both ends (in place)
	static inline void trim(std::string &s) {
		ltrim(s);
		rtrim(s);
	}

	// trim from start (copying)
	static inline std::string ltrimmed(std::string s) {
		ltrim(s);
		return s;
	}

	// trim from end (copying)
	static inline std::string rtrimmed(std::string s) {
		rtrim(s);
		return s;
	}

	// trim from both ends (copying)
	static inline std::string trimmed(std::string s) {
		trim(s);
		return s;
	}
};
