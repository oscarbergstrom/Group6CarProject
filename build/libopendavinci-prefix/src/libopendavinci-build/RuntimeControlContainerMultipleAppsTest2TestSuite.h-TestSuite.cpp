/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <fstream>
#define _CXXTEST_HAVE_STD
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/XUnitPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    std::ofstream ofstr("TEST-libopendavinci-RuntimeControlContainerMultipleAppsTest2TestSuite.h.xml");
    CxxTest::XUnitPrinter tmp(ofstr);
    CxxTest::RealWorldDescription::_worldName = "libopendavinci-RuntimeControlContainerMultipleAppsTest2TestSuite.h";
    status = CxxTest::Main< CxxTest::XUnitPrinter >( tmp, argc, argv );
    return status;
}
bool suite_RuntimeControlContainerMultipleAppsTest_init = false;
#include "../../../../libopendavinci/testsuites/RuntimeControlContainerMultipleAppsTest2TestSuite.h"

static RuntimeControlContainerMultipleAppsTest suite_RuntimeControlContainerMultipleAppsTest;

static CxxTest::List Tests_RuntimeControlContainerMultipleAppsTest = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_RuntimeControlContainerMultipleAppsTest( "/home/viktor/OpenDaVINCI/libopendavinci/testsuites/RuntimeControlContainerMultipleAppsTest2TestSuite.h", 499, "RuntimeControlContainerMultipleAppsTest", suite_RuntimeControlContainerMultipleAppsTest, Tests_RuntimeControlContainerMultipleAppsTest );

static class TestDescription_suite_RuntimeControlContainerMultipleAppsTest_testRuntimeControlContainerRegularRunReceivingSendingTwoAppsOneReceivingDataFromOtherAppSameFreqOneSP : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_RuntimeControlContainerMultipleAppsTest_testRuntimeControlContainerRegularRunReceivingSendingTwoAppsOneReceivingDataFromOtherAppSameFreqOneSP() : CxxTest::RealTestDescription( Tests_RuntimeControlContainerMultipleAppsTest, suiteDescription_RuntimeControlContainerMultipleAppsTest, 502, "testRuntimeControlContainerRegularRunReceivingSendingTwoAppsOneReceivingDataFromOtherAppSameFreqOneSP" ) {}
 void runTest() { suite_RuntimeControlContainerMultipleAppsTest.testRuntimeControlContainerRegularRunReceivingSendingTwoAppsOneReceivingDataFromOtherAppSameFreqOneSP(); }
} testDescription_suite_RuntimeControlContainerMultipleAppsTest_testRuntimeControlContainerRegularRunReceivingSendingTwoAppsOneReceivingDataFromOtherAppSameFreqOneSP;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";
