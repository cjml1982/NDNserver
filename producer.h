#ifndef YL_BSDR_PRODUCER
#define YL_BSDR_PRODUCER

#include <ndn-cxx/face.hpp>
#include <ndn-cxx/security/key-chain.hpp>

#include <string>
#include <vector>
#include <thread>
#include <mutex>

using namespace std;
using namespace ndn;

class Producer
{
public:
	Producer();
	Producer(string brname, int pronum);
	~Producer();

public:
	bool run();
	void startService();
	void stop();

	string getBrName();
	int getProNum();
private:
	void onInterest(const InterestFilter& filter, const Interest& interest);
	void onRegisterFailed(const Name& profix, const std::string& reason);
public:
	std::unique_ptr<Face> m_pface;
	KeyChain m_keyChain;

	string m_brname;
	int m_pronum;

	std::thread * plistenThr;

	const ndn::RegisteredPrefixId * m_listenId;

	std::mutex m_resumeMutex;
	std::mutex m_myself;
	volatile bool m_shouldresume;
};






#endif	/* #ifndef YL_BSDR_PRODUCER */