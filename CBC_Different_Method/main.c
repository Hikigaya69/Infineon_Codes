/***************************
* Header Files
***************************/
#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "ecp.h"
#include "ctr_drbg.h"
#include "entropy.h"
#include "aes.h"

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
    char print[10];
    for (uint8_t i=0; i < len; i++)
    {
        if ((i % 16) == 0)
        {
            printf("\r\n");
        }
        sprintf(print,"0x%02X ", *(data+i));
        printf("%s", print);
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
 * **************************/


void xor(const uint8_t *block, const uint8_t *iv, uint8_t *result) {
    for (size_t i = 0; i < 16; i++) {
        result[i] = block[i] ^ iv[i];
    }
}
//
//
//
//
//
//

//
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
  size_t i;
    /* Initialize the device and board peripherals */
   result = cybsp_init();

    /* Board init failed. Stop program execution */
  if (result != CY_RSLT_SUCCESS){	 CY_ASSERT(0); }

    /* Enable global interrupts */
    __enable_irq();
    /* Initialize retarget-io to use the debug UART port */
   result =cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,CY_RETARGET_IO_BAUDRATE);

    /* UART port init failed. Stop program execution */
  if (result != CY_RSLT_SUCCESS){	 CY_ASSERT(0); }

	uint8_t data[16]={0};
	uint8_t	input1[128]={1,2,3,4,1}; //before xor
	uint8_t	input2[128]={0};	//after xor

	uint8_t output[128]={0};	//after encrypt
	uint8_t decrypted[128]={0};	//after decrypt
	uint8_t decrypted2[128]={0};	//after xor at end


	uint8_t iv[16]={1,2,3};
	uint8_t iv2[16]={0};
	memcpy(iv2,iv,16);
	size_t j;



    /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
    printf("\x1b[2J\x1b[;H");

	printf("PSOC_OPTIGA_IOT_KIT template is ready to start.\r\n");

	 mbedtls_ctr_drbg_context memory;
	 mbedtls_entropy_context entropy;
	 mbedtls_aes_context aes;


	 mbedtls_ctr_drbg_init(&memory);
	 mbedtls_entropy_init(&entropy);
	 mbedtls_aes_init(&aes);



	 mbedtls_ctr_drbg_seed(&memory, mbedtls_entropy_func,&entropy,(const unsigned char *)"pancham", 7);
	 mbedtls_ctr_drbg_random(&memory, data, sizeof(data));


	 mbedtls_aes_setkey_enc(&aes, data, sizeof(data)*8);

	 for(j=0;j<(sizeof(input1)/16);j=j+16){
	xor(&input1[j],iv,&input2[j]);
	 mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, &input2[j], &output[j]);
	 }
	 print_uint8_data(output,128 );	 //	 mbedtls_ctr_drbg_free(&memory);


	 mbedtls_aes_setkey_dec(&aes, data, sizeof(data)*8);

	 for(j=0;j<(sizeof(input1))/16;j=j+16){
	 mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, &output[j], &decrypted[0]);
	 xor(&decrypted[j], iv2, &decrypted2[j]);
	 }

	 print_uint8_data(decrypted2, 128);	 //	 mbedtls_ctr_drbg_free(&memory);

//	 mbedtls_entropy_free(&entropy);
//	 sprintf(print,"0x%02X ", *(data));
//	 for (i = 0; i < sizeof(data)/sizeof(data[0]); i++) {
//		 printf("0x%02X, ", data[i]);
//	 }


//
//		     printf("****** "
//		            "HAL: Hello World! Example "
//		            "****** \r\n\n");
//
//		     printf("Hello World!!!\r\n\n");

/* [] END OF FILE */



	 mbedtls_ctr_drbg_free(&memory);
	 		 mbedtls_entropy_free(&entropy);
	 		 mbedtls_aes_free(&aes);
}
