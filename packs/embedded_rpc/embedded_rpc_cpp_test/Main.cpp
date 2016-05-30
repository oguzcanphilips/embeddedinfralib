#include "gtest/gtest.h"


int main(int argc, char **argv) 
{
    testing::FLAGS_gtest_filter="TestCommunicationCPPAscii*";
    
    ::testing::InitGoogleTest(&argc, argv);
    _exit(RUN_ALL_TESTS());
}
