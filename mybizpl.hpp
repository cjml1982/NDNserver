
#ifndef MYBIZPL_HPP
#define MYBIZPL_HPP

#include <unistd.h>

#include "tinyxml/myxml.hpp"

#include "openssl/myopenssl.hpp"
//begin 20170601 liulijin add
#include "mybasebiz.h"

#ifdef __cplusplus
extern "C" {
#endif

namespace ndn {


  struct ProducerData
  {
    string strOperType; // 取值为上面的那些静态变量
    string param; //参数
  };
  class CMyBIZPL:public CMyBaseBIZ {

  public:

    CMyBIZPL()
    {
      // MYSQL_IP = __MYSQL_IP;
      MYSQL_USER = __MYSQL_USER;
      MYSQL_PASSWORD = __MYSQL_PASSWORD;
      MYSQL_DATABASE = __MYSQL_DATABASE;

      XMLFILE = __XMLFILE;

      data_content = "";

      buff.clear();
    }
    ~CMyBIZPL() {}

    static CMyBIZPL *
    getInstance();

  public:




                    
    int
    methodProcess(ProducerData &data,std::string &content);

  public:

    int
    cqiHolderDestroy();

  public:

              


    int 
    queryMarketFromUser(CPay &cpay,std::string &outputJson);

    int 
    queryMarketFromMarket(CPay &cpay,std::string &outputJson);

    int 
    queryMarketFromTecCompany(CPay &cpay,std::string &outputJson);

    int 
    queryMarketFromTecCompanyAsUser(CPay &cpay,std::string &outputJson);


//ToDo:
    int
    mainPay(CQIHolder *holder,CPay &cpay,S3FileInfo &s3fi);

    int
    queryRegExist(CReg &reg,std::string & outputJson);


    int
    queryRegPrice(CReg &reg,std::string & outputJson);

    int 
    queryRegSum(CReg &reg,std::string &outputJson);

    int 
    queryMarketFromOtherUser(CPay &cpay,std::string &outputJson);

    int
    queryConsumeSum(CPay &cpay,std::string &outputJson);

    int
    queryConsumeBill(CPay &cpay,std::string &outputJson);

    int 
    getAuthRandom( CAuthUser &user,std::string &outputJson);

    int 
    getAuthTokenfromSig( CAuthUser &user,std::string &outputJson);

    int 
    reqPubkeySync( CAuthUser &user,std::string &outputJson);

    int
    queryMarketFromBn(CMarket &cm,std::string &outputJson);

    int 
    insertMarketInfo(CMarket &cm,std::string &outputJson);
   
    int 
    deleteMarketInfo(CMarket &cm,std::string &outputJson);

    int 
    changeMarketBalance(CMarket &cm,std::string &outputJson);


    int 
    mainUpdateRegPrice(CReg &reg,std::string &outputJson);

   
    //end by liangyuanting

  private:
    static CMyBIZPL * instance;

    std::string data_content;

    // const char * MYSQL_IP;
    const char * MYSQL_USER;
    const char * MYSQL_PASSWORD;
    const char * MYSQL_DATABASE;

    const char * XMLFILE;


  };
  class CRegMethodItem{

    public:
        std::string type;
        //函数指针
        int
        (CMyBIZPL::*processMethod)(CReg &input,
                                    std::string & result);
    };

  class CAuthTokenItem{
    public:
        std::string type;
        //函数指针
        int
        (CMyBIZPL::*processMethod)(CAuthUser &input,
                                  std::string &outputJson);
    };

  class CPayMethodItem{

    public:
        std::string type;
        //函数指针
        int
        (CMyBIZPL::*processMethod)(CPay &input,
                                    std::string & result);
    };
             
  class CMarketMethodItem{

    public:
        std::string type;
        //函数指针
        int
        (CMyBIZPL::*processMethod)(CMarket &market,
                                  std::string &outputJson);
    };




    

inline int writeJsonToCReg(const ProducerData &input, CReg &output){

		// cout<<input.JasonParam<<endl;
		Json::Reader reader;
		Json::Value value;
	
		if (reader.parse(input.param, value))
		{
        
        output.tid = value["TID"].asString();
        output.authToken = value["AuthToken"].asString();
        output.virtualId = value["VirtualId"].asString();
        output.blocklabel = value["BlockLabel"].asString();
        output.deviceGroupId = value["DeviceGroupId"].asString();
				output.providerMarket = value["ProviderMarket"].asString();
        output.price = value["Price"].asString();
        output.verification = value["Verification"].asString();
				output.servfee = value["ServFee"].asString();
        output.s3srcFileName = value["s3srcFileName"].asString();
        output.s3srcFileSize = value["s3srcFileSize"].asInt64();

				return 0;
			
		}
		return -1;
}



inline  int writeJsonToCPay(const ProducerData &input, CPay&output){


		Json::Reader reader;
		Json::Value value;
		//解析方法
		if (reader.parse(input.param, value))
		{
        output.tid = value["TID"].asString();
        output.authToken = value["AuthToken"].asString();
        output.virtualId = value["VirtualId"].asString();
        output.blocklabel = value["BlockLabel"].asString();
				output.consumerMarket = value["ConsumerMarket"].asString();
				output.providerMarket = value["ProviderMarket"].asString();
				output.deviceGroupId = value["DeviceGroupId"].asString();
				output.servfee = value["ServFee"].asString();
        output.authToken = value["AuthToken"].asString();
        output.virtualId = value["VirtualId"].asString();
        //output.verification = value["Verification"].asString();

				return 0;
		
		}
		
	return -1;
}

inline  int writeJsonToCMarket(const ProducerData &input, CMarket&output){


		Json::Reader reader;
		Json::Value value;
		//解析方法
		if (reader.parse(input.param, value))
		{
			  output.tid = value["TID"].asString();
				output.market = value["Market"].asString();
				output.deviceGroupId = value["DeviceGroupId"].asString();
				output.servFee = value["ServFee"].asString();
				output.balance = atof( value["InitBalance"].asString().c_str());
        output.balance = atof( value["ChangeBalance"].asString().c_str());
				return 0;
		
		}
		
	return -1;
}



inline  int writeJsonToCAuthToken(const ProducerData &input, CAuthUser&output){


		Json::Reader reader;
		Json::Value value;
		//解析方法
		if (reader.parse(input.param, value))
		{
			  output.tid = value["TID"].asString();
				output.virtualId = value["VirtualId"].asString();
				output.userPubKey = value["UserPubKey"].asString();
				output.ramdon = value["Random"].asString();
				output.sig = value["Signature"].asString();
				return 0;
		}
		
	return -1;
}


inline int readS3JSON(string input,S3FileInfo &output){
  	Json::Reader reader;
		Json::Value value;
		//解析方法
		if (reader.parse(input.c_str(), value))
		{

				output.s3Access = value["S3Access"].asString();
				output.s3Secret = value["S3Secret"].asString();
				output.s3User = value["S3User"].asString();
				output.s3Host = value["S3Host"].asString();
				output.s3Bucket = value["S3Bucket"].asString();
        output.s3DstFile = value["S3DstFile"].asString();
				return 0;
		
		}

  return -1;
}


};




#ifdef __cplusplus
}
#endif

#endif
  