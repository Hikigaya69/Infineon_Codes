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

	op_status = OPTIGA_UTIL_BUSY;

	//alice

	optiga_key_id_t alice_private_key = OPTIGA_KEY_ID_E0F2;
	uint8_t alice_public_key[100] = { 0 };
	uint16_t alice_pub_len = sizeof(alice_public_key);
	optiga_lib_status_t key_status_alice = optiga_crypt_ecc_generate_keypair(
			crypt_util, OPTIGA_ECC_CURVE_NIST_P_256,
			OPTIGA_KEY_USAGE_KEY_AGREEMENT, 0, &alice_private_key,
			alice_public_key, &alice_pub_len);

	if (key_status_alice == OPTIGA_CRYPT_SUCCESS) {
		printf("api  success alice \r\n");
	} else {
		printf("api call failure alice\r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY)
		;
	if (op_status == OPTIGA_UTIL_SUCCESS) {
		printf("API OPERATION SUCCESSFUL\r\n");
	} else {
		printf("API OPERATION FAILURE\r\n");
	}

	op_status = OPTIGA_UTIL_BUSY;

	//bob

	optiga_key_id_t bob_private_key = OPTIGA_KEY_ID_E0F3;
	uint8_t bob_public_key[100] = { 0 };
	uint16_t bob_pub_len = sizeof(bob_public_key);
	optiga_lib_status_t key_status_bob = optiga_crypt_ecc_generate_keypair(
			crypt_util, OPTIGA_ECC_CURVE_NIST_P_256,
			OPTIGA_KEY_USAGE_KEY_AGREEMENT, 0, &bob_private_key, bob_public_key,
			&bob_pub_len);

	if (key_status_bob == OPTIGA_CRYPT_SUCCESS) { // Changed to check for SUCCESS
		printf("api success bob\r\n");
	} else {
		printf("api call failure\r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY)
		;
	if (op_status == OPTIGA_UTIL_SUCCESS) {
		printf("API OPERATION SUCCESSFUL bob\r\n");
	} else {
		printf("API OPERATION FAILURE\r\n");
		CY_ASSERT(0);
	}

	op_status = OPTIGA_UTIL_BUSY;

	//ecdh

	// bob public struct for alice
	uint8_t alice_shared_key[100] = { 0 };

	public_key_from_host_t bob_pub;
	bob_pub.public_key = bob_public_key;
	bob_pub.length = bob_pub_len;
	bob_pub.key_type = OPTIGA_ECC_CURVE_NIST_P_256;

	optiga_lib_status_t ecdh_status = optiga_crypt_ecdh(crypt_util,
			alice_private_key, &bob_pub, 1, alice_shared_key);

	if (ecdh_status == OPTIGA_CRYPT_SUCCESS) {  // Changed to check for SUCCESS
		printf("api ecdh alice \r\n");
	} else {
		printf("api call failure\r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY)
		;
	if (op_status == OPTIGA_UTIL_SUCCESS) {
		printf("API OPERATION SUCCESSFUL alice ecdh\r\n");
	} else {
		printf("API OPERATION FAILURE\r\n");
		CY_ASSERT(0);
	}

	op_status = OPTIGA_UTIL_BUSY;

	print_uint8_data(alice_shared_key, 32);

	// alice public struct for bob
	uint8_t bob_shared_key[100] = { 0 };

	public_key_from_host_t alice_pub;
	alice_pub.public_key = alice_public_key;
	alice_pub.length = alice_pub_len;
	alice_pub.key_type = OPTIGA_ECC_CURVE_NIST_P_256;

	optiga_lib_status_t ecdh_status_bob = optiga_crypt_ecdh(crypt_util,
			bob_private_key, &alice_pub, 1, bob_shared_key);

	if (ecdh_status_bob == OPTIGA_CRYPT_SUCCESS) {
		printf("api ecdh bob \r\n");    // Changed from "alice" to "bob"
	} else {
		printf("api call failure\r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY)
		;
	if (op_status == OPTIGA_UTIL_SUCCESS) {
		printf("API OPERATION SUCCESSFUL bob ecdh\r\n"); // Changed from "alice" to "bob"
	} else {
		printf("API OPERATION FAILURE\r\n");
		CY_ASSERT(0);
	}

	op_status = OPTIGA_UTIL_BUSY;

	print_uint8_data(bob_shared_key, 32);

	//destroy
	api_status = optiga_crypt_destroy(crypt_util,OPTIGA_ECC_CURVE_NIST_P_256 );

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
