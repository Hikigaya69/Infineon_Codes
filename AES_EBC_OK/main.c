#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "optiga_util.h"
#include "optiga_crypt.h"

// Function to print data in a formatted way
void print_uint8_data(uint8_t data[], size_t len) {
    for(uint8_t i = 0; i < len; i++) {
        if((i % 16) == 0) {
            printf("\r\n");
        }
        printf("%d ", data[i]);
    }
    printf("\r\n");
}

// Callback function
optiga_lib_status_t op_status = OPTIGA_UTIL_BUSY;
void func(void* callback_ctx, optiga_lib_status_t event) {
    op_status = event;
}



uint8_t* pad_data(const uint8_t* input, size_t input_len, size_t* padded_len) {
    size_t padding_needed = 16 - (input_len % 16);
    *padded_len = input_len + padding_needed;

    uint8_t* padded_data = (uint8_t*)malloc(*padded_len);
    if (padded_data == NULL) {
        return NULL;
    }

    memcpy(padded_data, input, input_len);

    for (size_t i = input_len; i < *padded_len; i++) {
//        padded_data[i] = (uint8_t)padding_needed;
        padded_data[i] = 0;

    }

    return padded_data;
}

int main(void)
{
    cy_rslt_t result;

#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    result = cybsp_init();
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    __enable_irq();

    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
            CY_RETARGET_IO_BAUDRATE);
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    printf("\x1b[2J\x1b[;H");
    printf("Starting encryption demo...\r\n");

    // Create OPTIGA utility instance
    optiga_util_t* opt_util = optiga_util_create(0, func, NULL);
    if(opt_util == NULL) {
        printf("OPTIGA UTIL COULDN'T BE CREATED\r\n");
        CY_ASSERT(0);
    }

    // Open application
    optiga_lib_status_t api_status = optiga_util_open_application(opt_util, 0);
    if(api_status != OPTIGA_UTIL_SUCCESS) {
        printf("API STATUS NOT SUCCESSFUL\r\n");
        CY_ASSERT(0);
    }
    while(op_status == OPTIGA_UTIL_BUSY);

    if(op_status == OPTIGA_UTIL_SUCCESS) {
        printf("API OPERATION SUCCESSFUL\r\n");
    } else {
        printf("API OPERATION FAILURE\r\n");
        CY_ASSERT(0);
    }

    // Create OPTIGA crypt instance
    op_status = OPTIGA_CRYPT_BUSY;
    optiga_crypt_t* crypt_util = optiga_crypt_create(0, func, NULL);
    if(crypt_util == NULL) {
        printf("OPTIGA CRYPT COULDN'T BE CREATED\r\n");
        CY_ASSERT(0);
    }

    // Example input data (can be any length)
    uint8_t data[] = {2, 4, 2, 0, 0, 3, 7, 8, 9, 1, 2, 3, 4,
                         15, 23, 67, 89, 45, 12, 78, 56, 34, 90, 11, 24, 88, 92, 54, 33, 77, 101,
                         150, 200, 99, 87, 65, 43, 22, 76, 198, 210, 145, 167, 189, 233, 55, 44, 29, 83,
                         176, 254, 120, 30, 40, 50, 60, 70, 80, 13, 27, 39, 51, 63, 75, 85,
                         95, 105, 115, 125, 135, 145, 155, 165, 175, 185, 195, 205, 215, 225, 235, 245,
                         255, 142, 188, 99, 77, 66, 55, 44, 33, 22, 11, 202, 219, 233, 240, 123,
                         111, 98, 76, 54, 32, 21, 19, 5, 17, 31,2, 4, 2, 0, 0, 3, 7, 8, 9, 1, 2, 3, 4,
                         15, 23, 67, 89, 45, 12, 78, 56, 34, 90, 11, 24, 88, 92, 54, 33, 77, 101,
                         150, 200, 99, 87, 65, 43, 22, 76, 198, 210, 145, 167, 189, 233, 55, 44, 29, 83,
                         176, 254, 120, 30, 40, 50, 60, 70, 80, 13, 27, 39, 51, 63, 75, 85,
                         95, 105, 115, 125, 135, 145, 155, 165, 175, 185, 195, 205, 215, 225, 235, 245,
                         255, 142, 188, 99, 77, 66, 55, 44, 33, 22, 11, 202, 219, 233, 240, 123,
                         111, 98, 76, 54, 32, 21, 19, 5, 17, 31};
    size_t data_len = sizeof(data);

    uint8_t newData[256];
    memcpy(newData,data,sizeof(data));
    uint8_t newDatalen=sizeof(newData);
    uint32_t length = 100;

    // Pad data to multiple of 16 bytes
    size_t padded_len;
    uint8_t* padded_data = pad_data(newData,length , &padded_len);


    if (padded_data == NULL) {
        printf("Memory allocation failed\r\n");
        CY_ASSERT(0);
    }

    // Generate symmetric key
    optiga_key_id_t key = OPTIGA_KEY_ID_SECRET_BASED;
    api_status = optiga_crypt_symmetric_generate_key(crypt_util,
                                                   OPTIGA_SYMMETRIC_AES_128,
                                                   OPTIGA_KEY_USAGE_ENCRYPTION,
                                                   0,
                                                   &key);

    if(api_status != OPTIGA_CRYPT_SUCCESS) {
        printf("CRYPT AES KEY API STATUS NOT SUCCESSFUL\r\n");
        free(padded_data);
        CY_ASSERT(0);
    }

    while(op_status == OPTIGA_CRYPT_BUSY);
    if(op_status == OPTIGA_CRYPT_SUCCESS) {
        op_status = OPTIGA_CRYPT_BUSY;
        printf("CRYPT AES KEY OPERATION SUCCESSFUL\r\n");
    } else {
        printf("CRYPT AES KEY OPERATION FAILURE %0x\r\n", op_status);
        free(padded_data);
        CY_ASSERT(0);
    }

    // Allocate buffer for encrypted data (must be multiple of 16)
    uint8_t* encrypted = (uint8_t*)calloc(padded_len, sizeof(uint8_t));
    uint32_t encLength = padded_len;

    // Encrypt padded data
    api_status = optiga_crypt_symmetric_encrypt(crypt_util,
                                              OPTIGA_SYMMETRIC_ECB,
                                              OPTIGA_KEY_ID_SECRET_BASED,
                                              padded_data,
                                              padded_len,
                                              NULL, 0, NULL, 0,
                                              encrypted,
                                              &encLength);

    if(api_status != OPTIGA_CRYPT_SUCCESS) {
        printf("CRYPT AES API STATUS NOT SUCCESSFUL\r\n");
        free(padded_data);
        free(encrypted);
        CY_ASSERT(0);
    }

    while(op_status == OPTIGA_CRYPT_BUSY);
    if(op_status == OPTIGA_CRYPT_SUCCESS) {
        op_status = OPTIGA_CRYPT_BUSY;
        printf("CRYPT AES OPERATION SUCCESSFUL\r\n");
    } else {
        printf("CRYPT AES OPERATION FAILURE %d\r\n", op_status);
        free(padded_data);
        free(encrypted);
        CY_ASSERT(0);
    }

    printf("Original data length: %zu\r\n", data_len);
    printf("Padded data length: %zu\r\n", padded_len);
    printf("Original data: \r\n");
    print_uint8_data(data, data_len);
    printf("Encrypted data: \r\n");
    print_uint8_data(encrypted, encLength);

    // Allocate buffer for decrypted data
    uint8_t* decrypted = (uint8_t*)calloc(padded_len, sizeof(uint8_t));
    uint32_t decLength = padded_len;

    // Decrypt data
    api_status = optiga_crypt_symmetric_decrypt(crypt_util,
                                              OPTIGA_SYMMETRIC_ECB,
                                              OPTIGA_KEY_ID_SECRET_BASED,
                                              encrypted,
                                              encLength,
                                              NULL, 0, NULL, 0,
                                              decrypted,
                                              &decLength);

    if(api_status != OPTIGA_CRYPT_SUCCESS) {
        printf("CRYPT AES DECRYPT API STATUS NOT SUCCESSFUL\r\n");
        free(padded_data);
        free(encrypted);
        free(decrypted);
        CY_ASSERT(0);
    }

    while(op_status == OPTIGA_CRYPT_BUSY);
    if(op_status == OPTIGA_CRYPT_SUCCESS) {
        printf("CRYPT AES DECRYPT OPERATION SUCCESSFUL\r\n");
    } else {
        printf("CRYPT AES DECRYPT OPERATION FAILURE\r\n");
        free(padded_data);
        free(encrypted);
        free(decrypted);
        CY_ASSERT(0);
    }

    printf("Decrypted data (including padding): \r\n");
    print_uint8_data(decrypted, decLength);

    // Cleanup
    free(padded_data);
    free(encrypted);
    free(decrypted);

    api_status = optiga_crypt_destroy(crypt_util);
    if(api_status != OPTIGA_LIB_SUCCESS) {
        printf("CRYPT DESTROY FAILURE\r\n");
        CY_ASSERT(0);
    }

    op_status = OPTIGA_UTIL_BUSY;
    api_status = optiga_util_close_application(opt_util, 0);
    if(api_status != OPTIGA_UTIL_SUCCESS) {
        printf("API STATUS NOT SUCCESSFUL AT CLOSING\r\n");
        CY_ASSERT(0);
    }

    while(op_status == OPTIGA_UTIL_BUSY);
    if(op_status == OPTIGA_UTIL_SUCCESS) {
        printf("API OPERATION FOR CLOSING SUCCESSFUL\r\n");
    } else {
        printf("API OPERATION FAILURE FOR CLOSING\r\n");
        CY_ASSERT(0);
    }

    optiga_util_destroy(opt_util);

    for (;;)
    {
    }
}
