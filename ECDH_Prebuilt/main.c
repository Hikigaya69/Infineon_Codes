/***************************
* File Name:   main.c
*
* Description: This is the source code for the Empty Application Example
*              for ModusToolbox.
*
* Related Document: See README.md
*
*
***************************
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
***************************/

/***************************
* Header Files
***************************/
#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ecp.h"
#include "ecdh.h"
#include "ctr_drbg.h"
#include "entropy.h"
/**************************
* Macros
***************************/


/***************************
* Global Variables
***************************/


/***************************
* Function Prototypes
***************************/


/***************************
* Function Definitions
***************************/
/***************************
* Function Name: print_data()
****************************
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
***************************/
void print_uint8_data(uint8_t* data, size_t len)
{
    for (uint8_t i=0; i < len; i++)
    {
        if ((i % 16) == 0)
        {
            printf("\r\n");
        }
        printf("0x%02X ", *(data+i));
    }
    printf("\r\n");
}

/***************************
 * Function Name: print_mpi_data()
 ****************************
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
 ***************************/
void print_mpi_data(mbedtls_mpi* data)
{
	size_t len = mbedtls_mpi_size(data);
	unsigned char buffer[100] = {0};
    mbedtls_mpi_write_binary(data, buffer, len);
    print_uint8_data(buffer, len);
}

/***************************
 * Function Name: print_ecp_point_data()
 ****************************
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
 **************************/
void print_ecp_point_data(mbedtls_ecp_point* data, mbedtls_ecp_group *grp)
{
	unsigned char buffer[100] = {0};
	size_t buflen = 0; //ECP_KEY_LENGTH

    mbedtls_ecp_point_write_binary(grp, data, MBEDTLS_ECP_PF_UNCOMPRESSED,
    		&buflen, buffer, sizeof(buffer));
    print_uint8_data(buffer, buflen);
}

/***************************
* Function Name: main
***************************
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
***************************/
int main(void)
{
    cy_rslt_t result;

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();

    /* Initialize retarget-io to use the debug UART port */
    result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
    		CY_RETARGET_IO_BAUDRATE);

    /* UART port init failed. Stop program execution */
	if (result != CY_RSLT_SUCCESS)
	{
	   CY_ASSERT(0);
	}

    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

	printf("PSOC_PROTOTYPING_KIT template is ready to start.\r\n");
	mbedtls_ctr_drbg_context random_context;
			mbedtls_entropy_context entropy_context;
			mbedtls_ctr_drbg_init(&random_context);
			mbedtls_entropy_init(&entropy_context);
			mbedtls_ctr_drbg_seed(&random_context,mbedtls_entropy_func,&entropy_context,"hibee",5);

	mbedtls_ecdh_context Alice_context;
	mbedtls_ecdh_context Bob_context;

	mbedtls_ecdh_init(&Alice_context);
	mbedtls_ecdh_init(&Bob_context);

	mbedtls_ecdh_setup(&Alice_context,MBEDTLS_ECP_DP_SECP256R1); //curve setup
	mbedtls_ecdh_setup(&Bob_context,MBEDTLS_ECP_DP_SECP256R1);

	mbedtls_ecdh_gen_public(&Alice_context.private_grp,&Alice_context.private_d,&Alice_context.private_Q,&mbedtls_ctr_drbg_random,&random_context);  //generate key pair
	mbedtls_ecdh_gen_public(&Bob_context.private_grp,&Bob_context.private_d,&Bob_context.private_Q,&mbedtls_ctr_drbg_random,&random_context);

	mbedtls_ecdh_compute_shared(&Alice_context.private_grp,&Alice_context.private_z,&Bob_context.private_Q,&Alice_context.private_d,&mbedtls_ctr_drbg_random,&random_context);
	mbedtls_ecdh_compute_shared(&Bob_context.private_grp,&Bob_context.private_z,&Alice_context.private_Q,&Bob_context.private_d,&mbedtls_ctr_drbg_random,&random_context);

	int flag=mbedtls_mpi_cmp_mpi(&Alice_context.private_z,&Bob_context.private_z);
	if(flag==0)
	{
		printf("\r\n both secret key are equal \r\n");
	}
	else
	{
		printf("\r\n not equal\r\n");
	}
	print_mpi_data(&Alice_context.private_z);
    for (;;)
    {
    }
}

/* [] END OF FILE */
