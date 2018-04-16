#ifndef _MYBASEBIZ_CPP
#define _MYBASEBIZ_CPP
#include"mybasebiz.h"
INIT_LOGGER("CMyBaseBIZ");
std::vector<TiXmlElement *> ndn::CMyBaseBIZ::getXMLVec(){return nodeVec;}

std::string ndn::CMyBaseBIZ::getMyLocalName(){return mylocalname;}
                     

//do 把data表txid为null的字段回填到队列中
int 
ndn::CMyBaseBIZ::reChain(){
      
      _LOG_INFO("begin reChain");
      //raii
      HolderRaii raiiholder;
      CQIHolder *holder = raiiholder.getMysqlHolder();
      initMysqlHolder(holder);
      ndn::CMyMYSQLPL::getInstance()->rechain(holder,q,buff,qs3fi);
      uploadMarket(holder);
      //对qs3fi的内容进行发送给fxserver
      while(!qs3fi.empty())
      {
        S3FileInfo s3fi = qs3fi.front();
        postRegFxServ(holder,fxUrl,fxHeader,s3fi.tid,s3fi);
        q.pop();
      }
      
      _LOG_INFO("end reChain");      
      return 0;
    }

int
ndn::CMyBaseBIZ::initMySQLPool()
{
  //raii管理  
  
  int poollen = Config::Instance()->m_mysqlPoolSize;
  

  for(int i = 0;i<poollen;i++)
  {
    try{
         CQIHolder *holder = new CQIHolder;
         initMysqlHolder(holder);
    
        ndn::CMyInstQueue::getInstance()->qPush(holder);
    }catch(std::bad_alloc &e)
    {
        BN_LOG_ERROR("new operator is error");
        return -1;
    }
  }
  return 0;

}



int
ndn::CMyBaseBIZ::getAddressesByAccount(CQIHolder * holder,
                                     std::string & sURL,
                                     std::string & sHeader,
                                     std::string & address)
{
  _LOG_INFO("begin getAddressesByAccount");

  std::string methodValue(__CONSENSUS_RPC_METHOD_GETADDRESSESBYACCOUNT);
  std::string paramsValue("");
  std::string bodyJSON("");
  int responseCode = -1;
  std::string response("");
  ndn::CMyJSON::getInstance()->writeJSON(methodValue, paramsValue, bodyJSON);
 //fprintf(stderr, "addrJSON: %s\n", bodyJSON.c_str());
 // _LOG_INFO("addrJSON: " << bodyJSON);
  ndn::CMyCURLPL::getInstance()->doHTTPPost(holder, sURL, sHeader, bodyJSON, responseCode, response);
 //fprintf(stderr, "responseCode: %d\n", responseCode);
  //_LOG_INFO("responseCode: " << responseCode);
 
//_LOG_INFO("response: " << response);

  if (responseCode != 200)
  {
   //fprintf(stderr, "curl post request error!\n");
     BN_LOG_ERROR("curl post request error! the responseCode = %d",responseCode);
     throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
  }

  std::string jsResult("");
  std::string jsError("");
  std::string jsId("");
  ndn::CMyJSON::getInstance()->readJSON(response, jsResult, jsError, jsId);
 //fprintf(stderr, "jsResult: %s\n", jsResult.c_str());
  //_LOG_INFO("jsResult: " << jsResult);
 //fprintf(stderr, "jsError: %s\n", jsError.c_str());
  //_LOG_INFO("jsError: " << jsError);
 //fprintf(stderr, "jsId: %s\n", jsId.c_str());
  //_LOG_INFO("jsId: " << jsId);

  if (jsResult.empty())
  {
   //fprintf(stderr, "jsResult is empty!\n");
    BN_LOG_INTERFACE("jsResult is empty!");
    return -1;
  }

  address = jsResult;

  _LOG_INFO("end getAddressesByAccount");

  return 0;
}

//但上链失败时 的话
void
ndn::CMyBaseBIZ::doRun(CQIHolder * holder)
{
    //所有对容器的修改都需要加锁
    BN_LOG_DEBUG("the Size of Address:%d", wvec.size());
    std::lock_guard<std::mutex> lg(mtx_doRun);
    int resFail = initMysqlHolder(holder);
    if(-1 == resFail)
    {
        return;
    }
    int elemCount = 0;
    for( std::vector<TiXmlElement *>::iterator elem=nodeVec.begin();elem!=nodeVec.end();++elem)
    {
        int i = 0;
        for(std::vector<std::string>::iterator welem = wvec.begin();welem!=wvec.end();++welem)
        {
            if(*welem == (*elem)->Attribute("name"))
            {
                i++;
            }
        }

        BN_LOG_DEBUG("the Size of Address:%d", wvec.size());
        if(i==wvec.size())
        {
            BN_LOG_DEBUG("adding");

            std::string devicename("");
            std::string ip("");
            std::string port("");
            std::string chainid("");
        
            devicename = (*elem)->Attribute("name");
            ip =  (*elem)->Attribute("ip");

            for(int j = 0;j<chainTotal;j++)
            {
                port = Config::Instance()->portVec[j]->Attribute("port");
                chainid = std::to_string(j);
        
                std::string URL("http://" + ip + ":" + port + "/");
                std::string Header("Authorization: Basic bm9kZWR1c3I6Y29ublRvTm9kZWQ=");
                std::string Address("");
                resFail = getAddressesByAccount(holder,URL, Header, Address);
                BN_LOG_DEBUG("the Address:%s",Address.c_str());
                if (-1==resFail)
                {
                 BN_LOG_ERROR("deamon getAddressesByAccount error!");
                 continue;
                }           
                if(-1 == putToWalletArray((j+chainTotal*elemCount),URL,Header,Address,chainid))
                {
                  continue;
                };

                wvec.push_back(devicename);
            }
        }
        elemCount++;
    }
}

int
ndn::CMyBaseBIZ::initWalletHashMap(const std::string & globalw)
{
  BN_LOG_INTERFACE("begin initWalletHashMap");
  int resFail = 0;
 
  _LOG_INFO("begin loadXMLtoCreateWallet");
  TiXmlDocument * pDoc = new TiXmlDocument(globalw.c_str());
  bool isOpened = pDoc->LoadFile();
  if (!isOpened)
  {
      
      BN_LOG_ERROR("globalw file opened error!");
      throw new CMy_BSDR_Exception("__GLOBAL_CONFIG_ERR",__GLOBAL_CONFIG_ERR);
  }
  TiXmlElement * root = pDoc->RootElement();
  //begin 20170824 liangyuanting
  TiXmlElement * child = root->FirstChildElement(__XML_ITEMNAME_DEVICEGROUPID);
  TiXmlAttribute * pAttr = child->FirstAttribute();
  mylocalname = pAttr->Value();

  //end 20170824 liangyuanting
  if (NULL == root)
  {
      //fprintf(stderr, "loadXMLtoCreateWallet RootElement empty!\n");
      _LOG_INFO("loadXMLtoCreateWallet RootElement empty!");
      return -1;
  }

  int ret = 0;
  std::string devicename("");
  std::string ip("");
  std::string port("");

  nodeCount = 0;
  for (TiXmlElement * elem = root->FirstChildElement("NODE"); \
    elem != NULL; \
    elem = elem->NextSiblingElement()){
      nodeCount++;
  }
  //新建数组
  chainTotal = Config::Instance()->portVec.size();
  nodeTotal = nodeCount*chainTotal;
  
  walletArray = new CWallet*[nodeTotal];
  //new CWallet*[nodeTotal];
  for(int i = 0;i<nodeTotal;i++)
  {
    walletArray[i] = NULL;
  }

  //对自身的ip进行获取
   
    for(int i = 0;i<chainTotal;i++)
    {
      TiXmlElement * tmp = Config::Instance()->portVec[i];
      string chainid = std::to_string(i);
      string port = tmp->Attribute("port");
      
      std::string URL("http://" + Config::Instance()->myip + ":" + port + "/");
      BN_LOG_INTERFACE("url:%s",URL.c_str());
      std::string Header("Authorization: Basic bm9kZWR1c3I6Y29ublRvTm9kZWQ=");
      std::string Address("");
      _PRINT_KEY_VALUE(URL);
      ret = ndn::CMyBIZ::getInstance()->getAddressesByAccountinitWalletHashMap(URL, Header, Address);
      BN_LOG_INTERFACE("Address:%s", Address.c_str());
      if (ret == -1)
      {
          BN_LOG_ERROR("loadXMLtoCreateWallet getAddressesByAccount error!");
          continue;
      }
      CWallet *tmpWallet = new CWallet();
      tmpWallet->chainid = i;
      tmpWallet->dftaddr = Address;
      tmpWallet->header = Header;
      tmpWallet->url = URL;
      myWallet[i] = tmpWallet;
      
    }
  

  int j = 0;
  //加载bsdrca 获取node
  for (TiXmlElement * elem = root->FirstChildElement("NODE"); \
      elem != NULL; \
      elem = elem->NextSiblingElement())
  {
      BN_LOG_INTERFACE(elem->Attribute("name"));
      if(NULL == elem->Attribute("name"))
      {
         
          BN_LOG_ERROR("loadXMLtoCreateWallet name empty!");
         
          continue;
      }
      if(NULL == elem->Attribute("ip"))
      {
          //fprintf(stderr, "loadXMLtoCreateWallet ip empty!\n");
          BN_LOG_ERROR("loadXMLtoCreateWallet ip empty!");
          continue;
      }

      TiXmlElement * tmpElem ;
      tmpElem =(TiXmlElement *) elem->Clone();

      nodeVec.push_back(tmpElem);
      devicename = elem->Attribute("name");
      ip = elem->Attribute("ip");

      for(int i = 0;i<chainTotal;i++)
      {
        TiXmlElement * tmp = Config::Instance()->portVec[i];
        string chainid = std::to_string(i);
        string port = tmp->Attribute("port");
        
        std::string URL("http://" + ip + ":" + port + "/");
        BN_LOG_INTERFACE("url:%s",URL.c_str());
        std::string Header("Authorization: Basic bm9kZWR1c3I6Y29ublRvTm9kZWQ=");
        std::string Address("");
        _PRINT_KEY_VALUE(URL);
        ret = ndn::CMyBIZ::getInstance()->getAddressesByAccountinitWalletHashMap(URL, Header, Address);
        BN_LOG_INTERFACE("Address:%s", Address.c_str());
         if (ret == -1)
         {
             BN_LOG_ERROR("loadXMLtoCreateWallet getAddressesByAccount error!");
            continue;
         }
        BN_LOG_INTERFACE("putting");
        //加入map
        if(-1 == putToWalletArray((i+chainTotal*j),URL,Header,Address,chainid))
        {
          continue;
        }  
        wvec.push_back(devicename);
      }
      //end modified by liangyuanting 20171204
      j++;
  }
  pDoc->Clear();
  delete pDoc;
  pDoc = NULL;
  //法信服务器的curl的构建
  fxHeader = std::string("Content-Type:application/json");
  
  fxUrl = std::string("http://" + Config::Instance()->futil.fip + ":" + Config::Instance()->futil.fport + "/");
  
  BN_LOG_INTERFACE("end initWalletHashMap");
  return 0;
}

//mysqlOp init
int 
ndn::CMyBaseBIZ::initMysqlHolder(CQIHolder *holder)
{
    BN_LOG_DEBUG("begin initMySQLHolder");
    if (NULL == holder->client)
    {
        for(int i = 0;i<(Config::Instance()->dbvec.size());i++)
        {
          
          holder->client = ndn::CMyMYSQLPL::getInstance()->connectMYSQL(((Config::Instance()->dbvec[i]).ip).c_str(), __MYSQL_USER, __MYSQL_PASSWORD, __MYSQL_DATABASE);
          if(holder->client!=NULL)
          {
              BN_LOG_INTERFACE("the ip:%s%s",(Config::Instance()->dbvec[i]).ip.c_str()," is good");
              break;
          }
        }
  
       if (NULL == holder->client)
       {
         BN_LOG_ERROR("SQL handler is NULL!");
         throw new CMy_BSDR_Exception("__GLOBAL_MY_MYSQL_CONNECTION_ERROR",__GLOBAL_MY_MYSQL_CONNECTION_ERROR);
       }
     }
  
      //此时要判断连接时候过时
  
    if (ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder) == -1)
    {
        //重连失败
        BN_LOG_ERROR("SQL handler is NULL!");
        throw new CMy_BSDR_Exception("__GLOBAL_MY_MYSQL_CONNECTION_ERROR",__GLOBAL_MY_MYSQL_CONNECTION_ERROR);
    }

    if (NULL == holder->curl)
    {
      holder->curl = ndn::CMyCURLPL::getInstance()->curlInstance();
  
      if (NULL == holder->curl)
      {
        {
          BN_LOG_ERROR("curl inst is NULL!");
          throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_INIT_ERR",__GLOBAL_RESULT_CURL_INIT_ERR);
        }
      }
    }
    BN_LOG_DEBUG("end initMySQLHolder");
    return 0;
}



int 
ndn::CMyBaseBIZ::checkBlockLabelAndAvailableType(CQIHolder *holder,
                                                CDealUnit &unit,
                                                int &blcount,
                                                int &available)
{
    BN_LOG_INTERFACE("begin check blocklabel and available"); 
    int resFail = 0;
    std::string cursorTxId("");
    
    unit.chainId = std::to_string(getMyChainNum(unit.blocklabel));

    //_LOG_INFO("chainId:"<<unit.chainId);
     //查链可以一步搞掂
    ndn::CMyMYSQLPL::getInstance()->hasblocklabel(holder,unit,blcount,cursorTxId,available);
    if(-1 == resFail)
    {
        return -1;
    }
    if(1 == blcount&&1 == available)
    {
        //只有使能时 才能进行下一步查链
        //查链可以一步搞掂
        // resFail = ndn::CMyMYSQLPL::getInstance()->regAvailable(holder,unit.blocklabel,unit.chainId,available);
        // if(-1 == resFail)
        // {
        //     return -1;
        // }

        //查链 无论怎样都是要查链的 假设数据库存储内容是不可靠的
        resFail = checkChainData(holder,unit,cursorTxId);
        //如果db存的东西查链过了的话 就要将verification修改
        //如果发送方没有将verfication传入 则
        // if(unit.verification.empty())
        // {
        //   unit.verification = unit.dbVerif;
        // }
        if(-1 == resFail)
        {
           throw new CMy_BSDR_Exception("__GLOBAL_NOT_RIGHT_CHAINDATA_ERR",__GLOBAL_NOT_RIGHT_CHAINDATA_ERR);
        }
        //对账成功

    }
    BN_LOG_INTERFACE(" check blocklabel and available"); 
    return 0;
};


int 
ndn::CMyBaseBIZ::checkChainData(CQIHolder *holder,CDealUnit unit,std::string &cursorTxId){

      std::string cURL("");
      std::string cHeader("");
      std::string cAddress("");
      std::string cChainId("");
      //对每一个like的内容获取chainId 与bsdr 进行地址的处理 此时是该链的地址 url
      // retJudge = getWalletFromMap(cWhere, cURL, cHeader,cAddress,cChainId);
      int resFail = 0;
      if(-1 == unit.ackey){
   
        unit.ackey = getACKey(unit.blocklabel);
        BN_LOG_INTERFACE("ACKEY:%d",unit.ackey);
      }
      
      resFail =  getFromWalletArray(unit.ackey,cURL, cHeader,cAddress,cChainId);
      // resFail = getWalletFromHashMap(unit.providerMarket, cURL, cHeader,cAddress,unit.chainId);
      
      if (-1 == resFail){
        BN_LOG_INTERFACE("wallet query error!");
        return -1;
      }
      // if(cURL.empty()){
      //     _LOG_INFO("url or header or address empty!");
      //     doRun(holder);
      //     return -1;
      // }
      //获取该笔已经[登记确权]的区块链tx记录 
      //链上数据，此时子链的地址 url 应为data表的子链的url
      
      BN_LOG_DEBUG("cURL: %s",cURL.c_str());
      BN_LOG_DEBUG("cHeader: %s", cHeader.c_str());
      BN_LOG_DEBUG("cAddress: %s" ,cAddress.c_str());

      std::string cContent("");
      resFail =  getGetBnRawTransaction(holder, cURL, cHeader, cAddress, cursorTxId, cContent);
      if (resFail == -1)
      {
          BN_LOG_ERROR("BnRawTransaction error!");
          return -1;
      }
    
      std::string tmpcContent("");
      ndn::CMyJSON::getInstance()->stringSplit(cContent, tmpcContent);
     
      std::string bcHashDigestv1("");
      ndn::CMyJSON::getInstance()->stringSplitB(tmpcContent, bcHashDigestv1);
      
      // std::string bcHashDigest("");
      // ndn::CMyJSON::getInstance()->charOutOfHead(bcHashDigestv1, bcHashDigest);
      bcHashDigestv1.erase(std::remove(bcHashDigestv1.begin(), bcHashDigestv1.end(), '\"'), bcHashDigestv1.end());//"
      
      //查file表 
      S3FileInfo s3fi;
      s3fi.blocklabel = unit.blocklabel;
      ndn::CMyMYSQLPL::getInstance()->selectS3File(holder,s3fi);
      //计算hash256;
      std::string hsDigest;
      getVerif(unit.content,s3fi.digestf,hsDigest);

      if (hsDigest!= bcHashDigestv1)
      {
          BN_LOG_ERROR("check error!");
          throw new CMy_BSDR_Exception("__GLOBAL_USERDATA_BROKEN",__GLOBAL_USERDATA_BROKEN);
      }
  return 0;
}




int
ndn::CMyBaseBIZ::readChainQueueElement()
{
  BN_LOG_INTERFACE("begin readChainQueueElement");

  int ret = 0;
  
  CQIHolder *holder = new CQIHolder;
 
  initMysqlHolder(holder);

  //_LOG_INFO("the address of holder:"<<holder);

  std::string verification = std::string("");
  RegChainInfo hs;

  while (true)
  {
    {
      std::unique_lock<std::mutex> ul(mtx_queue);
      while (q.empty())
        cv.wait(ul);
      if(!q.empty())
      {
         hs = q.front();
         verification = hs.verification; 
         q.pop();
        if (verification.empty())
        {
          continue;
        }
      }
      else
      {
        continue;
      }
    }
    


    //上链上sha256 txid回存data表
    //此时没有market 此时为devicegroupid代替
    //所以此时的market为在表之中的
    //该数据上哪条链？
    
    std::string sURL("");
    std::string sHeader("");
    std::string sAddress("");
    std::string sChainId("");
    if(hs.ackey ==-1)
    {
      hs.ackey = getACKey(hs.blocklabel); 
    }
    
    getFromWalletArray(hs.ackey,sURL,sHeader,sAddress,sChainId);
    
    std::string txid("");
   
   {
     try{
         
         if(!hs.s3SrcFileName.empty())
         {

              hs.digestf = getDigestFFormMap(hs.blocklabel);

              if(hs.digestf.empty())
              {
                writeChainQueueElement(hs);
                continue;
              }
         }
         
          std::string output;
          getVerif(hs.content,hs.digestf,output);
          //插入错误
          if(output!=hs.verification)
          {
            //
            //此次更新失败，置
            //delete file表内容与
            // if(!hs.s3SrcFileName.empty())
            // {              deleteDataAndFile(holder,hs.blocklabel,sChainId);
            // }
            // else{
            //   deleteData(holder,hs.blocklabel,sChainId);
            // }
              if(1 == buff.count(hs.blocklabel))
              {
                buff.erase(hs.blocklabel);  
              }
            //此次更新失败，只有该blocklabel被锁定，当且仅但传入正确的后，才能生效
          //  
            hs.resultCode = __GLOBAL_INPUT_MESSAGE_PARSE_ERR;
            try{
                postChainInfoFxServ(holder,fxUrl,fxHeader,hs.tid,std::to_string(hs.resultCode));
            }catch(CMy_BSDR_Exception *e)
            {
              BN_LOG_ERROR("%s%d",e->getInfo().c_str(),e->getResultCode());          
              delete e;
              e = NULL;
            }
            continue;
          }

          std::unique_lock<std::mutex> ul(mtx_curl);
          ret = getSendToAddressForRightV2Demo(holder,sURL, sHeader, verification, sAddress, hs.txid,__LOWEST_AMOUNT,sChainId);

          _LOG_INFO("txid: " << hs.txid);
          if (-1 == ret)
          {
            BN_LOG_ERROR("getSendToAddressForRightV2Demo error!");
            writeChainQueueElement(hs);  
            
            continue;
          }

          if(hs.txid.empty())
          {
              BN_LOG_ERROR("getSendToAddressForRightV2Demo error!");
              writeChainQueueElement(hs);  
              continue;
          }

          if(!hs.s3SrcFileName.empty())
          {
            ndn::CMyMYSQLPL::getInstance()->updateToChainDataAndFile(holder,hs,sChainId);
          }else{
            ndn::CMyMYSQLPL::getInstance()->updateToChainData(holder,hs,sChainId);
          }

          //这里发出对服务器的curl请求
          
          postChainInfoFxServ(holder,fxUrl,fxHeader,hs);

     }catch(CMy_BSDR_Exception *e)
     {
       BN_LOG_ERROR("%s%d",e->getInfo().c_str(),e->getResultCode())
       if(e->getResultCode() != __GLOBAL_SENDFXSERVER_ERR)
       {
         //不是法信服务器异常
         writeChainQueueElement(hs);        
       }
       
       delete e;
       e = NULL;
     }
     catch(...)
     {
       BN_LOG_ERROR("unknown c++ error!");
     }
   

    }
   
  }
  
  ndn::CMyMYSQL::getInstance()->closeMYSQL();
 // _LOG_INFO("end readChainQueueElement");
  return 0;
}


int ndn::CMyBaseBIZ::deleteDataAndFile(CQIHolder *holder,std::string blocklabel,std::string sChainId)
{
  //删除map key为blocklabel的value
  if(1 == buff.count(blocklabel))
  {
    buff.erase(blocklabel);  
  }
  ndn::CMyMYSQLPL::getInstance()->deleteDataAndFile(holder,blocklabel,sChainId);
  //删除File上的文件
  return 0;
}

int ndn::CMyBaseBIZ::deleteData(CQIHolder *holder,std::string blocklabel,std::string sChainId)
{
  //删除map key为blocklabel的value
  ndn::CMyMYSQLPL::getInstance()->deleteDataAndFile(holder,blocklabel,sChainId);
  //删除File上的文件
  return 0;
}



int
ndn::CMyBaseBIZ::runReader()
{
  ndn::CMyMYSQL::getInstance()->initMYSQL();
  int len = Config::Instance()->m_asynThrSize;
  mythread = new std::thread*[len];
  BN_LOG_INTERFACE("begin runReader");
  for(int i = 0;i<len;i++)
  {
    try{
        mythread[i] = new std::thread(&ndn::CMyBaseBIZ::readChainQueueElement, this);
    } catch (const std::bad_alloc& e) {
       // BN_LOG_ERROR("Allocation failed: %s",e.what);
        return -1;
    }
    
  }


  return 0;
}

int
ndn::CMyBaseBIZ::postRegFxServ(CQIHolder *holder,std::string sURL,std::string sHeader,std::string tid,S3FileInfo input)
{
  BN_LOG_DEBUG("begin to postChainInfoFxServ");
  std::string bodyJSON("");
  std::string  methodValue("");
  int responseCode = -1;
  std::string response("");
  //将RegChain变为json bodyJSon
  bodyJSON.append("{");
  bodyJSON.append(writeResultJson("TID",tid));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("ResultCode","1"));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3Action",input.s3Action));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3User",input.s3User));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3Access",input.s3Access));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3Secret",input.s3Secret));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3Host",input.s3Host));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3Bucket",input.s3Bucket));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("S3DstFile",input.s3DstFile)); 
  bodyJSON.append("}");
  try{
    BN_LOG_DEBUG("CURLJSON: %s" ,bodyJSON.c_str());
    ndn::CMyCURLPL::getInstance()->doHTTPPost(holder,sURL, sHeader, bodyJSON, responseCode, response);
    BN_LOG_DEBUG("responseCode: %d" ,responseCode);
    BN_LOG_DEBUG("response: %s", response.c_str());

    if (responseCode != 200)
    {
      BN_LOG_ERROR("curl post request error!");
    throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
    }
 }catch(...)
  {
    throw new CMy_BSDR_Exception("__GLOBAL_SENDFXSERVER_ERR",__GLOBAL_SENDFXSERVER_ERR);
  }
  return 0;
}

int
ndn::CMyBaseBIZ::postChainInfoFxServ(CQIHolder *holder,std::string sURL,std::string sHeader,std::string tid,std::string resultCode){

  //c
  //这是发向fxserver服务器的 so
  try{
        BN_LOG_DEBUG("begin to postChainInfoFxServ");
        std::string bodyJSON("");
        std::string  methodValue("");
        int responseCode = -1;
        std::string response("");
        //将RegChain变为json bodyJSon
        bodyJSON.append("{");
        bodyJSON.append(writeResultJson("TID",tid));
        bodyJSON.append(",");
        bodyJSON.append(writeResultJson("ResultCode",resultCode));
        bodyJSON.append("}");
        
        BN_LOG_DEBUG("CURLJSON: %s" ,bodyJSON.c_str());
        ndn::CMyCURLPL::getInstance()->doHTTPPost(holder,sURL, sHeader, bodyJSON, responseCode, response);
        BN_LOG_DEBUG("responseCode: %d" ,responseCode);
        BN_LOG_DEBUG("response: %s", response.c_str());

        if (responseCode != 200)
        {
          BN_LOG_ERROR("curl post request error!");
          throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
        }
  }catch(...)
  {
    throw new CMy_BSDR_Exception("__GLOBAL_SENDFXSERVER_ERR",__GLOBAL_SENDFXSERVER_ERR);
  }

  return 0;
}


int
ndn::CMyBaseBIZ::postChainInfoFxServ(CQIHolder *holder,std::string sURL,std::string sHeader,RegChainInfo input){
  BN_LOG_DEBUG("begin to postChainInfoFxServ");
  std::string bodyJSON("");
  std::string  methodValue("");
  int responseCode = -1;
  std::string response("");
  //将RegChain变为json bodyJSon
  bodyJSON.append("{");
  bodyJSON.append(writeResultJson("TID",input.tid));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("ResultCode","1"));
  bodyJSON.append(",");
  bodyJSON.append(writeResultJson("TxId",input.txid));
  bodyJSON.append("}");
  try{
    BN_LOG_DEBUG("CURLJSON: %s" ,bodyJSON.c_str());
    ndn::CMyCURLPL::getInstance()->doHTTPPost(holder,sURL, sHeader, bodyJSON, responseCode, response);
    BN_LOG_DEBUG("responseCode: %d" ,responseCode);
    BN_LOG_DEBUG("response: %s", response.c_str());

    if (responseCode != 200)
    {
      BN_LOG_ERROR("curl post request error!");
    throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
    }
 }catch(...)
  {
    throw new CMy_BSDR_Exception("__GLOBAL_SENDFXSERVER_ERR",__GLOBAL_SENDFXSERVER_ERR);
  }
  return 0;
}



//begin liangyuanting added
int
ndn::CMyBaseBIZ::sendToAddress(CQIHolder *holder,std::string & sURL,
                                            std::string & sHeader,
                                            std::string & hDigest,
                                            std::string & address,
                                            std::string & txid,
                                            std::string amount,
                                            std::string chainid)
{
  _LOG_INFO("begin getSendToAddressForRightV2Demo");

  std::string methodValue(__CONSENSUS_RPC_METHOD_SENDBNRAWTRANSACTION);
  std::vector<std::string> paramsValues;
  paramsValues.clear();

  paramsValues.push_back("");//reqseq
  paramsValues.push_back(address);
  paramsValues.push_back(amount);//登记确权不需要代币/为了业务给出默认最小amount=btc
  paramsValues.push_back(hDigest);

  std::string bodyJSON("");
  int responseCode = -1;
  std::string response("");
  ndn::CMyJSON::getInstance()->writeJSONArraySendBnRawTransaction(methodValue, paramsValues, bodyJSON);
  _LOG_INFO("txJSON: " << bodyJSON);
  ndn::CMyCURLPL::getInstance()->doHTTPPost(holder,sURL, sHeader, bodyJSON, responseCode, response);
  _LOG_INFO("responseCode: " << responseCode);
  _LOG_INFO("response: " << response);

  if (responseCode != 200)
  {
    BN_LOG_ERROR("curl post request error!");
   throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
  }

  std::string jsResult("");
  std::string jsTxId("");
  std::string jsReqSeq("");
  std::string jsError("");
  std::string jsId("");
  ndn::CMyJSON::getInstance()->readJSONSendBnRawTransaction(response, jsResult, jsTxId, jsReqSeq, jsError, jsId);
  //这时不够钱 就进行转账

  _LOG_INFO("jsResult: " << jsResult);
  _LOG_INFO("jsTxId: " << jsTxId);
  _LOG_INFO("jsReqSeq: " << jsReqSeq);
  _LOG_INFO("jsError: " << jsError);
  _LOG_INFO("jsId: " << jsId);


  if (jsResult.empty())
  {
    _LOG_ERROR("jsResult is empty!");
    throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_RESPONSE_ERR",__GLOBAL_RESULT_CURL_RESPONSE_ERR);
  }

  txid = jsTxId;
  _LOG_INFO("end getSendToAddressForRightV2Demo");
  return 0;
}


//begin liangyuanting added
int
ndn::CMyBaseBIZ::getSendToAddressForRightV2Demo(CQIHolder *holder,std::string & sURL,
                                            std::string & sHeader,
                                            std::string & hDigest,
                                            std::string & address,
                                            std::string & txid,
                                            std::string amount,
                                            std::string chainid)
{
  _LOG_INFO("begin getSendToAddressForRightV2Demo");

  std::string methodValue(__CONSENSUS_RPC_METHOD_SENDBNRAWTRANSACTION);
  std::vector<std::string> paramsValues;
  paramsValues.clear();

  paramsValues.push_back("");//reqseq
  paramsValues.push_back(address);
  paramsValues.push_back(amount);//登记确权不需要代币/为了业务给出默认最小amount=btc
  paramsValues.push_back(hDigest);

  std::string bodyJSON("");
  int responseCode = -1;
  std::string response("");
  ndn::CMyJSON::getInstance()->writeJSONArraySendBnRawTransaction(methodValue, paramsValues, bodyJSON);
  _LOG_INFO("txJSON: " << bodyJSON);
  ndn::CMyCURLPL::getInstance()->doHTTPPost(holder,sURL, sHeader, bodyJSON, responseCode, response);
  _LOG_INFO("responseCode: " << responseCode);
  _LOG_INFO("response: " << response);

  if (responseCode != 200)
  {
    BN_LOG_ERROR("curl post request error!");
   throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
  }

  std::string jsResult("");
  std::string jsTxId("");
  std::string jsReqSeq("");
  std::string jsError("");
  std::string jsId("");
  ndn::CMyJSON::getInstance()->readJSONSendBnRawTransaction(response, jsResult, jsTxId, jsReqSeq, jsError, jsId);
  //这时不够钱 就进行转账

  _LOG_INFO("jsResult: " << jsResult);
  _LOG_INFO("jsTxId: " << jsTxId);
  _LOG_INFO("jsReqSeq: " << jsReqSeq);
  _LOG_INFO("jsError: " << jsError);
  _LOG_INFO("jsId: " << jsId);

  //
  if(!jsError.empty())
  {
    //如果code == -6 进行转账
    Json::Reader reader;
    Json::Value value;
    int errorCode = 0;
    if (reader.parse(jsError, value))
    {
       errorCode = value["code"].asInt();
    }
    if(-6 == errorCode)
    {
      CWallet *tmp = myWallet[atoi(chainid.c_str())];
      std::string content("");
      std::string mtxid;
      std::string amount1("50");
      for(int i = 0;i < nodeCount;i++)
      {
        std::string url;
        std::string header;
        std::string address;
        std::string schainid;
        
        getFromWalletArray(i*chainTotal+atoi(schainid.c_str()),url,header,address,schainid);
        try{
            sendToAddress(holder,url,header,content,tmp->dftaddr,mtxid,amount1,tmp->chainid);
            response = "";
            ndn::CMyCURLPL::getInstance()->doHTTPPost(holder,sURL, sHeader, bodyJSON, responseCode, response);

            std::string jsResult0("");
            std::string jsTxId0("");
            std::string jsReqSeq0("");
            std::string jsError0("");
            std::string jsId0("");
            ndn::CMyJSON::getInstance()->readJSONSendBnRawTransaction(response, jsResult0, jsTxId0, jsReqSeq0, jsError0, jsId0);
            if(jsError0.empty())
              break;

        }catch(...)
        {
          continue;

        }
        
      }  
    }
    
    
  }

  if (jsResult.empty())
  {
    _LOG_ERROR("jsResult is empty!");
    throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_RESPONSE_ERR",__GLOBAL_RESULT_CURL_RESPONSE_ERR);
  }

  txid = jsTxId;
  _LOG_INFO("end getSendToAddressForRightV2Demo");
  return 0;
}

//end by liangyuanting 


int 
ndn::CMyBaseBIZ::uploadMarket(CQIHolder *holder)
{
      _LOG_INFO("begin to uploadMarket...");
      MYSQL* client = holder->client;
      MYSQL_RES * cursor;
      MYSQL_FIELD * fieldtitle;
      MYSQL_ROW records;
    
      std::string sql("select devicegroupid,market,balance from mymysql.market m;");

      int ret = 0;
      ret = ndn::CMyMYSQLPL::getInstance()->pingAliveIfReconn(holder);
      if (-1 == ret)
      {
        _LOG_ERROR("pingAliveIfReconn next...");
        return -1;
      }
      ret = mysql_real_query(client, sql.c_str(), strlen(sql.c_str()));
      if (ret != 0)
      {
        //_LOG_INFO("sql=" << sql);
         _PRINT_KEY_VALUE(sql);
        _LOG_ERROR("Error:" << mysql_errno(client) << "," << mysql_error(client));
       
      }
    
      cursor = mysql_store_result(client);
      if (cursor)
      {

        while ((records = mysql_fetch_row(cursor)))
        {
          _LOG_INFO("market=" << records[1]);
          CMarket *cm = new CMarket();
          cm->deviceGroupId=std::string(records[0]);
          cm->market=std::string(records[1]);
          cm->balance=atoi(records[2]);
          // putToMarketVectest(cm);
          putMarketToHashMap(cm->market,cm);
        }
        mysql_free_result(cursor);
        while (!mysql_next_result(client))
        {
          cursor = mysql_store_result(client);
          mysql_free_result(cursor);
        }
      }

    return 0;
};






int
ndn::CMyBaseBIZ::getGetBnRawTransaction(CQIHolder * holder,
                                      std::string & sURL,
                                      std::string & sHeader,
                                      std::string & address,
                                      std::string & txid,
                                      std::string & txContent)
{
  _LOG_INFO("begin getGetBnRawTransaction");

  std::string methodValue(__CONSENSUS_RPC_METHOD_GETBNRAWTRANSACTION);
  std::vector<std::string> paramsValues;
  paramsValues.clear();

  paramsValues.push_back(txid);

  std::string bodyJSON("");
  int responseCode = -1;
  std::string response("");
  ndn::CMyJSON::getInstance()->writeJSONArrayGetBnRawTransaction(methodValue, paramsValues, bodyJSON);
  _LOG_INFO("txContent: " << bodyJSON);
  ndn::CMyCURLPL::getInstance()->doHTTPPost(holder, sURL, sHeader, bodyJSON, responseCode, response);
  _LOG_INFO("responseCode: " << responseCode);
  _LOG_INFO("myBaseResponse: " << response);

  if (responseCode != 200)
  {
      BN_LOG_ERROR("curl post request error!");
   throw new CMy_BSDR_Exception("__GLOBAL_RESULT_CURL_POST_ERR",__GLOBAL_RESULT_CURL_POST_ERR);
  } 

  std::string jsResult("");
  std::string jsError("");
  std::string jsId("");
  if(-1 == ndn::CMyJSON::getInstance()->readJSONGetBnRawTransaction(response, jsResult, jsError, jsId))
  {
   BN_LOG_ERROR("readjson getbnrawTransaction is error");
    return -1;
  }
  
  _LOG_INFO("myBasejsResult: " << jsResult);
  _LOG_INFO("jsError: " << jsError);
  _LOG_INFO("jsId: " << jsId);

  if (jsResult.empty())
  {
    _LOG_INFO("jsResult is empty!");
    return -1;
  }

  txContent = jsResult;

  _LOG_INFO("end getGetBnRawTransaction");

  return 0;
}


/* code */
#endif //_MYBASEBIZ_CPP

