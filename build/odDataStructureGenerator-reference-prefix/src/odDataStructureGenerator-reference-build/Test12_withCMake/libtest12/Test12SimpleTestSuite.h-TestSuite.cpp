/* Generated file, do not edit */

#ifndef CXXTEST_RUNNING
#define CXXTEST_RUNNING
#endif

#include <fstream>
#define _CXXTEST_HAVE_EH
#include <cxxtest/TestListener.h>
#include <cxxtest/TestTracker.h>
#include <cxxtest/TestRunner.h>
#include <cxxtest/RealDescriptions.h>
#include <cxxtest/TestMain.h>
#include <cxxtest/XUnitPrinter.h>

int main( int argc, char *argv[] ) {
 int status;
    std::ofstream ofstr("TEST-libtest12-Test12SimpleTestSuite.h.xml");
    CxxTest::XUnitPrinter tmp(ofstr);
    CxxTest::RealWorldDescription::_worldName = "libtest12-Test12SimpleTestSuite.h";
    status = CxxTest::Main< CxxTest::XUnitPrinter >( tmp, argc, argv );
    return status;
}
bool suite_Test12Simple_TestSuite_init = false;
#include "../../../../../../odDataStructureGenerator/org.opendavinci.datamodel/test/odvd/reference/Test12_withCMake/libtest12/testsuites/Test12SimpleTestSuite.h"

static Test12Simple_TestSuite suite_Test12Simple_TestSuite;

static CxxTest::List Tests_Test12Simple_TestSuite = { 0, 0 };
CxxTest::StaticSuiteDescription suiteDescription_Test12Simple_TestSuite( "/home/viktor/OpenDaVINCI/odDataStructureGenerator/org.opendavinci.datamodel/test/odvd/reference/Test12_withCMake/libtest12/testsuites/Test12SimpleTestSuite.h", 25, "Test12Simple_TestSuite", suite_Test12Simple_TestSuite, Tests_Test12Simple_TestSuite );

static class TestDescription_suite_Test12Simple_TestSuite_testCreateObject : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test12Simple_TestSuite_testCreateObject() : CxxTest::RealTestDescription( Tests_Test12Simple_TestSuite, suiteDescription_Test12Simple_TestSuite, 28, "testCreateObject" ) {}
 void runTest() { suite_Test12Simple_TestSuite.testCreateObject(); }
} testDescription_suite_Test12Simple_TestSuite_testCreateObject;

static class TestDescription_suite_Test12Simple_TestSuite_testCreateAndCopyObject : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test12Simple_TestSuite_testCreateAndCopyObject() : CxxTest::RealTestDescription( Tests_Test12Simple_TestSuite, suiteDescription_Test12Simple_TestSuite, 56, "testCreateAndCopyObject" ) {}
 void runTest() { suite_Test12Simple_TestSuite.testCreateAndCopyObject(); }
} testDescription_suite_Test12Simple_TestSuite_testCreateAndCopyObject;

static class TestDescription_suite_Test12Simple_TestSuite_testCreateAndAssignObject : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test12Simple_TestSuite_testCreateAndAssignObject() : CxxTest::RealTestDescription( Tests_Test12Simple_TestSuite, suiteDescription_Test12Simple_TestSuite, 112, "testCreateAndAssignObject" ) {}
 void runTest() { suite_Test12Simple_TestSuite.testCreateAndAssignObject(); }
} testDescription_suite_Test12Simple_TestSuite_testCreateAndAssignObject;

static class TestDescription_suite_Test12Simple_TestSuite_testCreateAndSerializeObject : public CxxTest::RealTestDescription {
public:
 TestDescription_suite_Test12Simple_TestSuite_testCreateAndSerializeObject() : CxxTest::RealTestDescription( Tests_Test12Simple_TestSuite, suiteDescription_Test12Simple_TestSuite, 168, "testCreateAndSerializeObject" ) {}
 void runTest() { suite_Test12Simple_TestSuite.testCreateAndSerializeObject(); }
} testDescription_suite_Test12Simple_TestSuite_testCreateAndSerializeObject;

#include <cxxtest/Root.cpp>
const char* CxxTest::RealWorldDescription::_worldName = "cxxtest";