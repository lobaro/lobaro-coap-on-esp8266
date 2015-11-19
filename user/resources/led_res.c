#include <os_type.h>
#include <ets_sys.h>

#include "../lobaro-coap/coap.h"
#include "led_res.h"

CoAP_Res_t* pLed_Res = NULL;

bool LedState = true;

static void ICACHE_FLASH_ATTR led(bool onOff) {
	if(onOff) {gpio_output_set(0, 1 << 12, 1 << 12, 0);ets_uart_printf("LED on\r\n");}
	else  {gpio_output_set(1 << 12, 0, 1 << 12, 0);ets_uart_printf("LED off\r\n");}
}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR Res_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {

	if(pReq->Code == REQ_POST) {
		CoAP_option_t* pOpt;
		int8_t FoundNum=0;

		for(pOpt =pReq->pOptionsList ;pOpt != NULL; pOpt = pOpt->next) {
			FoundNum=CoAP_CompareUriQueryVal2Cstr(pOpt,"",3, "on","off", "tgl");
			if(FoundNum==0) continue;
			else if(FoundNum==1) LedState=true;
			else if(FoundNum==2) LedState=false;
			else if(FoundNum==3) LedState=!LedState;

			led(LedState);
			return HANDLER_OK;
		}
		pResp->Code=RESP_ERROR_BAD_REQUEST_4_00;
	} else {
		pResp->Code=RESP_ERROR_BAD_REQUEST_4_00;
	}
	return HANDLER_OK;
}



CoAP_Res_t* ICACHE_FLASH_ATTR Create_Led_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_POST};

	// Enable LED
	PIN_FUNC_SELECT(PERIPHS_IO_MUX_MTDI_U, FUNC_GPIO12);
	led(LedState);

	return (pLed_Res=CoAP_CreateResource("esp8266/led_gpio12", "LED at GPIO12",Options, Res_ReqHandler, NULL));
}

