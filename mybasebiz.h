#ifndef _CMYBASEBIZ_H
#define _CMYBASEBIZ_H
#include <vector>
#include <mutex>
#include <condition_variable>
#include <unordered_map>
#include <queue>
#include <map>
#include <algorithm>    // std::remove
#include <thread>
#include "tinyxml/tinyxml.h"
#include "constant/myconstant.hpp"
#include "curl/mycurlpl.hpp"
#include "mysql/mymysqlpl.hpp"
#include "./config.h"
#include "mybiz.hpp"
#include "userauth/myjwt.hpp"

namespace ndn{

struct Key {
        std::string first;
        std::string second;
};

struct KeyHash {
	std::size_t operator()(const Key& k) const
	{
			return std::hash<std::string>()(k.first) ^
							(std::hash<std::string>()(k.second) << 1);
	}
};

struct KeyEqual {
	bool operator()(const Key& lhs, const Key& rhs) const
	{
			return lhs.first == rhs.first && lhs.second == rhs.second;
	}
};

class CMyBaseBIZ{

public:

	void 
	doRun(CQIHolder * holder);

	int 
	initMySQLPool();

	int
	getAddressesByAccount(CQIHolder * holder,
							std::string & sURL,
							std::string & sHeader,
							std::string & address);

	int
	getSendToAddressForRightV2Demo(CQIHolder *holder,std::string & sURL,
									std::string & sHeader,
									std::string & verification,
									std::string & address,
									std::string & txid,
									std::string amount,
									std::string chainid);

	int 
	postChainInfoFxServ(CQIHolder *holder,std::string sURL,std::string sHeader,RegChainInfo chainInfo);

	int 
	postChainInfoFxServ(CQIHolder *holder,std::string sURL,std::string sHeader,std::string tid,std::string resultCode);

protected:
	

	inline CMarket *
	getMarketFromHashMap(std::string &key){
		if(0 == marketmap.count(key)){	return NULL;}
				return  marketmap[key];
	}

	inline int 
	marketCount(std::string &key){	return marketmap.count(key);}

	//返回现在的market的balance
	inline int
	marketBalance(std::string &key){
		if(0 == marketmap.count(key)){	return 0;}
				return  (marketmap[key])->balance;

	}

	inline int 
	putMarketToHashMap(std::string &key,CMarket *cm){
			std::lock_guard<std::mutex> lg(decmtx);
			marketmap.insert(std::make_pair(key,cm));
			return 0;
	}

	inline int
	deleteMarket(std::string &mkt){
			std::lock_guard<std::mutex> lg(decmtx);
			CMarket *cm = marketmap[mkt];
			marketmap.erase(mkt);
			delete cm;
			cm = NULL;
			return 0;
	};


	int postRegFxServ(CQIHolder *holder,std::string sURL,std::string sHeader,std::string tid,S3FileInfo input);


	inline int 
	getACKey(std::string blocklabel){
		int keyAddr = getMyAddrNum(blocklabel);
		int keyChain = getMyChainNum(blocklabel);
		int ACKey = keyAddr*chainTotal+keyChain;
		return ACKey;
	}

	inline int
	getMyAddrNum(std::string blocklabel){
		
		return ndn::CMyOpenSSL::getInstance()->hash_string2(blocklabel)%nodeCount;
	}

	inline int
	getMyChainNum(std::string blocklabel){
		return ndn::CMyOpenSSL::getInstance()->hash_string(blocklabel)%chainTotal;
	}

	inline int 
	getSha256(std::string src,std::string &result){
			return  ndn::CMyOpenSSL::getInstance()->SHA256(src, result);//all params
	}

	inline int 
	checkGroupIdAndMarket(CDealUnit &unit){

		if(mylocalname!=unit.deviceGroupId||(0 == marketCount(unit.providerMarket))){ throw new CMy_BSDR_Exception("__GLOBAL_NO_MARKET_ERR",__GLOBAL_NO_MARKET_ERR);}
		unit.chainId = std::to_string(getMyChainNum(unit.blocklabel));
		return 0;
	}

	inline int
	setDigestFMap(std::string key,std::string value){
		std::unique_lock<std::mutex> ul(mtx_map);
		buff[key] = value;
		//cv_map.notify_all();
		return 0;
	}


	inline int
	writeChainQueueElement(RegChainInfo &ele){
		std::unique_lock<std::mutex> ul(mtx_queue);
		q.push(ele);
		cv.notify_one();
		return 0;
	};//解决重复插入的bug

	inline int 
    checkAuthToken(CQIHolder *holder,CAuthUser &user)
    {
         //michael 20180307 added /授权验证
       
			CJWTInputIPPort jwtipport;
			jwtipport.ip = (Config::Instance()->jwtcfg[0]).ip;//112
			jwtipport.port = (Config::Instance()->jwtcfg[0]).port;
			CJWTInputHeader jwthdr;
			jwthdr.hdr = "";
			CJWTInputParams jwtparams;
			jwtparams.user_id = user.virtualId;
			jwtparams.user_authtoken = user.authToken;

			int jwtrescode;
			std::string jwtres;
			CJWTClient::getInstance()->doJWTComm(JWT_TYPE::JWT_TYPE_AUTHVERIFY, \
												holder, \
												jwtipport,
												jwthdr,
												jwtparams,
												jwtrescode,
												jwtres);

			CJWTResponse jwtresponse;
			CJWTParser::getInstance()->parseJWTAuthBlob(JWT_TYPE::JWT_TYPE_AUTHVERIFY, \
														jwtres,
														jwtresponse);
		
	

      return 0;
    }
 


public:
		inline std::string
		getDigestFFormMap(const std::string key){
		
			if(0 == buff.count(key))
			{
				return std::string("");
			}
			else{
				return buff[key];
			}
  		
		}

		int
		readChainQueueElement();


		int 
		getVerif(std::string content,std::string digestf,std::string &output)
		{
			std::string output1;
			getSha256(content,output1);
			output1.append(digestf);
			getSha256(output1,output);
			return 0;
		}


		int 
		deleteDataAndFile(CQIHolder *holder,std::string blocklabel,std::string sChainId);

		int 
		deleteData(CQIHolder *holder,std::string blocklabel,std::string sChainId);


		int
		runReader();

		int 
		reChain();

		int 
		uploadMarket(CQIHolder *holder);


		int 
		initMysqlHolder(CQIHolder *holder);


		int 
		initWalletHashMap(const std::string & globalw);

		int 
		checkBlockLabelAndAvailableType(CQIHolder *holder,
										 CDealUnit &unit,
										 int &count,
										 int &result);

		int 
		sendToAddress(CQIHolder *holder,std::string & sURL,
                                            std::string & sHeader,
                                            std::string & hDigest,
                                            std::string & address,
                                            std::string & txid,
                                            std::string amount,
                                            std::string chainid);



		int 
		getGetBnRawTransaction(CQIHolder * holder,
								std::string & sURL,
								std::string & sHeader,
								std::string & address,
								std::string & txid,
								std::string & txContent);
	    int
	    checkChainData(CQIHolder *holder,CDealUnit unit,std::string &cursorTxId);


protected:

		inline int
		putToWalletArray(int i,
						std::string &url,
						std::string &header,
						std::string &dftaddr,
						std::string &chainid){

			if(i>=nodeTotal){
				return -1;
			}
			if(url.empty()||
				header.empty()||
				dftaddr.empty()||
				chainid.empty()){
				return -1;
			}
			std::lock_guard<std::mutex> lg(decmtx);
			CWallet *cw = new CWallet();
			cw->url = url;
			cw->header = header;
			cw->dftaddr = dftaddr; 
			cw->chainid = chainid;
			walletArray[i] = cw;
		
			

			//对非本地数据进行存储

			return 0;
		}




		inline int
		getFromWalletArray(int i,
						std::string &url,
						std::string &header,
						std::string &dftaddr,
						std::string &chainid){
			if(i>=nodeTotal){
				return -1;
			}
			CWallet *itv = walletArray[i];
			
			if(NULL == itv)
			{
				return -1;
			}
			url = itv->url;
			header = itv->header;
			dftaddr = itv->dftaddr;
			chainid = itv->chainid;
			return 0;
		}

		inline int
		updateMoney(CQIHolder *holder,CMarket *cm1,CMarket *cm2)
		{
			
			ndn::CMyMYSQLPL::getInstance()->updateMoney(holder,cm1,cm2);
			return 0;
		}



		inline int
		updateMarketInfo(CQIHolder *holder,CMarket cm){


			if((mylocalname != cm.deviceGroupId)||(0 == marketCount(cm.market)))
			{
			
				throw new CMy_BSDR_Exception("__GLOBAL_NO_MARKET_ERR",__GLOBAL_NO_MARKET_ERR);
			}
			CMarket *cmk = getMarketFromHashMap(cm.market);

			{
				int tmpbalance = cmk->balance;
				std::lock_guard<std::mutex> lg(mtx_market);
				cmk->balance +=cm.balance;
				if(0 > cmk->balance)
				{
					cmk->balance = 0;
				}

				try{

					ndn::CMyMYSQLPL::getInstance()->updateMarket(holder,cmk);	

				}catch(CMy_BSDR_Exception *e)
				{
					cmk->balance = tmpbalance;
					throw e;
				}
		
			}	
			
			return 0;
		};

		inline std::string writeResultJson(string key,string value){

				string res("\"");
				res.append(key);
				res.append("\"");
				res.append(":");
				res.append("\"");
				res.append(value);
				res.append("\"");
				
				return res;
		}

		


protected:
		std::vector<TiXmlElement*> getXMLVec();

		std::string getMyLocalName();

protected:

		std::unordered_map<Key, CWallet*, KeyHash, KeyEqual> cwalletmap;
		std::unordered_map<Key, CWallet*, KeyHash, KeyEqual> cbcrwalletmap;
		std::unordered_map<std::string,CMarket*> marketmap;
		CWallet **walletArray;
		std::mutex decmtx;
		std::vector<TiXmlElement*> nodeVec;
		int chainTotal;
		int nodeCount;
		int nodeTotal;
		std::string mylocalname;
		std::vector<std::string> wvec;
		std::queue<S3FileInfo> qs3fi;
		std::string fxHeader;
		std::string fxUrl;


protected:
		CWallet *myWallet[100];
		std::thread **mythread;
		std::mutex mtx;
		std::mutex vecmtx;
		std::mutex regmtx;
		std::mutex mtx_queue;
		std::queue<RegChainInfo> q;
		std::condition_variable cv;
		//the string* to prevent error
		std::map<std::string,std::string> buff;
		std::mutex mtx_map;
		std::condition_variable cv_map;
				
		std::mutex mtx_doRun;
		//更新market必须锁
		std::mutex mtx_market;
		//异步线程更新还是要加锁
		std::mutex mtx_curl;
		
		


	  

};

}
   
#endif //_CMYBASEBIZ_H

