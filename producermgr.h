#ifndef YL_BSDR_PRODUCERMGR
#define YL_BSDR_PRODUCERMGR

#include <string>
#include <vector>
#include <map>
#include <thread>
#include <time.h>
#include "producer.h"


using namespace std;

class ProducerMgr
{
public:
	ProducerMgr();
	~ProducerMgr();
public:
	bool run();
	void stop();
private:
	void listenBrName();
	void onInterest(const InterestFilter& filter, const Interest& interest);
	void onRegisterSuccess(const Name& profix);
	void onRegisterFailed(const Name& profix, const std::string& reason);
	void monitor();
private:
	std::thread *m_pthread;    // thread that receive bsdr name

	std::unique_ptr<Face> m_pface;
	const ndn::RegisteredPrefixId * m_listenBrNameId;

	std::mutex m_resumeMutex;
	volatile bool m_shouldresume;

	std::vector<Producer *> m_vecPro;
	std::mutex m_vecProMutex;
	std::map<std::string, time_t> m_brname2t;
	std::mutex m_brname2tMutex;

};


#endif	/* #ifndef YL_BSDR_PRODUCERMGR */
