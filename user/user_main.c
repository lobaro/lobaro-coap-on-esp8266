#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <user_interface.h>
#include <espconn.h>
#include <ip_addr.h>
#include "driver/uart.h"

int ets_uart_printf(const char *fmt, ...);

#include "lobaro-coap/coap.h"
#include "lobaro-coap/interface/esp8266/lobaro-coap_esp8266.h"

#define DELAY_LOOP 100 // milliseconds
LOCAL os_timer_t MainLoop_timer;

LOCAL void ICACHE_FLASH_ATTR timer_cb(void *arg)
{
    static uint8_t lastConStatus = STATION_IDLE;
    uint8_t conStatus = wifi_station_get_connect_status();

	if (conStatus == STATION_GOT_IP && conStatus!=lastConStatus) { //just got ip
		ets_uart_printf("\r\n- Got IP from external AP! -> ");
		struct ip_info ipconfig;
		if(wifi_get_ip_info(STATION_IF, &ipconfig)){
			  ets_uart_printf("%d.%d.%d.%d",IP2STR(&ipconfig.ip));
		}
		else ets_uart_printf("ERROR!!\r\n");
	}else if(conStatus == STATION_CONNECTING && conStatus!=lastConStatus){
		ets_uart_printf("...connecting to remote wifi access point...\r\n");
	}else if(conStatus == STATION_WRONG_PASSWORD && conStatus!=lastConStatus){
		ets_uart_printf("(!!!) Wrong Password!\r\n");
		wifi_station_disconnect();
	}else if(conStatus == STATION_NO_AP_FOUND && conStatus!=lastConStatus){
		ets_uart_printf("(!!!) No AP Found!\r\n");
		wifi_station_disconnect();
	}else if(conStatus == STATION_CONNECT_FAIL && conStatus!=lastConStatus){
		ets_uart_printf("(!!!) Connect Fail\r\n");
		wifi_station_disconnect();
	}

	lastConStatus = conStatus;

	if( CoAPSocket_TxOngoing == false) { //only one esp transmission at a time!
		CoAP_doWork();
	}

}

//internal ESP8266 Control Structures
esp_udp UdpCoAPConn;
struct espconn CoAP_conn = {.proto.udp = &UdpCoAPConn};

void init_done(void){

#if USE_SOFT_AP ==1
	struct ip_info ipconfig;
	if(wifi_get_ip_info(SOFTAP_IF, &ipconfig)){
				  ets_uart_printf("- SoftAP IP -> %d.%d.%d.%d\r\n",IP2STR(&ipconfig.ip));
	}
#endif

	//init coap
	CoAP_Init();
	Create_RTC_Resource();
	Create_About_Resource();
	Create_Wifi_Resource();
	Create_Wifi_Scan_Resource();
	Create_Led_Resource();
	CoAP_ESP8266_CreateInterfaceSocket(0, &CoAP_conn, 5683, CoAP_onNewPacketHandler, CoAP_ESP8266_SendDatagram);
	ets_uart_printf("- CoAP init done! Used CoAP ram memory (poolsize = %d bytes):\r\n", COAP_RAM_TOTAL_BYTES); //note: static ram footprint depends primary on resource count+uri lengths
	coap_mem_defineStaticMem();
	coap_mem_stats();


}

void user_init(void)
{
	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_delay_us(1000);
	ets_uart_printf("\r\n\r\nLobaro-CoAP on ESP8266 Demo!\r\nwww.lobaro.com\r\n\r\n");

	//Config Device
	CoAP_ESP8266_ConfigDevice();
	system_init_done_cb(init_done);

	// Set up a "main-loop" timer
	os_timer_disarm(&MainLoop_timer);
	os_timer_setfn(&MainLoop_timer, (os_timer_func_t *)timer_cb, (void *)0);
	os_timer_arm(&MainLoop_timer, DELAY_LOOP, 1);
}
