#include "cyhal.h"

#include "cybsp.h"

#include "cy_retarget_io.h"

#include "optiga_crypt.h"

#include "optiga_util.h"

#include <string.h>

#define MESSAGE "Hello world, This is a sample message"

#define KEY_ID OPTIGA_KEY_ID_E0F3

volatile optiga_lib_status_t optiga_lib_status;

void optiga_lib_callback(void *context, uint16_t event) {

	optiga_lib_status = event;

}

int main(void) {

	cy_rslt_t result;

	result = cybsp_init();

	if (result != CY_RSLT_SUCCESS) {

		printf("Board initialization failed!\n");

		return -1;

	}

	__enable_irq();

	cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
			CY_RETARGET_IO_BAUDRATE);

	printf("OPTIGA ECDSA Example\n");

	optiga_util_t *me_util = optiga_util_create(0, optiga_lib_callback, NULL);

	optiga_crypt_t *me_crypt = optiga_crypt_create(0, optiga_lib_callback,
	NULL);

	optiga_lib_status = OPTIGA_LIB_BUSY;

	optiga_util_open_application(me_util, 0);

	while (optiga_lib_status == OPTIGA_LIB_BUSY)
		;

	if (optiga_lib_status != OPTIGA_LIB_SUCCESS) {

		printf("Failed to open OPTIGA session\n");

		return -1;

	}

	uint8_t public_key[100];

	uint16_t public_key_length = sizeof(public_key);

	optiga_lib_status = OPTIGA_LIB_BUSY;

	optiga_crypt_ecc_generate_keypair(me_crypt,

	OPTIGA_ECC_CURVE_NIST_P_256,

	OPTIGA_KEY_USAGE_SIGN,

	FALSE,

	KEY_ID,

			public_key,

			&public_key_length);

	while (optiga_lib_status == OPTIGA_LIB_BUSY)
		;

	if (optiga_lib_status != OPTIGA_LIB_SUCCESS) {

		printf("Failed to generate ECC key pair\n");

		return -1;

	}

	printf("ECC Key Pair generated successfully\n");

	uint8_t hash[32];

	optiga_lib_status = OPTIGA_LIB_BUSY;

	optiga_crypt_hash(me_crypt,

	OPTIGA_HASH_TYPE_SHA_256,

	OPTIGA_CRYPT_HOST_DATA,

	(uint8_t*) MESSAGE,

	strlen(MESSAGE));  // ✅ Fixed: Removed extra argument

	while (optiga_lib_status == OPTIGA_LIB_BUSY)
		;

	if (optiga_lib_status != OPTIGA_LIB_SUCCESS) {

		printf("Failed to compute hash\n");

		return -1;

	}

	printf("SHA-256 hash computed successfully\n");

	uint8_t signature[80];

	uint16_t signature_length = sizeof(signature);

	optiga_lib_status = OPTIGA_LIB_BUSY;

	optiga_crypt_ecdsa_sign(me_crypt, hash, sizeof(hash), KEY_ID, signature,
			&signature_length);

	while (optiga_lib_status == OPTIGA_LIB_BUSY)
		;

	if (optiga_lib_status != OPTIGA_LIB_SUCCESS) {

		printf("Failed to sign message\n");

		return -1;

	}

	printf("Message signed successfully\n");

	optiga_lib_status = OPTIGA_LIB_BUSY;

	optiga_crypt_ecdsa_verify(me_crypt,

	hash,

	sizeof(hash),

	signature,

	signature_length,

	public_key,  // ✅ Fixed: Use public_key instead of KEY_ID

			public_key_length);

	while (optiga_lib_status == OPTIGA_LIB_BUSY)
		;

	if (optiga_lib_status != OPTIGA_LIB_SUCCESS) {

		printf("Signature verification failed\n");

		return -1;

	}

	printf("Signature verified successfully\n");

	optiga_util_close_application(me_util, 0);

	optiga_util_destroy(me_util);

	optiga_crypt_destroy(me_crypt);

	printf("OPTIGA session closed\n");

	return 0;

}
