#include "unity.h"
#include "phev_store.h"
#include "mock_logger.h"

void setUp(void)
{
    hexdump_Ignore();
}

void test_create_store(void)
{
    phevStore_t * store = phev_store_create();

    TEST_ASSERT_NOT_NULL(store);
}
void test_add_to_store(void)
{

    const uint8_t data[] = {1,2,3,4};

    uint8_t reg = 0x11;
    
    phevStore_t * store = phev_store_create();

    int ret = phev_store_add(store,reg,data,4);

    TEST_ASSERT_EQUAL(1,ret);
} 
void test_get_from_store(void)
{
    const uint8_t data[] = {1,2,3,4};

    phevStore_t * store = phev_store_create();

    int ret = phev_store_add(store,0x11,data,4);

    phevRegister_t * msg2 = phev_store_get(store,0x11);

    TEST_ASSERT_NOT_NULL(msg2);

    TEST_ASSERT_EQUAL_MEMORY(data,msg2->data,4);

} 
void test_get_from_store_not_found(void)
{
    phevStore_t * store = phev_store_create();

    phevRegister_t * msg2 = phev_store_get(store,0x12);

    TEST_ASSERT_NULL(msg2);

} 
void test_update_store(void)
{
    const uint8_t data[] = {1,2,3,4};

    const uint8_t replacementData[] = {1,5,3,4};

    phevStore_t * store = phev_store_create();

    int ret = phev_store_add(store,0x11,data,4);

    phevRegister_t * msg = phev_store_get(store,0x11);

    TEST_ASSERT_NOT_NULL(msg);

    TEST_ASSERT_EQUAL_MEMORY(data,msg->data,4);

    ret = phev_store_add(store,0x11,replacementData,4);

    phevRegister_t * msg2 = phev_store_get(store,0x11);

    TEST_ASSERT_NOT_NULL(msg2);

    TEST_ASSERT_EQUAL_MEMORY(replacementData,msg2->data,4);
}
void test_store_compare(void)
{
    const uint8_t data[] = {1,2,3,4};

    const uint8_t newData[] = {1,2,3,4};
    
    phevStore_t * store = phev_store_create();

    phev_store_add(store,0x11,data,4);

    int ret = phev_store_compare(store,0x11,newData);

    TEST_ASSERT_EQUAL(0,ret);

} 
void test_store_compare_not_same(void)
{
    const uint8_t data[] = {1,2,3,4};

    const uint8_t newData[] = {1,2,4,4};
    
    phevStore_t * store = phev_store_create();

    phev_store_add(store,0x11,data,4);

    int ret = phev_store_compare(store,0x11,newData);

    TEST_ASSERT_NOT_EQUAL(0,ret);

} 
void test_store_compare_not_set(void)
{
    const uint8_t data[] = {1,2,3,4};

    phevStore_t * store = phev_store_create();

    int ret = phev_store_compare(store,0x11,data);

    TEST_ASSERT_NOT_EQUAL(0,ret);

}