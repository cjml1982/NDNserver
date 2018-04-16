
#ifndef MYCONSTANT_HPP
#define MYCONSTANT_HPP


#define __XMLFILE "bsdrca.xml"
#define __XMLFILE_GLOBALW "globalw.xml"
//begin add by liangyuanting 20171204
#define _XMLFILE_PORT "bsdrport.xml"
//end add by liangyuanting 20171204
#define __XML_ITEMNAME_DEVICEGROUPID "DEVICEGROUPID"

#define __XML_ITEMNAME_ERROR "Error"
#define __XML_ITEMNAME_CODE "Code"
#define __XML_ITEMNAME_REASON "Reason"

#define __XML_ITEMNAME_ITEM "Item"
#define __XML_ITEMNAME_BLOCKEDLABEL "BlockedLabel"
#define __XML_ITEMNAME_MARKET "Market"
#define __XML_ITEMNAME_PRICE "Price"
#define __XML_ITEMNAME_SRCDEVICENO "SrcDeviceNo"
#define __XML_ITEMNAME_TIMESTAMP "TimeStamp"
#define __XML_ITEMNAME_CONTENT "Content"
#define __XML_ITEMNAME_COUNT "Count"

#define __XML_ITEMNAME_TXID "TxId"
#define __XML_ITEMNAME_TXCONTENT "TxContent"

#define __XML_ITENNAME_ROOT "yl"
#define __XML_ITEMNAME_ROOT_BEGIN "<yl>"
#define __XML_ITEMNAME_ROOT_END "</yl>"

#define __CONSENSUS_RPC_METHOD_SENDTOADDRESS "sendtoaddress"
#define __CONSENSUS_RPC_METHOD_SENDTOADDRESS_WORD_BCR_TO_BSDR "send_amount_from_bcr_to_bsdr"

#define __CONSENSUS_RPC_METHOD_SENDBNRAWTRANSACTION "sendbnrawtransaction"
#define __CONSENSUS_RPC_METHOD_SENDBNRAWTRANSACTION_WORD_BCR_TO_BSDR "send_amount_from_bcr_to_bsdr"

#define __CONSENSUS_RPC_METHOD_GETADDRESSESBYACCOUNT "getaddressesbyaccount"

#define __CONSENSUS_RPC_METHOD_GETBNRAWTRANSACTION "getbnrawtransaction"

#define __CONSENSUS_RPC_METHOD_GETBALANCE "getbalance"
//to use the proxysql to connect the databases;
//#define __MYSQL_IP "172.16.25.112"
#define __MYSQL_PORT 3306
#define __MYSQL_USER "root"
#define __MYSQL_PASSWORD "1qaz@WSX3edc$RFVyl"
#define __MYSQL_DATABASE "mymysql"
#define __MYSQL_CONN_TIMEOUT_MILS 3000

#define __TABLE_DATA "data"
#define __TABLE_TX "tx"

#define __TABLE_MARKET "market"

#define __LOWEST_AMOUNT "0.00005"
#define __LOWEST_FEE "0.00005"

#define __STEP_AMOUNT_F 5.0f
#define __STEP_AMOUNT_S "5.0"

#define __XML_MOREINFO_ROOT "YLBN"
#define __XML_MOREINFO_SHA256 "HashDigest"
#define __XML_MOREINFO_PROCTIME "ProcTime"
#define __XML_MOREINFO_PROCTYPE "ProcType"
#define __XML_MOREINFO_SRCMARKET "SrcMarket"
#define __XML_MOREINFO_DSTMARKET "DstMarket"

#define __BUSINESS_PROCTYPE_REG "reg"
#define __BUSINESS_PROCTYPE_PAY "pay"

#define __MYSQL_WILDCARD_DATA "data"
#define __MYSQL_WILDCARD_TX "tx"
#define __MYSQL_WILDCARD_MARKET "market"


#define _PRINT_ERROR_EMPTY_WITH_RETURN(str) if(str.empty()){ BN_LOG_ERROR("the input param:%s%s",#str," is empty!"); return -1;}

#define _PRINT_ERROR_EMPTY_WITHOUT_RETURN(str) if(str.empty()){ BN_LOG_ERROR("the input param:%s%s",#str," is empty!");}

#define _PRINT_ERROR_FAILED_WITH_RETURN(in) if(in==-1){ BN_LOG_ERROR("the result:%s%s",#in," is failed!"); return -1;}

#define _PRINT_ERROR_FAILED_WITHOUT_RETURN(in) if(in==-1){ BN_LOG_ERROR("the result:%s%s",#in," is failed!");}

#define _PRINT_KEY_VALUE(str) BN_LOG_INTERFACE("the input param name:%s%s%s",#str," value:",str.c_str())





#include "../json/myjson.hpp"
#include "../mysql/myinstqueue.hpp"
#include "../myholder.hpp"
#include "myresultcode.hpp"
//静态变量
    enum PRODUCER_DATA_FUNC
    {
        REG_DATA_SET    =1,
        REG_DATA_UPDATE,
        REG_DATA_DISABLE,
        REG_DATA_PRICE_UPDATE,
        MARKET_ATTACH_BNNODE,
        MARKET_DISABLE_IN_BNNODE,
        DATA_FUNC_ERR
    };

//操作的类
namespace ndn{
    struct CWallet {

                std::string key;
                std::string url;
                std::string header;
                std::string dftaddr;
                std::string chainid;
        };

    class HolderRaii{

            public:
                HolderRaii()
                {
                    int size = 0;
                    // std::cout<<"thus us gege"<<std::endl;
                    ndn::CMyInstQueue::getInstance()->qGetSize(size);
                    //std::cout<<size<<std::endl;
                    holder =(CQIHolder*)ndn::CMyInstQueue::getInstance()->qPop(); 
                    //std::cout<<holder->client<<":"<<holder<<std::endl;
                }
                ~HolderRaii()
                {
                    ndn::CMyInstQueue::getInstance()->qPush(holder);    
                }

               
                inline CQIHolder* 
                getMysqlHolder(){
                    
                    return holder;
                }

            private:
                CQIHolder *holder = NULL;
        };


    class CMarket{

            public:
                std::string tid;
                std::string deviceGroupId;
                std::string market;
                int balance;
                std::string servFee;
              
                CMarket(){
                    tid = "";
                    deviceGroupId = "";
                    market = "";
                    balance = 0;
                    servFee = "";
                    
                }

                inline int 
                checkAddRight(){
                    if(deviceGroupId.empty()||
                        market.empty()||
                        balance<0||
                        tid.empty())
                    {
                       throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    }
                    return 0;
                }

                inline int 
                checkDeleteRight(){
                    if(deviceGroupId.empty()||
                        tid.empty()||
                        market.empty())
                    {
                      throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    }
                    return 0;
                }
                
                inline int
                checkQueryRight(){
                    if(deviceGroupId.empty()||
                        tid.empty()||
                        market.empty())
                    {
                       throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    }
                    return 0;
                }
        };

    class CDealUnit{

        public:   
              std::string tid;
              std::string blocklabel;
              std::string providerMarket;
              std::string verification;
              std::string deviceGroupId;
              std::string price;
              std::string servfee;
              std::string chainId;
              std::string content;
              
              int ackey = -1;

        inline int
            checkBlockLabeExpression()
            {
                if(-1 == ndn::CMyJSON::getInstance()->stringIsBeginEnd(blocklabel))
                {
                    throw new CMy_BSDR_Exception("__GLOBAL_INPUT_MESSAGE_PARSE_ERR",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                };
                return 0;
            }
    };

    class CPay:public CDealUnit{

            public:
                std::string authToken;
                std::string virtualId;
                std::string consumerMarket;
                std::string payMount;
              
            public:
                CPay(){
                    tid="";
                    blocklabel="";
                    consumerMarket="";
                    deviceGroupId="";
                    payMount="0";
                    authToken=""; 
                    virtualId="";
                    content="";
                }

                inline int 
                checkCommon()
                {
                    
                    if(blocklabel.empty()
                        ||0!=checkBlockLabeExpression()
                        ||tid.empty()
                        ||authToken.empty()
                        ||virtualId.empty()
                        ||providerMarket.empty()
                        ||consumerMarket.empty()
                        ||deviceGroupId.empty())
                    {
                       throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    };
                    return 0;
                }

                inline int 
                checkMarketOrTec()
                {
                    
                    if(blocklabel.empty()
                        ||0!=checkBlockLabeExpression()
                        ||tid.empty()
                        ||!authToken.empty()
                        ||!virtualId.empty()
                        ||providerMarket.empty()
                        ||consumerMarket.empty()
                        ||deviceGroupId.empty())
                    {
                        throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    };
                    return 0;
                }
        };

    class CReg:public CDealUnit{
            
            public:
                std::string authToken;
                std::string virtualId;
                std::string s3srcFileName;
                int64_t s3srcFileSize;
               
            public:
                CReg(){
                    authToken="";
                    blocklabel="";
                    virtualId = "";
                    providerMarket="";
                    price="";
                    deviceGroupId="";
                    servfee="";
                    s3srcFileName="";
                    s3srcFileSize=0;
                    verification="";
                }

            public:

                inline int checkRight(int type){
                    if(type == PRODUCER_DATA_FUNC::REG_DATA_SET||type == PRODUCER_DATA_FUNC::REG_DATA_UPDATE)
                     return checkAddOrUpdate();
                    if(type == PRODUCER_DATA_FUNC::REG_DATA_DISABLE)
                     return checkBanOrReviewOrQuery();
                    if(type == PRODUCER_DATA_FUNC::REG_DATA_DISABLE)
                     return checkUpdatePrice();
                    //没有这个类型额
                    return 0;

                }

                inline int
                checkRegSum(){
                    if(virtualId.empty()
                        ||tid.empty()
                        ||verification.empty()
                        ||authToken.empty()
                        ||providerMarket.empty()
                        ||deviceGroupId.empty())
                    {
                       throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    };
                    return 0;

                }

                inline int 
                checkAddOrUpdate(){              
                    if(blocklabel.empty()
                        ||0!=checkBlockLabeExpression()
                        ||price.empty()
                        ||atof(price.c_str())<0
                        ||tid.empty()
                        ||authToken.empty()
                        ||virtualId.empty()
                        ||providerMarket.empty()
                        ||deviceGroupId.empty()
                        ||verification.empty()
                        ||((s3srcFileName.empty()||s3srcFileSize<0)&&content.empty()))
                    {
                       throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    };
                    return 0;
                }

                inline int
                checkBanOrReviewOrQuery(){              
                    if(blocklabel.empty()
                        ||0!=checkBlockLabeExpression()
                        ||virtualId.empty()
                        ||tid.empty()
                        ||authToken.empty()
                        ||providerMarket.empty()
                        ||deviceGroupId.empty())
                    {
                       throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    };
                    return 0;
                }

                inline int 
                checkUpdatePrice(){
                    if(blocklabel.empty()
                        ||0!=checkBlockLabeExpression()
                        ||virtualId.empty()
                        ||tid.empty()
                        ||authToken.empty()
                        ||providerMarket.empty()
                        ||deviceGroupId.empty()
                        ||price.empty())
                    {
                        throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
                    };
                    return 0;
                }


    };


    struct RegResult{

        std::string resultCode;
        std::string txid;
        std::string content;
        std::string price;
        std::string sum;

    };

    struct S3FileInfo{

        S3FileInfo()
        {
            digestf = "";
            blocklabel = "";
            tid = "";
            s3SrcFileName = "";
            s3Access = "";
            s3Secret = "";
            s3User = "";
            s3Host = "";
            s3Bucket = "";
            s3DstFile = "";
            s3Action = "0";
            s3SrcFileSize = 0;

        }

        std::string tid;
        std::string blocklabel;
        std::string s3SrcFileName;
        std::string s3Access; 
        std::string s3Secret; 
        std::string s3User; 
        std::string s3Host; 
        std::string s3Bucket; 
        std::string s3DstFile; 
        std::string s3Action;
        std::string digestf;
        int64_t s3SrcFileSize;
 

    };
    struct RegChainInfo{
    
        RegChainInfo (){

            blocklabel="";
            verification="";
            market="";
            content = "";
            txid="";
            s3SrcFileName="";
            s3Access=""; 
            s3Action = "0";
            s3Secret=""; 
            s3User=""; 
            s3Host=""; 
            s3Bucket=""; 
            s3DstFile=""; 
            tid = "";
            ackey= -1;
        }

        std::string tid;
        std::string blocklabel;
        std::string verification;
        std::string market;
        std::string content;

        std::string txid;

        std::string deviceGroupId;
        std::string s3SrcFileName;
        std::string s3Action;
        std::string s3Access; 
        std::string s3Secret; 
        std::string s3User; 
        std::string s3Host; 
        std::string s3Bucket; 
        std::string s3DstFile; 
        std::string digestf;
        int ackey;
        int resultCode;

    };

    struct DBUtil{
    
                std::string ip;
                std::string port;
                std::string pwd;
                std::string user;
    };
    //数据结构

    //法信服务器
    struct FXUtil{
        std::string fip;
        std::string fport;
    };

    struct CAuthUser{

        std::string tid;
        std::string virtualId;
        std::string userPubKey;
        std::string sig;
        std::string ramdon;
        std::string authToken;

        CAuthUser(){
            
            tid = "";
            virtualId = "";
            userPubKey = "";
            sig = "";
            ramdon = "";
            authToken = "";
        }

        inline int checkSynRight(){
            if(tid.empty()
            ||userPubKey.empty()
            ||virtualId.empty())
               throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
            return 0;
        }

        inline int checkGetRamdonRight(){
            if(tid.empty()
            ||virtualId.empty())
                throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
            return 0;
        }

        inline int checkSignRight(){
            if(tid.empty()
            ||virtualId.empty()
            ||sig.empty())
                throw new CMy_BSDR_Exception("ERROR_FORMATE",__GLOBAL_INPUT_MESSAGE_PARSE_ERR);
            return 0;
        }

    };
}


#endif
