#include "myxml.hpp"

//end 20170601 liulijin add

INIT_LOGGER("CMyXML");

ndn::CMyXML * ndn::CMyXML::instance = new CMyXML;

ndn::CMyXML *
ndn::CMyXML::getInstance()
{
  return instance;
}

int
ndn::CMyXML::loadLocalName(const std::string & xmlfile, std::string & result)
{
  _LOG_INFO("begin loadLocalName");

  TiXmlDocument * pDoc = new TiXmlDocument(xmlfile.c_str());
  pDoc->LoadFile();

  TiXmlElement * pRoot = pDoc->RootElement();
  TiXmlElement * child = pRoot->FirstChildElement(__XML_ITEMNAME_DEVICEGROUPID);
  TiXmlAttribute * pAttr = child->FirstAttribute();
  result = pAttr->Value();

  pDoc->Clear();
  pDoc=NULL;
  _LOG_INFO("end loadLocalName");
  return 0;
}

int
ndn::CMyXML::createDataContentError(const std::string & code,
                                    const std::string & reason,
                                    std::string & result)
{
  _LOG_INFO("begin createDataContentError");

  TiXmlDocument * pDoc = new TiXmlDocument;
  TiXmlElement * pRoot = new TiXmlElement(__XML_ITEMNAME_ERROR);
  pDoc->LinkEndChild(pRoot);

  TiXmlElement * pCode = new TiXmlElement(__XML_ITEMNAME_CODE);
  pCode->InsertEndChild(TiXmlText(code.c_str()));
  TiXmlElement * pReason = new TiXmlElement(__XML_ITEMNAME_REASON);
  pReason->InsertEndChild(TiXmlText(reason.c_str()));
  pRoot->InsertEndChild(*pCode);
  pRoot->InsertEndChild(*pReason);

  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  pDoc->Accept(&printer);

  result = printer.CStr();

  pDoc->Clear();

  _LOG_INFO("end createDataContentError");
  return 0;
}

int
ndn::CMyXML::createDataContentContentListItem(const std::string & blockedlabel,
                                              const std::string & marketer,
                                              const std::string & price,
                                              const std::string & srcdeviceno,
                                              const std::string & timestamp,
                                              const std::string & content,
                                              std::string & item)
{
  _LOG_INFO("begin createDataContentContentListItem");

  TiXmlDocument * pDoc = new TiXmlDocument;
  TiXmlElement * pRoot = new TiXmlElement(__XML_ITEMNAME_ITEM);
  pDoc->LinkEndChild(pRoot);

  TiXmlElement * pBlockedLabel = new TiXmlElement(__XML_ITEMNAME_BLOCKEDLABEL);
  pBlockedLabel->InsertEndChild(TiXmlText(blockedlabel.c_str()));
  pRoot->InsertEndChild(*pBlockedLabel);

  TiXmlElement * pMarketer = new TiXmlElement(__XML_ITEMNAME_MARKET);
  pMarketer->InsertEndChild(TiXmlText(marketer.c_str()));
  pRoot->InsertEndChild(*pMarketer);

  TiXmlElement * pPrice = new TiXmlElement(__XML_ITEMNAME_PRICE);
  pPrice->InsertEndChild(TiXmlText(price.c_str()));
  pRoot->InsertEndChild(*pPrice);

  TiXmlElement * pSrcDeviceNo = new TiXmlElement(__XML_ITEMNAME_SRCDEVICENO);
  pSrcDeviceNo->InsertEndChild(TiXmlText(srcdeviceno.c_str()));
  pRoot->InsertEndChild(*pSrcDeviceNo);

  TiXmlElement * pTimeStamp = new TiXmlElement(__XML_ITEMNAME_TIMESTAMP);
  pTimeStamp->InsertEndChild(TiXmlText(timestamp.c_str()));
  pRoot->InsertEndChild(*pTimeStamp);

  TiXmlElement * pContent = new TiXmlElement(__XML_ITEMNAME_CONTENT);
  pContent->InsertEndChild(TiXmlText(content.c_str()));
  pRoot->InsertEndChild(*pContent);

  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  pDoc->Accept(&printer);

  item = printer.CStr();

  pDoc->Clear();

  _LOG_INFO("end createDataContentContentListItem");
  return 0;
}

int
ndn::CMyXML::createDataContentContentList(std::vector<std::string> & items,
                                          std::string & result)
{
  _LOG_INFO("begin createDataContentContentList");

  result.append(__XML_ITEMNAME_ROOT_BEGIN);
  for (unsigned int i = 0; i < items.size(); i++)
  {
    result.append(items[i]);
  }
  result.append(__XML_ITEMNAME_ROOT_END);

  _LOG_INFO("end createDataContentContentList");
  return 0;
}

int
ndn::CMyXML::createDataContentPurchaseListItem(const std::string & blockedlabel,
                                               const std::string & marketer,
                                               const std::string & txid,
                                               const std::string & txcontent,
                                               std::string & item)
{
  _LOG_INFO("begin createDataContentPurchaseListItem");

  TiXmlDocument * pDoc = new TiXmlDocument;
  TiXmlElement * pRoot = new TiXmlElement(__XML_ITEMNAME_ITEM);
  pDoc->LinkEndChild(pRoot);

  TiXmlElement * pBlockedLabel = new TiXmlElement(__XML_ITEMNAME_BLOCKEDLABEL);
  pBlockedLabel->InsertEndChild(TiXmlText(blockedlabel.c_str()));
  pRoot->InsertEndChild(*pBlockedLabel);

  TiXmlElement * pMarketer = new TiXmlElement(__XML_ITEMNAME_MARKET);
  pMarketer->InsertEndChild(TiXmlText(marketer.c_str()));
  pRoot->InsertEndChild(*pMarketer);

  TiXmlElement * pTxId = new TiXmlElement(__XML_ITEMNAME_TXID);
  pTxId->InsertEndChild(TiXmlText(txid.c_str()));
  pRoot->InsertEndChild(*pTxId);

  TiXmlElement * pTxContent = new TiXmlElement(__XML_ITEMNAME_TXCONTENT);
  pTxContent->InsertEndChild(TiXmlText(txcontent.c_str()));
  pRoot->InsertEndChild(*pTxContent);

  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  pDoc->Accept(&printer);

  item = printer.CStr();

  pDoc->Clear();

  _LOG_INFO("end createDataContentPurchaseListItem");
  return 0;
}
//begin 20170824 liangyuanting
int 
ndn::CMyXML::createCountXML(const std::string &count,std::string &item){
  
  _LOG_INFO("begin createCountXML");
  TiXmlDocument * pDoc = new TiXmlDocument;
  TiXmlElement * pRoot = new TiXmlElement(__XML_ITEMNAME_ITEM);
  pDoc->LinkEndChild(pRoot);

  TiXmlElement * pcount = new TiXmlElement(__XML_ITEMNAME_COUNT);
  pcount->InsertEndChild(TiXmlText(count.c_str()));
  pRoot->InsertEndChild(*pcount);

  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  pDoc->Accept(&printer);

  item = printer.CStr();

  pDoc->Clear();

  _LOG_INFO("end createCountXML");
  
  return 0;
}

int 
ndn::CMyXML::createOneXML(const std::string &item,std::string &result){

  _LOG_INFO("begin createOneXML");
  result.append(__XML_ITEMNAME_ROOT_BEGIN);
  result.append(item); 
  result.append(__XML_ITEMNAME_ROOT_END);
  _LOG_INFO("end createOneXML");
  
  return 0;
}
//end 20170824 liangyuanting





int
ndn::CMyXML::createDataContentPurchaseList(std::vector<std::string> & items,
                                           std::string & result)
{
  _LOG_INFO("begin createDataContentPurchaseList");

  result.append(__XML_ITEMNAME_ROOT_BEGIN);
  for (unsigned int i = 0; i < items.size(); i++)
  {
    result.append(items[i]);
  }
  result.append(__XML_ITEMNAME_ROOT_END);

  _LOG_INFO("end createDataContentPurchaseList");
  return 0;
}

int
ndn::CMyXML::parseXMLtoArray(const string & input,
                             std::vector<std::string> & items)
{
  _LOG_INFO("begin parseXMLtoArray");

  TiXmlDocument * pDoc = new TiXmlDocument();
  pDoc->Parse(input.c_str());

  TiXmlHandle docHandle(pDoc);
  TiXmlElement * root = docHandle.FirstChild(__XML_ITENNAME_ROOT).ToElement();

  if (root == NULL)
  {
   //fprintf(stderr, "root is null");
    _LOG_INFO("root is null");
    return -1;
  }

  std::string item("");
  std::string itemResult("");

  for (TiXmlElement * elem = root->FirstChildElement(); \
      elem != NULL; \
      elem = elem->NextSiblingElement())
  {
    TiXmlElement * pBlockedLabel = elem->FirstChildElement();
    TiXmlElement * pMarketer = pBlockedLabel->NextSiblingElement();
    TiXmlElement * pPrice = pMarketer->NextSiblingElement();
    TiXmlElement * pSrcDeviceNo = pPrice->NextSiblingElement();
    TiXmlElement * pTimeStamp = pSrcDeviceNo->NextSiblingElement();
    TiXmlElement * pContent = pTimeStamp->NextSiblingElement();

    createDataContentContentListItem(pBlockedLabel->GetText(), \
                                     pMarketer->GetText(), \
                                     pPrice->GetText(), \
                                     pSrcDeviceNo->GetText(), \
                                     pTimeStamp->GetText(), \
                                     pContent->GetText(), \
                                     item);
    itemResult.append(__XML_ITEMNAME_ROOT_BEGIN);
    itemResult.append(item);
    itemResult.append(__XML_ITEMNAME_ROOT_END);
    items.push_back(itemResult);

    item="";
    itemResult="";
  }

  pDoc->Clear();

  _LOG_INFO("end parseXMLtoArray");
  return 0;
}

int
ndn::CMyXML::regMoreInfoCreate(std::vector<std::string> & input,
                               std::string & output)
{
  _LOG_INFO("begin regMoreInfoCreate");

  TiXmlDocument * pDoc = new TiXmlDocument;
  TiXmlElement * pRoot = new TiXmlElement(__XML_MOREINFO_ROOT);
  pDoc->LinkEndChild(pRoot);

  TiXmlElement * pBlockedLabel = new TiXmlElement(__XML_ITEMNAME_BLOCKEDLABEL);
  pBlockedLabel->InsertEndChild(TiXmlText(input[0].c_str()));
  pRoot->InsertEndChild(*pBlockedLabel);

  TiXmlElement * pMarketer = new TiXmlElement(__XML_ITEMNAME_MARKET);
  pMarketer->InsertEndChild(TiXmlText(input[1].c_str()));
  pRoot->InsertEndChild(*pMarketer);

  TiXmlElement * pPrice = new TiXmlElement(__XML_ITEMNAME_PRICE);
  pPrice->InsertEndChild(TiXmlText(input[2].c_str()));
  pRoot->InsertEndChild(*pPrice);

  TiXmlElement * pSrcDeviceNo = new TiXmlElement(__XML_ITEMNAME_SRCDEVICENO);
  pSrcDeviceNo->InsertEndChild(TiXmlText(input[3].c_str()));
  pRoot->InsertEndChild(*pSrcDeviceNo);

  TiXmlElement * pTimeStamp = new TiXmlElement(__XML_ITEMNAME_TIMESTAMP);
  pTimeStamp->InsertEndChild(TiXmlText(input[4].c_str()));
  pRoot->InsertEndChild(*pTimeStamp);

  TiXmlElement * pContent = new TiXmlElement(__XML_ITEMNAME_CONTENT);
  pContent->InsertEndChild(TiXmlText(input[5].c_str()));
  pRoot->InsertEndChild(*pContent);

  TiXmlElement * pHashDigest = new TiXmlElement(__XML_MOREINFO_SHA256);
  pHashDigest->InsertEndChild(TiXmlText(input[6].c_str()));
  pRoot->InsertEndChild(*pHashDigest);

  TiXmlElement * pProcTime = new TiXmlElement(__XML_MOREINFO_PROCTIME);
  pProcTime->InsertEndChild(TiXmlText(input[7].c_str()));
  pRoot->InsertEndChild(*pProcTime);

  TiXmlElement * pProcType = new TiXmlElement(__XML_MOREINFO_PROCTYPE);
  pProcType->InsertEndChild(TiXmlText(input[8].c_str()));
  pRoot->InsertEndChild(*pProcType);

  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  pDoc->Accept(&printer);

  output = printer.CStr();

  pDoc->Clear();

  _LOG_INFO("end regMoreInfoCreate");
  return 0;
}

int
ndn::CMyXML::payMoreInfoCreate(std::vector<std::string> & input,
                               std::string & output)
{
  _LOG_INFO("begin payMoreInfoCreate");

  TiXmlDocument * pDoc = new TiXmlDocument;
  TiXmlElement * pRoot = new TiXmlElement(__XML_MOREINFO_ROOT);
  pDoc->LinkEndChild(pRoot);

  TiXmlElement * pSrcMarketer = new TiXmlElement(__XML_MOREINFO_SRCMARKET);
  pSrcMarketer->InsertEndChild(TiXmlText(input[0].c_str()));
  pRoot->InsertEndChild(*pSrcMarketer);

  TiXmlElement * pDstMarketer = new TiXmlElement(__XML_MOREINFO_DSTMARKET);
  pDstMarketer->InsertEndChild(TiXmlText(input[1].c_str()));
  pRoot->InsertEndChild(*pDstMarketer);

  TiXmlElement * pBlockedLabel = new TiXmlElement(__XML_ITEMNAME_BLOCKEDLABEL);
  pBlockedLabel->InsertEndChild(TiXmlText(input[2].c_str()));
  pRoot->InsertEndChild(*pBlockedLabel);

  TiXmlElement * pPrice = new TiXmlElement(__XML_ITEMNAME_PRICE);
  pPrice->InsertEndChild(TiXmlText(input[3].c_str()));
  pRoot->InsertEndChild(*pPrice);

  TiXmlElement * pProcTime = new TiXmlElement(__XML_MOREINFO_PROCTIME);
  pProcTime->InsertEndChild(TiXmlText(input[4].c_str()));
  pRoot->InsertEndChild(*pProcTime);

  TiXmlElement * pProcType = new TiXmlElement(__XML_MOREINFO_PROCTYPE);
  pProcType->InsertEndChild(TiXmlText(input[5].c_str()));
  pRoot->InsertEndChild(*pProcType);

  TiXmlPrinter printer;
  printer.SetStreamPrinting();
  pDoc->Accept(&printer);

  output = printer.CStr();

  pDoc->Clear();

  _LOG_INFO("end payMoreInfoCreate");
  return 0;
}

int
ndn::CMyXML::regMoreInfoParse(const string & input)
{
  _LOG_INFO("begin regMoreInfoParse");

  TiXmlDocument * pDoc = new TiXmlDocument();
  pDoc->Parse(input.c_str());

  TiXmlHandle docHandle(pDoc);
  TiXmlElement * root = docHandle.FirstChild(__XML_MOREINFO_ROOT).ToElement();

  TiXmlElement * pBlockedLabel = root->FirstChildElement();
  pBlockedLabel->GetText();

  TiXmlElement * pMarketer = pBlockedLabel->NextSiblingElement();
  pMarketer->GetText();

  TiXmlElement * pPrice = pMarketer->NextSiblingElement();
  pPrice->GetText();

  TiXmlElement * pSrcDeviceNo = pPrice->NextSiblingElement();
  pSrcDeviceNo->GetText();

  TiXmlElement * pTimeStamp = pSrcDeviceNo->NextSiblingElement();
  pTimeStamp->GetText();

  TiXmlElement * pContent = pTimeStamp->NextSiblingElement();
  pContent->GetText();

  TiXmlElement * pHashDigest = pContent->NextSiblingElement();
  pHashDigest->GetText();

  TiXmlElement * pProcTime = pHashDigest->NextSiblingElement();
  pProcTime->GetText();

  TiXmlElement * pProcType = pProcTime->NextSiblingElement();
  pProcType->GetText();

  pDoc->Clear();

  _LOG_INFO("end regMoreInfoParse");
  return 0;
}

int
ndn::CMyXML::payMoreInfoParse(const string & input)
{
  _LOG_INFO("begin payMoreInfoParse");

  TiXmlDocument * pDoc = new TiXmlDocument();
  pDoc->Parse(input.c_str());

  TiXmlHandle docHandle(pDoc);
  TiXmlElement * root = docHandle.FirstChild(__XML_MOREINFO_ROOT).ToElement();

  TiXmlElement * pSrcMarketer = root->FirstChildElement();
  pSrcMarketer->GetText();

  TiXmlElement * pDstMarketer = pSrcMarketer->NextSiblingElement();
  pDstMarketer->GetText();

  TiXmlElement * pBlockedLabel = pDstMarketer->NextSiblingElement();
  pBlockedLabel->GetText();

  TiXmlElement * pPrice = pBlockedLabel->NextSiblingElement();
  pPrice->GetText();

  TiXmlElement * pProcTime = pPrice->NextSiblingElement();
  pProcTime->GetText();

  TiXmlElement * pProcType = pProcTime->NextSiblingElement();
  pProcType->GetText();

  pDoc->Clear();

  _LOG_INFO("end payMoreInfoParse");
  return 0;
}

