#include <gtest/gtest.h>
#include "mybizpl.hpp"
using namespace std;
using namespace ndn;


class BaarUnitTest : public ::testing::Test {
      public:
  	ndn::CMyBIZPL* instance;
    CQIHolder *holder;
protected:
  virtual void SetUp() {

    instance = ndn::CMyBIZPL::getInstance();
    holder = new CQIHolder;
    //加载
    string s1 = string("");
    s1.append(__XMLFILE);
    string s2 = string("");
    s2.append(_XMLFILE_PORT);
    
    Config::Instance()->ReadConfig(s1);
    Config::Instance()->ReadPortConfig(s2);
    Config::readRGWConfig();
    Config::Instance()->readJWTConfig();
    ndn::CMyBIZ::getInstance()->initMainTLib();

    instance->initWalletHashMap(s1);
    instance->initMysqlHolder(holder);
    //加载market表数据
    instance->uploadMarket(holder);

    instance->initMySQLPool();

  }

  virtual void TearDown() {

        Config::Instance()->portVec.clear();
        Config::Instance()->rgwVec.clear();
        Config::Instance()->dbvec.clear();
        ndn::CMyBIZPL::getInstance()->cqiHolderDestroy();

  }


};


TEST_F(BaarUnitTest, produceDataTrans) {


ProducerData data;
data.strOperType = "getregdata";
data.param ="{\"BlockLabel\": \"abc\", \
						\"VirtualId\": \"def\", \
            \"AuthToken\": \"def\", \
						\"ProviderMarket\": \"def\", \
						\"DeviceGroupId\": \"HIJ\", \
						\"Verification\": \"ASDJKS\"\
            }"; \
    CReg creg;
    writeJsonToCReg(data,creg);
    cout<<creg.blocklabel;                    




}