#include "unity.h"
#include "test_phev_register.c"
#include "test_phev_pipe.c"

int main()
{
    UNITY_BEGIN();

//  PHEV_REGISTER

    RUN_TEST(test_phev_register_bootstrap);
    RUN_TEST(test_phev_register_getVin);

//  PHEV PIPE
    
    RUN_TEST(test_phev_pipe_createPipe);
    RUN_TEST(test_phev_pipe_outputChainInputTransformer);
    return UNITY_END();
}