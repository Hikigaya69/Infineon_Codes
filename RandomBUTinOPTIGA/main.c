
#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "optiga_util.h"
#include "optiga_crypt.h"
#include "cy_retarget_io.h"

void print_uint8_data(uint8_t* data, size_t len)
{
    char print[10];
    for (uint8_t i=0; i < len; i++)
    {
        if ((i % 16) == 0)
        {
            printf("\r\n");
        }
        sprintf(print,"%3u ", *(data+i));
        printf("%s", print);
    }
    printf("\r\n");
}




//optiga_lib_status_t final_response=1;
optiga_lib_status_t op_status = OPTIGA_UTIL_BUSY;
void handler_func(void *callback_ctx, optiga_lib_status_t event) {
 			op_status = event;
 		}





int main(void)
{
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
    if (result != CY_RSLT_SUCCESS)
    {
        CY_ASSERT(0);
    }

    /* Enable global interrupts */
    __enable_irq();
    result =cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,CY_RETARGET_IO_BAUDRATE);

     /* UART port init failed. Stop program execution */
   if (result != CY_RSLT_SUCCESS){	 CY_ASSERT(0); }

 	uint8_t data1[10]={0},data2[10]={0};
     /* \x1b[2J\x1b[;H - ANSI ESC sequence for clear screen */
     printf("\x1b[2J\x1b[;H");

 	printf("PSOC_OPTIGA_IOT_KIT template is ready to start.\r\n");


 	//optiga_lib_status_t op_status = OPTIGA_UTIL_BUSY;




 	optiga_util_t *op_util= optiga_util_create(0, handler_func, NULL);
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

 		while (op_status == OPTIGA_UTIL_BUSY);

 		if (op_status != OPTIGA_UTIL_SUCCESS) {
 			printf("Unsuccessful operation done \r\n");
 		} else {
 			printf("Successful operation done \r\n");
 		}


 	    op_status = OPTIGA_UTIL_BUSY;


    optiga_crypt_t *crypt_inst=optiga_crypt_create(0, handler_func, NULL);

    if (crypt_inst == NULL) {
    		printf("Unsuccessful in creating util \r\n");

    	} else {
    		printf("Successful in creating utillll \r\n");

    	}

    uint8 random_data[32]={0};


    optiga_lib_status_t rand_status =optiga_crypt_random(crypt_inst, OPTIGA_RNG_TYPE_TRNG , random_data, 32);

    if (rand_status != OPTIGA_LIB_SUCCESS) {
    		printf("API FAILED");

    	} else {
    		printf("API IS OK!!");
    	}

    	while (op_status == OPTIGA_UTIL_BUSY)
    		;

    	if (op_status != OPTIGA_UTIL_SUCCESS) {
    		printf("Unsuccessful WRITE \r\n");
    	} else {
    		printf("Successful WRITE done \r\n");
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

   print_uint8_data(random_data, 32);


}

/* [] END OF FILE */
