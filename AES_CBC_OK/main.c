#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ecp.h"
#include "ecdh.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "optiga_crypt.h"
#include "optiga_util.h"

void print_uint8_data(uint8_t *data, size_t len) {
	for (uint8_t i = 0; i < len; i++) {
		if ((i % 16) == 0) {
			printf("\r\n");
		}
		printf("%d  ", *(data + i));
	}
	printf("\r\n");
}

// Callback function
optiga_lib_status_t op_status = OPTIGA_UTIL_BUSY;
void func(void *callback_ctx, optiga_lib_status_t event) {
	op_status = event;
}

// Main function
int main(void) {
	cy_rslt_t result;

#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

	/* Initialize the device and board peripherals */
	result = cybsp_init();

	/* Board init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	/* Initialize retarget-io to use the debug UART port */
	result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
			CY_RETARGET_IO_BAUDRATE);

	/* UART port init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	__enable_irq();
	/* Clear screen */
	printf("\x1b[2J\x1b[;H");

	optiga_lib_status_t api_status;

	// Create OPTIGA utility instance
	optiga_util_t *opt_util = optiga_util_create(0, func, NULL);
	if (opt_util == NULL) {
		printf("OPTIGA UTIL COULD NOT BE CREATED\r\n");
		CY_ASSERT(0);
	}

	// Open OPTIGA application
	api_status = optiga_util_open_application(opt_util, 0);
	if (api_status != OPTIGA_LIB_SUCCESS) {
		printf("API STATUS NOT SUCCESSFUL\r\n");
		CY_ASSERT(0);
	}

	// Wait for operation to complete
	while (op_status == OPTIGA_UTIL_BUSY)
		;
	if (op_status == OPTIGA_UTIL_SUCCESS) {
		printf("API OPERATION SUCCESSFUL\r\n");
	} else {
		printf("API OPERATION FAILURE\r\n");
		CY_ASSERT(0);
	}

	op_status = OPTIGA_CRYPT_BUSY;

	// Create OPTIGA cryptographic instance
	optiga_crypt_t *crypt_util = optiga_crypt_create(0, func, NULL);
	if (crypt_util == NULL) {
		printf("OPTIGA CRYPT INSTANCE COULD NOT BE CREATED\r\n");
		CY_ASSERT(0);
	}

	// Define key generation parameters
	optiga_symmetric_key_type_t key_type = OPTIGA_SYMMETRIC_AES_128;
	uint8_t key_usage = OPTIGA_KEY_USAGE_ENCRYPTION;
	bool_t export_symmetric_key = false;
	optiga_key_id_t symmetric_key = OPTIGA_KEY_ID_SECRET_BASED;

	// Generate symmetric key
	api_status = optiga_crypt_symmetric_generate_key(crypt_util, key_type,
			key_usage, export_symmetric_key, &symmetric_key);

	// Check API response
	if (api_status != OPTIGA_LIB_SUCCESS) {
		printf("Symmetric key generation failed");
		CY_ASSERT(0);
	} else {
		printf("Symmetric key generated successfully!\r\n");

	}

	while (op_status == OPTIGA_CRYPT_BUSY) {
	}

	if (op_status == OPTIGA_CRYPT_SUCCESS) {
		printf("CRYPT OPERATION SUCESSFULL\r\n");
	} else {
		printf("CRYPT OPERATION FAILURE\r\n");
		CY_ASSERT(0);
	}

	op_status = OPTIGA_CRYPT_BUSY;

	//done with key generation

	//Define encryption parameters
	uint8_t data[16] = { 8,9,0,67,89,45,34};





	uint8_t iv[16] = {22,45,67,867,32,344,56,22,33 };
	uint32_t ivlength=sizeof(iv);
	uint32_t ivcopy[16];
	memcpy(ivcopy,iv,16);



	uint8_t encrypted[16] = { 0 };
	uint32_t encLength = sizeof(encrypted);


	api_status = optiga_crypt_symmetric_encrypt(crypt_util,
			OPTIGA_SYMMETRIC_CBC, OPTIGA_KEY_ID_SECRET_BASED, data,
			sizeof(data) / sizeof(data[0]), iv, ivlength, NULL, 0, encrypted,
			&encLength);

	if (api_status != OPTIGA_CRYPT_SUCCESS) {
		printf("CRYPT AES API STATUS NOT SUCESSFULL\r\n");
		CY_ASSERT(0);
	}

	while (op_status == OPTIGA_CRYPT_BUSY)
		;

	if (op_status == OPTIGA_CRYPT_SUCCESS) {
		op_status = OPTIGA_CRYPT_BUSY;
		printf("CRYPT AES OPERATION SUCESSFULL\r\n");
	} else {
		printf("CRYPT AES OPERATION FAILURE %d\r\n", op_status);
		CY_ASSERT(0);
	}

	printf("Encypted data: \r\n");
	print_uint8_data(encrypted, encLength);

	//done with encryption

	uint8_t decrypted[16] = { 0 };
	uint32_t decLength = sizeof(decrypted) / sizeof(decrypted[0]);

	api_status = optiga_crypt_symmetric_decrypt(crypt_util,
			OPTIGA_SYMMETRIC_CBC, OPTIGA_KEY_ID_SECRET_BASED, encrypted,
			encLength, ivcopy,16, NULL, 0, decrypted, &decLength);
	if (api_status != OPTIGA_CRYPT_SUCCESS) {
		printf("CRYPT AES DECRYPT API STATUS NOT SUCESSFULL\r\n");
		CY_ASSERT(0);
	}

	while (op_status == OPTIGA_CRYPT_BUSY)
		;

	if (op_status == OPTIGA_CRYPT_SUCCESS) {
		printf("CRYPT AES DECRYPT OPERATION SUCESSFULL\r\n");
	} else {
		printf("CRYPT AES DECRYPT OPERATION FAILURE\r\n");
		CY_ASSERT(0);
	}

	printf("DECRYPT  data: \r\n");
	print_uint8_data(decrypted, decLength);

	//destroy
	api_status = optiga_crypt_destroy(crypt_util);

	if (api_status != OPTIGA_LIB_SUCCESS) {
		printf("CRYPT DESTROY FAILURE\r\n");
		CY_ASSERT(0);
	}

	//close

	op_status = OPTIGA_UTIL_BUSY;

	api_status = optiga_util_close_application(opt_util, 0);

	if (api_status != OPTIGA_UTIL_SUCCESS) {
		printf("API STATUS NOT SUCESSFULL AT CLOSING\r\n");
		CY_ASSERT(0);
	}
	while (op_status == OPTIGA_UTIL_BUSY)
		;

	if (op_status == OPTIGA_UTIL_SUCCESS) {
		printf("API OPERATION FOR CLOSING SUCESSFULL\r\n");
	} else {
		printf("API OPERATION FAILURE FOR CLOSING\r\n");
		CY_ASSERT(0);
	}

	optiga_util_destroy(opt_util);

	for (;;) {
	}

}
