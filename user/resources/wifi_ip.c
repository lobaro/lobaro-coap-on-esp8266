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
#include <user_interface.h>

#include "../lobaro-coap/coap.h"

CoAP_Res_t* pWifi_ip_Res = NULL;

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR RequestHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {

	struct ip_info ipconfig;
	char payloadTemp[250];
	char* pStrWorking = payloadTemp;

	if(wifi_get_ip_info(STATION_IF, &ipconfig))
		pStrWorking+= coap_sprintf((char*)pStrWorking, "Station: %d.%d.%d.%d\r\n",IP2STR(&ipconfig.ip));


	if(wifi_get_ip_info(SOFTAP_IF, &ipconfig))
		pStrWorking+= coap_sprintf((char*)pStrWorking, "SoftAP: %d.%d.%d.%d\r\n",IP2STR(&ipconfig.ip));

	if(pStrWorking!=payloadTemp) {
		CoAP_SetPayload(pReq, pResp, (uint8_t*)payloadTemp, coap_strlen(payloadTemp), true);
	}else pResp->Code=RESP_INTERNAL_SERVER_ERROR_5_00;

	return HANDLER_OK;
}


CoAP_Res_t* ICACHE_FLASH_ATTR Create_Wifi_IPs_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET };
	return (pWifi_ip_Res=CoAP_CreateResource("wifi/ipconfig", "ESP8266 IP configuration",Options, RequestHandler, NULL));
}
