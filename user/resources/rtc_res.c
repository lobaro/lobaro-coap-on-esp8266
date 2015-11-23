/*******************************************************************************
 * Copyright (c)  2015  Dipl.-Ing. Tobias Rohde, http://www.lobaro.com
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *******************************************************************************/
#include <os_type.h>
#include <osapi.h>

#include "../lobaro-coap/coap.h"
#include "rtc_res.h"

CoAP_Res_t* pRTC_Res = NULL;

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR RequestHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	char myString[20];
	coap_sprintf(myString,"%d [s]", hal_rtc_1Hz_Cnt());
	CoAP_SetPayload(pReq, pResp, myString, coap_strlen(myString), true);

	return HANDLER_OK;
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR NotifyHandler(CoAP_Observer_t* pObserver, CoAP_Message_t* pResp) {
	char myString[20];
	coap_sprintf(myString,"%d [s]", hal_rtc_1Hz_Cnt());
	CoAP_SetPayload(NULL, pResp, myString, coap_strlen(myString), true);

	return HANDLER_OK;
}

//Update Observers every second
#define DELAY_LOOP 1000 // milliseconds
LOCAL os_timer_t Notify_timer;

LOCAL void ICACHE_FLASH_ATTR notify_cb(void *arg) {
	CoAP_NotifyResourceObservers(pRTC_Res);
}

CoAP_Res_t* ICACHE_FLASH_ATTR Create_RTC_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET};

	os_timer_disarm(&Notify_timer);
	os_timer_setfn(&Notify_timer, (os_timer_func_t *)notify_cb, (void *)0);
	os_timer_arm(&Notify_timer, DELAY_LOOP, 1);

	return (pRTC_Res=CoAP_CreateResource("/rtc", "esp8266 RTC", Options, RequestHandler, NotifyHandler));
}
