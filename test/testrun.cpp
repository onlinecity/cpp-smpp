#include <cppunit/TestCaller.h>
#include <cppunit/TestResult.h>
#include <cppunit/ui/text/TestRunner.h>
#include "pdutest.h"
#include "encodertest.h"
#include "timetest.h"
#include "smstest.h"

int main(int argc, char **argv)
{
	CppUnit::TextUi::TestRunner runner;
	runner.addTest(PduTest::suite());
	runner.addTest(EncoderTest::suite());
	runner.addTest(TimeTest::suite());
	runner.addTest(SmsTest::suite());
	return runner.run() ? 0 : 1;
}
