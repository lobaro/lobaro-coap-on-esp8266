#include <os_type.h>
#include <osapi.h>

#include "../lobaro-coap/coap.h"
#include "rtc_res.h"

CoAP_Res_t* pRTC_Res = NULL;

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR RTC_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	static char myString[100];

	coap_sprintf(myString,"%d [s]", hal_rtc_1Hz_Cnt());
	CoAP_SetPayloadBlockwise(pReq, pResp, myString, coap_strlen(myString), false);

	return HANDLER_OK;
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR RTC_NotifyHandler(CoAP_Observer_t* pObserver, CoAP_Message_t* pResp) {
	static int i=0;

	//define payload
	coap_sprintf((char*)(pResp->Payload),"%d [s]", hal_rtc_1Hz_Cnt());
	pResp->PayloadLength = (uint16_t)coap_strlen(pResp->Payload);

	//send every 10th notification as CON to support "lazy" cancelation
	if(i%10==0) pResp->Type = CON;
	else pResp->Type = NON;
	i++;

	return HANDLER_OK;
}

//Update Observers every second
#define DELAY_LOOP 1000 // milliseconds
LOCAL os_timer_t Notify_timer;

LOCAL void ICACHE_FLASH_ATTR notify_cb(void *arg) {
	CoAP_ResUpdated(pRTC_Res);
}


CoAP_Res_t* ICACHE_FLASH_ATTR Create_RTC_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET};

	os_timer_disarm(&Notify_timer);
	os_timer_setfn(&Notify_timer, (os_timer_func_t *)notify_cb, (void *)0);
	os_timer_arm(&Notify_timer, DELAY_LOOP, 1);

	return (pRTC_Res=CoAP_CreateResource("/rtc", "esp8266 RTC", Options, RTC_ReqHandler, RTC_NotifyHandler));
}
