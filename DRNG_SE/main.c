#if defined (CY_USING_HAL)
#include "cyhal.h"
#endif
#include "cybsp.h"
#include "cy_retarget_io.h"
#include "optiga_util.h"
#include <stdio.h>


optiga_lib_status_t op_status = OPTIGA_UTIL_BUSY;
void handler_func(void *callback_ctx, optiga_lib_status_t event) {
		op_status = event;
	}

int main(void) {
	cy_rslt_t result;

#if defined (CY_DEVICE_SECURE) && defined (CY_USING_HAL)
    cyhal_wdt_t wdt_obj;
    result = cyhal_wdt_init(&wdt_obj, cyhal_wdt_get_max_timeout_ms());
    CY_ASSERT(CY_RSLT_SUCCESS == result);
    cyhal_wdt_free(&wdt_obj);
#endif

	result = cybsp_init();
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	__enable_irq();

	result = cy_retarget_io_init(CYBSP_DEBUG_UART_TX, CYBSP_DEBUG_UART_RX,
			CY_RETARGET_IO_BAUDRATE);
	if (result != CY_RSLT_SUCCESS) {
		CY_ASSERT(0);
	}

	printf("\x1b[2J\x1b[;H");


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

	//WRITE PROCESS
	op_status = OPTIGA_UTIL_BUSY;
	uint16_t oid = 0xE0E1;  // General-purpose data object
	uint8_t write_data[] = "YOKOSO WATASHI NO SOUL SOCIETY YE!!";
	uint16_t write_length = sizeof(write_data);

	optiga_lib_status_t write_status = optiga_util_write_data(op_util, oid,OPTIGA_UTIL_WRITE_ONLY, 0, write_data, write_length);

	if (write_status != OPTIGA_LIB_SUCCESS) {
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

	//CLOSE PROCESSS

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

	//OPEN AGAIN

	op_status = OPTIGA_UTIL_BUSY;
	optiga_lib_status_t opt_open2 = optiga_util_open_application(op_util, 0);
	if (opt_open2 != OPTIGA_LIB_SUCCESS) { //if incase of error turn it back to OPTIGA_UTIL_SUCCESS
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

//READ PROCESS
	uint8_t read_data[30] = { 0 };
	uint8_t read_length = sizeof(read_data);
	op_status = OPTIGA_UTIL_BUSY;
	optiga_lib_status_t read_status = optiga_util_read_data(op_util, oid, 0,read_data, read_length);

	if (read_status != OPTIGA_LIB_SUCCESS) { //if incase of error turn it back to OPTIGA_UTIL_SUCCESS
		printf("READ of File API failes \r\n");
	} else {
		printf("REDA of file SUCCESS \r\n");
	}

	while (op_status == OPTIGA_UTIL_BUSY)
		;

	if (op_status != OPTIGA_UTIL_SUCCESS) {
		printf("Unsuccessful operation done \r\n");
	} else {
		printf("Successful operation done \r\n");
	}


	  printf("Data read successfully: %s\r\n", read_data);

	  optiga_util_destroy(op_util);

	for (;;) {
	}
}
