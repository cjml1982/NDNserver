/*
 * test_main.cpp
 *
 *  Created on: 2018年1月22日
 *      Author: bn
 */
#include <gtest/gtest.h>
#include "../mywallet.h"
using namespace std;
using namespace ndn;
class Fenv:public testing::Environment{
public:

    virtual void SetUp()
    {
        std::cout << "Fenv SetUP" << std::endl;
		
    }
    virtual void TearDown()
    {
        std::cout << "Fenv TearDown" << std::endl;
    }
};

int main(int argc, char **argv) {
	testing::AddGlobalTestEnvironment(new Fenv);
	testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}




