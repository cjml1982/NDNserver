/*
 * BsdrNamePub.h
 *
 *  Created on: 2017年6月29日
 *      Author:
 */

#ifndef YL_BSDR_BSDRNAMEPUB
#define YL_BSDR_BSDRNAMEPUB

#include <thread>
#include <ndn-cxx/face.hpp>

using namespace std;
using namespace ndn;

class BsdrNamePub {
public:
	BsdrNamePub();
	~BsdrNamePub();

public:
	bool run();
	void doRun();
	void stop();
private:
	void onData(const Interest& interest, const Data& data);
	void onNack(const Interest& interest, const lp::Nack& nack);
	void onTimeout(const Interest& interest);

private:
	bool m_shouldResume;
	std::thread * m_thr;

	std::unique_ptr<Face> m_pface;
};

#endif /* YL_BSDR_BSDRNAMEPUB */
