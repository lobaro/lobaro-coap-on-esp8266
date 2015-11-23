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
#include <ets_sys.h>

#include "../lobaro-coap/coap.h"
#include "led_res.h"

CoAP_Res_t* pLed_Res = NULL;

bool LedState = true;

static void ICACHE_FLASH_ATTR led(bool onOff) {

	if(onOff) {gpio_output_set(0, 1 << 12, 1 << 12, 0);ets_uart_printf("LED on\r\n");}
	else  {gpio_output_set(1 << 12, 0, 1 << 12, 0);ets_uart_printf("LED off\r\n");}

	if(LedState != onOff) {
		CoAP_NotifyResourceObservers(pLed_Res); //Notify all observers of change
		LedState = onOff;
	}

}

static void ICACHE_FLASH_ATTR SetLedstatePayload(CoAP_Message_t* pReq, CoAP_Message_t* pResp){
	if(LedState) CoAP_SetPayload(pReq, pResp, "Led is on!", coap_strlen("Led is on!"), true);
	else CoAP_SetPayload(pReq, pResp, "Led is off!", coap_strlen("Led is off!"), true);
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR Res_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	if(pReq->Code == REQ_POST) {
		CoAP_option_t* pOpt;
		bool Found = false;

		for(pOpt =pReq->pOptionsList ; pOpt != NULL; pOpt = pOpt->next) {
			switch(CoAP_FindUriQueryVal(pOpt,"",3, "on","off", "tgl")) { //no prefix used -> use /led_gpio12?on or /led_gpio12?off
				case 0: break; //not found
				case 1: led(true); Found=true; break; //found "on"
				case 2: led(false); Found=true; break; //found "off"
				case 3: led(!LedState); Found=true; break; //found "tgl"
			}
			if(Found) {
				SetLedstatePayload(pReq, pResp);
				break;
			}
		}

		if(!Found){
			char info[] = {"usage: coap://.../led_gpio12?on (or \"off\", \"tgl\")"};
			CoAP_SetPayload(pReq, pResp, info, coap_strlen(info), true);
			pResp->Code=RESP_ERROR_BAD_REQUEST_4_00;
		}

	}else if(pReq->Code == REQ_GET){
		SetLedstatePayload(pReq, pResp);
	}

	return HANDLER_OK;
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR Res_NotifyHandler(CoAP_Observer_t* pObserver, CoAP_Message_t* pResp) {
	SetLedstatePayload(NULL, pResp);
	return HANDLER_OK;
}

CoAP_Res_t* ICACHE_FLASH_ATTR Create_Led_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_POST | RES_OPT_GET};

	// Enable LED on ESP8266
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	led(LedState);

	return (pLed_Res=CoAP_CreateResource("esp8266/led_gpio12", "LED at GPIO12",Options, Res_ReqHandler, Res_NotifyHandler));
}

