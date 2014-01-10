#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/ui/text/TestRunner.h>
#include <cppunit/extensions/HelperMacros.h>

#include "pdutest.h"
#include "encodertest.h"
#include "timetest.h"
#include "smstest.h"

CPPUNIT_TEST_SUITE_REGISTRATION(PduTest);
CPPUNIT_TEST_SUITE_REGISTRATION(EncoderTest);
CPPUNIT_TEST_SUITE_REGISTRATION(TimeTest);
CPPUNIT_TEST_SUITE_REGISTRATION(SmsTest);

int main(int argc, char **argv) {
    CppUnit::TextUi::TestRunner runner;
    CppUnit::TestFactoryRegistry &registry = CppUnit::TestFactoryRegistry::getRegistry();
    runner.addTest(registry.makeTest());
    return runner.run() ? 0 : 1;
}
