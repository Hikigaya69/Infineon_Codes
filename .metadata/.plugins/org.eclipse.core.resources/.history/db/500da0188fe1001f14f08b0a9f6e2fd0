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
#include "aes.h"

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
int main(void)
{
    cy_rslt_t result;
    mbedtls_aes_context aes_context;
   // mbedtls_aes_context okokoko;
    uint8_t Key_Buffer[]={0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08,0x09,0x0a,0x0b,0x0c,0x0d,0x0e,0x0f};
    uint8_t Inp_Buffer[]={1,2,3,4,5,6,7,8,9,10,1,2,3,4,5,6};
    uint8_t Out_Buffer[16];
    uint8_t Recv_Buffer[16];



#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;

    /* Clear watchdog timer so that it doesn't trigger a reset */
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

    /* Initialize the device and board peripherals */
    result = cybsp_init();

    result = cy_retarget_io_init_fc(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
                CYBSP_DEBUG_UART_CTS,CYBSP_DEBUG_UART_RTS,CY_RETARGET_IO_BAUDRATE);


    /* Board init failed. Stop program execution */
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();


  //Encryption Process
    mbedtls_aes_init(&aes_context);
    //mbedtls_aes_init(&okokoko);

    mbedtls_aes_setkey_enc(&aes_context,Key_Buffer,128);
  //  mbedtls_aes_setkey_en( &okokok, Key_Buffer,128);
    mbedtls_aes_crypt_ecb(&aes_context,MBEDTLS_AES_ENCRYPT,Inp_Buffer,Out_Buffer);
//mbedtls_aes_crypt_ecb(&okokoko,MBEDTLS_AES_ENCRYPT ,Inp_Buffer, unsigned char output[16])

    printf("\r\n AES Encryption Data values\r\n");

    for(int i=0;i<sizeof(Out_Buffer);i++){
    	printf(" %x ",Out_Buffer[i] );
    }

    printf("\r\n");


    //Decryption process
    mbedtls_aes_setkey_dec(&aes_context,Key_Buffer,128);
    mbedtls_aes_crypt_ecb(&aes_context,MBEDTLS_AES_DECRYPT,Out_Buffer,Recv_Buffer);

    //memset(Recv_Buffer, 0, sizeof(Recv_Buffer));

    printf("\r\n AES Decryption Data values\r\n");

    for(int i=0;i<sizeof(Recv_Buffer);i++){
    	printf(" %x ", Recv_Buffer[i] );
    }
    printf("\r\n");



    mbedtls_aes_free(&aes_context);

    for (;;)
       {
       }






}

/* [] END OF FILE */
