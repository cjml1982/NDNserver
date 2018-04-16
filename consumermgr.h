#ifndef YL_BSDR_CONSUMERMGR
#define YL_BSDR_CONSUMERMGR

#include <ndn-cxx/face.hpp>
#include <thread>
#include <mutex>
#include <map>
#include "consumer.h"

using namespace std;

class ConsumerMgr
{
public:
	ConsumerMgr();
	~ConsumerMgr();

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
	std::thread *m_pthread;    // thread that receive br name
	std::unique_ptr<Face> m_pface;
	const ndn::RegisteredPrefixId * m_listenBrNameId;
	std::mutex m_resumeMutex;
	volatile bool m_shouldresume;

	std::vector<Consumer*> m_vecConumer;
	std::mutex m_vecConumerMtx;

	std::thread *m_pthrMonitor;    //
};

#endif	/* #ifndef YL_BSDR_CONSUMERMGR */
