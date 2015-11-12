/*
	The hello world demo
*/

#include <ets_sys.h>
#include <osapi.h>
#include <os_type.h>
#include <gpio.h>
#include <user_interface.h>
#include <espconn.h>
#include <ip_addr.h>
#include "driver/uart.h"
#include "driver/drv_mems_bno55.h"
#include "driver/i2c_master.h"
#include "driver/TWI_master.h"
#include "driver/gpio16.h"

// CHANGE DEVICE PROPERTIES HERE!
// -------------------------------
#define PERIPH_NUM	(5)
#if PERIPH_NUM > 4
	#define BNO_NUM		(3)		// >4 -> BNO_NUM = 2
#else
	#define BNO_NUM		(2)
#endif
//#define UDPSERVERIP			"192.168.90.206"
//#define UDPSERVERIP				"192.168.90.208"
#define UDPSERVERIP			"192.168.178.25"
#define UDP_SERVER_PORT			(4325)
//#define UDP_BROADCAST_IP 		"192.168.255.255"
//#define UDP_BROADCAST_PORT		(4326)
//#define SSID  "CMN"
//#define PW    "2045938735586583"
#define SSID 	"MoCap_Router"
#define PW   	"3963988706849715"
// -------------------------------


#define DELAY_LOOP 25 /* milliseconds */
struct espconn Conn;
esp_udp UdpConn;

struct espconn broadcastConn;
esp_udp UdpBroadcastConn;

LOCAL os_timer_t hello_timer;
extern int ets_uart_printf(const char *fmt, ...);

LOCAL DualQuatJoystick_t QuatData;

struct sys {
	bool initDone;
	bool connectSend;
	bool sendStartDone;
	volatile bool LastSendDone;
	uint32_t lastSendTime;
};

struct sys system = {.initDone=false, .connectSend=false, .sendStartDone=false, .LastSendDone = false, .lastSendTime = 0};


static void ICACHE_FLASH_ATTR senddata()
{
	uint8 payload[512];
	static uint32_t i=0;
	os_memset(payload, 'x', 512);

	QuatData.periphNum = PERIPH_NUM;

	SLAVE_ADDRESS = 0x50;
	softi2c_bno055_read_quat(&QuatData.Data1);
	SLAVE_ADDRESS = 0x52;
	softi2c_bno055_read_quat(&QuatData.Data2);
#if BNO_NUM == 3
	SLAVE_ADDRESS = 0x4E;
	softi2c_bno055_read_quat(&QuatData.Data3);
#endif

	os_memcpy((void *)&payload[0], (void *)&QuatData.periphNum, 1);
	os_memcpy((void *)&payload[1], (void *)&QuatData.Data1.data[0], QUAT_BYTE_LENGTH);
	os_memcpy((void *)&payload[QUAT_BYTE_LENGTH+1], (void *)&QuatData.Data2.data[0], QUAT_BYTE_LENGTH);
#if BNO_NUM == 3
	os_memcpy((void *)&payload[2*QUAT_BYTE_LENGTH+1], (void *)&QuatData.Data3.data[0], QUAT_BYTE_LENGTH);
#endif

#if BNO_NUM == 3
	sint8 espsent_status = espconn_sent(&Conn, (uint8 *)&payload[0], 3*QUAT_BYTE_LENGTH+1);
#else
	sint8 espsent_status = espconn_sent(&Conn, (uint8 *)&payload[0], 2*QUAT_BYTE_LENGTH+1);
#endif

//	ets_uart_printf("Send Status: %d\r\n", espsent_status);
}

LOCAL void ICACHE_FLASH_ATTR hello_cb(void *arg)
{
	static uint32_t i = 0;
	static uint8_t conStatus = 0;

	uint8_t oldConStatus = conStatus;
	conStatus = wifi_station_get_connect_status();

	if (!system.connectSend && system.initDone) {

		if(wifi_station_connect()){
			ets_uart_printf("connect OK\r\n");
		} else {
			ets_uart_printf("connect Fail\r\n");
		}
		system.connectSend = true;
	}

//	ets_uart_printf("ConStati: %d - %d sizeof=%d\r\n", oldConStatus, conStatus, sizeof(DualQuatJoystick_t));
	if (conStatus == 5 && oldConStatus != 5) { //have ip
		ets_uart_printf("got ip!\r\n");
		system.LastSendDone = true;
	}

	if (system.LastSendDone) {
		//ets_uart_printf("sending next packet\r\n");
		system.LastSendDone = false;
		senddata();
	}

}

void user_rf_pre_init(void)
{

}


void ICACHE_FLASH_ATTR user_set_station_config(void)
{
	char ssid[32] = SSID;
	char password[64] = PW;

	struct station_config stationConf;
	stationConf.bssid_set = 0; //need not check MAC address of AP
	os_memcpy(&stationConf.ssid, ssid, 32);
	os_memcpy(&stationConf.password, password, 64);
	if(wifi_station_set_config(&stationConf)){
		ets_uart_printf("setconfig OK\r\n");
	} else {
		ets_uart_printf("setconfig Fail\r\n");
	}

	system.initDone = true;
}

static void ICACHE_FLASH_ATTR udpclient_sent_cb(void *arg) {
	system.LastSendDone = true;
}

//static void ICACHE_FLASH_ATTR udp_broadcast_recv_cb(void *arg, char *pdata, unsigned short len) {
//	struct espconn *endPoint = (struct espconn *)arg;
//	uint8_t *ip = endPoint->proto.udp->remote_ip;
//	char msg[len];
//	os_sprintf(msg, "%s", pdata);
//	ets_uart_printf("BROADCAST REC=%s, remote ip=%d.%d.%d.%d\r\n", msg, ip[0], ip[1], ip[2], ip[3]);
//}


void user_init(void)
{
	// Change system clk freq to 80MHz
	system_update_cpu_freq(SYS_CPU_80MHZ);
	os_delay_us(1000);

	// Configure the UART
	uart_init(BIT_RATE_115200, BIT_RATE_115200);
	os_delay_us(1000);

	// Reset onboard BNO055
	gpio16_output_conf();
	gpio16_output_set(0);

	// Broadcast receive udp socket
//	broadcastConn.type = ESPCONN_UDP;
//	broadcastConn.state = ESPCONN_NONE;
//	broadcastConn.proto.udp = &UdpBroadcastConn;
//	char broadcastip[16];
//	os_sprintf(broadcastip, "%s", UDP_BROADCAST_IP);
//	uint32_t ip = ipaddr_addr(broadcastip);
//	broadcastConn.proto.udp->local_port = UDP_BROADCAST_PORT;
//	broadcastConn.proto.udp->remote_port = 0;
//	os_memcpy(broadcastConn.proto.udp->remote_ip, &ip, 4);
//	espconn_regist_recvcb(&broadcastConn, udp_broadcast_recv_cb); // register a udp packet receiving callback
//
//	sint8 statusCreate = espconn_create(&broadcastConn);
//	ets_uart_printf("ESP-BroadcastConnCreate Status %d\r\n", statusCreate);

	// Send udp socket
	Conn.type = ESPCONN_UDP;
	Conn.state = ESPCONN_NONE;
	Conn.proto.udp = &UdpConn;
	char serverip[15];
	os_sprintf(serverip, "%s", UDPSERVERIP);
	uint32_t ip = ipaddr_addr(serverip);
	Conn.proto.udp->local_port = espconn_port();
	Conn.proto.udp->remote_port = UDP_SERVER_PORT;
	os_memcpy(Conn.proto.udp->remote_ip, &ip, 4);

	sint8 statusCreate = espconn_create(&Conn);
	ets_uart_printf("ESP-ConnCreate Status %d\r\n", statusCreate);

	// Print system freq
	ets_uart_printf("System clk freq: %d\r\n", system_get_cpu_freq());

    // i2c
	// i2c_master_gpio_init();
	// i2c_master_stop();
	twi_init();

	// Enable LED
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	gpio_output_set(0, 1 << 12, 1 << 12, 0);

	// Enable onboard BNO055
	gpio16_output_set(1);

	espconn_regist_sentcb(&Conn, udpclient_sent_cb);

	// Sleep 500ms
	os_delay_us(500000);

	// Initialize bno's
	SLAVE_ADDRESS = 0x50;	// addr = 0x28 << 1
	softi2c_bno055_init_quat_mode();
	SLAVE_ADDRESS = 0x52;	// addr = 0x29 << 1
	softi2c_bno055_init_quat_mode();
#if BNO_NUM == 3
	SLAVE_ADDRESS = 0x4E;	// addr = 0x27 << 1
	softi2c_bno055_init_quat_mode();
#endif

	//Station Mode
	wifi_set_opmode(STATION_MODE);
	user_set_station_config();
	// Set up a timer to send the message
	// os_timer_disarm(ETSTimer *ptimer)
	os_timer_disarm(&hello_timer);
	// os_timer_setfn(ETSTimer *ptimer, ETSTimerFunc *pfunction, void *parg)
	os_timer_setfn(&hello_timer, (os_timer_func_t *)hello_cb, (void *)0);
	// void os_timer_arm(ETSTimer *ptimer,uint32_t milliseconds, bool repeat_flag)
	os_timer_arm(&hello_timer, DELAY_LOOP, 1);
}
