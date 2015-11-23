#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <user_interface.h>
#include <espconn.h>
#include <ip_addr.h>
int ets_uart_printf(const char *fmt, ...);
#include "driver/uart.h"

#include "lobaro-coap/coap.h"
#include "lobaro-coap/interface/esp8266/lobaro-coap_esp8266.h"

//internal ESP8266 Control Structures
esp_udp UdpCoAPConn;
struct espconn CoAP_conn = {.proto.udp = &UdpCoAPConn};

//"Main Loop" Speed
#define DELAY_LOOP 100 // milliseconds
LOCAL os_timer_t MainLoop_timer;

LOCAL void ICACHE_FLASH_ATTR mainLoopTimer_cb(void *arg)
{
	if(CoAP_ESP8266_States.TxSocketIdle)
		CoAP_doWork();
}

void ICACHE_FLASH_ATTR init_done(void) {

	static uint8_t CoAP_WorkMemory[4096]; //Working memory of CoAPs internal memory allocator
	CoAP_Init(CoAP_WorkMemory, 4096);
	CoAP_ESP8266_CreateInterfaceSocket(0, &CoAP_conn, 5683, CoAP_onNewPacketHandler, CoAP_ESP8266_SendDatagram);

	Create_Wifi_IPs_Resource(); 		//example of simple GET resource
	Create_About_Resource();			//example of large resource (blockwise transfers)
	Create_Wifi_Config_Resource(); 		//example of uri-query usage
	Create_RTC_Resource(); 				//example of observable resource
	Create_Led_Resource(); 				//example of observable resource triggered by itself + uri-query usage
	Create_Wifi_Scan_Resource(); 		//example of longer lasting "postponed" resource with two responses (1. empty ACK, 2. actual resource)

	ets_uart_printf("- CoAP init done! Used CoAP ram memory:\r\n"); //note: static ram footprint depends primary on resource count+uri lengths
	coap_mem_determinateStaticMem();
	coap_mem_stats();

	// Set up a "main-loop" timer
	os_timer_disarm(&MainLoop_timer);
	os_timer_setfn(&MainLoop_timer, (os_timer_func_t *)mainLoopTimer_cb, (void *)0);
	os_timer_arm(&MainLoop_timer, DELAY_LOOP, 1);
}

void user_init(void) {
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	ets_uart_printf("\r\n\r\nLobaro-CoAP on ESP8266 Demo!\r\nwww.lobaro.com\r\n\r\n");

	//Config ESP8266 network
	CoAP_ESP8266_ConfigDevice();
	system_init_done_cb(init_done);
}
