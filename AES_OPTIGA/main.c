/*******************************************************************************
 * File Name:   main.c
 *
 * Description: This is the source code for the Empty Application Example
 *              for ModusToolbox.
 *
 * Related Document: See README.md
 *
 *
 *******************************************************************************
 * Copyright 2021-2024, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 *******************************************************************************/

/*******************************************************************************
 * Header Files
 *******************************************************************************/
#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ecp.h"
#include "optiga_util.h"
#include "optiga_crypt.h"

/******************************************************************************
 * Macros
 *******************************************************************************/

/*******************************************************************************
 * Global Variables
 *******************************************************************************/

/*******************************************************************************
 * Function Prototypes
 *******************************************************************************/

/*******************************************************************************
 * Function Definitions
 *******************************************************************************/
/*******************************************************************************
 * Function Name: print_data()
 ********************************************************************************
 * Summary:
 * 	Function used to display the data in hexadecimal format
 *
 * Parameters:
 *  uint8_t* data - Pointer to location of data to be printed
 *  size_t  len  - length of data to be printed
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void print_uint8_data(uint8_t *data, size_t len) {
	char print[10];
	for (uint8_t i = 0; i < len; i++) {
		if ((i % 16) == 0) {
			printf("\r\n");
		}
		sprintf(print, "0x%02X ", *(data + i));
		printf("%s", print);
	}
	printf("\r\n");
}

/*******************************************************************************
 * Function Name: print_mpi_data()
 ********************************************************************************
 * Summary:
 *  Prints the contents of an mbedtls_mpi structure in hexadecimal format.
 *
 * Parameters:
 *  mbedtls_mpi* data: Pointer to the mbedtls_mpi structure to be printed.
 *  size_t  len  - length of data to be printed
 *
 * Return:
 *  void
 *
 *******************************************************************************/
void print_mpi_data(mbedtls_mpi *data) {
	size_t len = mbedtls_mpi_size(data);
	unsigned char buffer[100] = { 0 };
	mbedtls_mpi_write_binary(data, buffer, len);
	print_uint8_data(buffer, len);
}

/*******************************************************************************
 * Function Name: print_ecp_point_data()
 ********************************************************************************
 * Summary:
 *  Prints the contents of an mbedtls_ecp_point structure in
 *  uncompressed binary format.
 *
 * Parameters:
 *  mbedtls_ecp_point* data: Pointer to the mbedtls_ecp_point structure to be printed.
 *  mbedtls_ecp_group* grp: Pointer to the mbedtls_ecp_group structure associated
 *  with the point.
 *
 * Return:
 *  void
 *
 ******************************************************************************/
void print_ecp_point_data(mbedtls_ecp_point *data, mbedtls_ecp_group *grp) {
	unsigned char buffer[100] = { 0 };
	size_t buflen = 0; //ECP_KEY_LENGTH

	mbedtls_ecp_point_write_binary(grp, data, MBEDTLS_ECP_PF_UNCOMPRESSED,
			&buflen, buffer, sizeof(buffer));
	print_uint8_data(buffer, buflen);
}

/*******************************************************************************
 * Function Name: main
 *********************************************************************************
 * Summary:
 * This is the main function for CPU. It...
 *    1.
 *    2.
 *
 * Parameters:
 *  void
 *
 * Return:
 *  int
 *
 *******************************************************************************/

optiga_lib_status_t op_status = OPTIGA_UTIL_BUSY;
void handler_func(void *callback_ctx, optiga_lib_status_t event) {
	op_status = event;
}
int main(void) {
	cy_rslt_t result;

	/* Initialize the device and board peripherals */
	result = cybsp_init();

	/* Board init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	/* Enable global interrupts */
	__enable_irq();

	/* Initialize retarget-io to use the debug UART port */
	result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
			CY_RETARGET_IO_BAUDRATE);

	/* UART port init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	optiga_util_t *op_util;

	op_util = optiga_util_create(0, handler_func, NULL);
	if (op_util == NULL) {
		printf("Unsuccessful in creating util \r\n");

	} else {
		printf("Successful in creating util \r\n");

	}

	optiga_lib_status_t opt_open = optiga_util_open_application(op_util, 0);
	if (opt_open != OPTIGA_LIB_SUCCESS) { //if incase of error turn it back to OPTIGA_UTIL_SUCCESS
		printf("Unsuccessful in opening \r\n");
	} else {
		printf("Successful in opening \r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY)
		;

	if (op_status != OPTIGA_UTIL_SUCCESS) {
		printf("Unsuccessful operation done \r\n");
	} else {
		printf("Successful operation done \r\n");
	}

	optiga_crypt_t *crypt_inst = optiga_crypt_create(0, handler_func, NULL);

	if (crypt_inst == NULL) {
		printf("Unsuccessful in creating util \r\n");

	} else {
		printf("Successful in creating utillll \r\n");

	}

	op_status = OPTIGA_UTIL_BUSY;
	uint8_t keyBuffer[128];

	optiga_lib_status_t keyg_status = optiga_crypt_symmetric_generate_key(crypt_inst, OPTIGA_SYMMETRIC_AES_128, OPTIGA_KEY_USAGE_ENCRYPTION,0, );

	if (keyg_status != OPTIGA_LIB_SUCCESS) { //if incase of error turn it back to OPTIGA_UTIL_SUCCESS
		printf("Unsuccessful in opening \r\n");
	} else {
		printf("Successful in opening \r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY);

	if (op_status != OPTIGA_UTIL_SUCCESS) {
		printf("Unsuccessful operation done \r\n");
	} else {
		printf("Successful operation done \r\n");
	}



	op_status = OPTIGA_UTIL_BUSY;
	uint8_t plaintext[16] = {
	         0x41, 0x45, 0x53, 0x20, 0x45, 0x43, 0x42, 0x20,
	         0x54, 0x45, 0x53, 0x54, 0x20, 0x44, 0x41, 0x54
	     };
	uint32_t plaindatalen=sizeof(plaintext);
	uint8_t encrypted[16];
	uint32_t enctyptedlen=sizeof(encrypted);

	optiga_lib_status_t ecb_stat=optiga_crypt_symmetric_encrypt_ecb	(crypt_inst,OPTIGA_KEY_ID_E0F2 ,plaintext,plaindatalen,encrypted,enctyptedlen );

	if (ecb_stat != OPTIGA_LIB_SUCCESS) { //if incase of error turn it back to OPTIGA_UTIL_SUCCESS
			printf("Unsuccessful in opening \r\n");
		} else {
			printf("Successful in opening \r\n");
		}

		while (op_status == OPTIGA_UTIL_BUSY);

		if (op_status != OPTIGA_UTIL_SUCCESS) {
			printf("Unsuccessful operation done \r\n");
		} else {
			printf("Successful operation done \r\n");
		}

	optiga_crypt_destroy(crypt_inst);
	op_status = OPTIGA_UTIL_BUSY;
	optiga_lib_status_t close_status = optiga_util_close_application(op_util,0);

	if (close_status != OPTIGA_LIB_SUCCESS) {
		printf("Close of Application failed");
	} else {
		printf("Close of APPlication is done");
	}
	while (op_status == OPTIGA_UTIL_BUSY);

	if (op_status != OPTIGA_UTIL_SUCCESS) {
		printf("Unsuccessful CLOSE \r\n");
	} else {
		printf("Successful CLOSE done \r\n");
	}

	optiga_util_destroy(op_util);
	/* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
	printf("\x1b[2J\x1b[;H");

	printf("PSOC_OPTIGA_IOT_KIT template is ready to start.\r\n");

	for (;;) {
	}
}

/* [] END OF FILE */
