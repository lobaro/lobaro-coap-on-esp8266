#include <os_type.h>
#include <osapi.h>
#include <user_interface.h>

#include "../lobaro-coap/coap.h"
#include "wifi_cfg_res.h"

CoAP_Res_t* pWifi_Res = NULL;

//...?key=WIFI_RES_KEY... must be provided to get/put to the wifi resource
//comment out define if you don't want to "secure" this resource
#define WIFI_RES_KEY "123"

#define SSID_QUERY_PREFIX "ssid="
#define PW_QUERY_PREFIX "pw="

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR ResWifi_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	struct station_config cfg;
	CoAP_option_t* pOpt;
	bool key_ok = false;

	if(!wifi_station_get_config(&cfg)) return HANDLER_ERROR; //will set "RESP_INTERNAL_SERVER_ERROR_5_00"

//check for ?key=xyz option and compare to WIFI_RES_KEY
	for(pOpt=pReq->pOptionsList; pOpt != NULL; pOpt = pOpt->next) {
		if(!key_ok) {
			key_ok = CoAP_UriQuery_KeyCorrect(pOpt, WIFI_RES_KEY);
		} else break;
	}

	if(!key_ok){
		const char infoStr[] = "Usage: coap://[host:port]/[...]/?key=KEY&" SSID_QUERY_PREFIX "WIFI-SSID&" PW_QUERY_PREFIX "WIFI-PASSWORD" "\r\nUse only over softAP connection!";
		CoAP_SetPayloadBlockwise(pReq, pResp, (uint8_t*)infoStr, coap_strlen(infoStr), true);
		pResp->Code = RESP_ERROR_UNAUTHORIZED_4_01;
	    return HANDLER_OK;
	}
//correct key supplied -> go on...

	if(pReq->Code == REQ_GET) {
			int TotalLength =  os_strlen(cfg.ssid)+os_strlen(cfg.password)+os_strlen(SSID_QUERY_PREFIX)+os_strlen(PW_QUERY_PREFIX)+1;

			//Check if initial payload buffer offered by stack is big enough
			if(pResp->PayloadBufSize < TotalLength) { //it's not...
				uint8_t* pGreaterPayload = (uint8_t*)coap_mem_get(TotalLength); //will be garbage collected by stack later
				if(pGreaterPayload == NULL) return HANDLER_ERROR; //will set "RESP_INTERNAL_SERVER_ERROR_5_00"
				else pResp->Payload = pGreaterPayload; //old memory "lost" location will be freed along with msg
			}

			os_sprintf(pResp->Payload,"%s%s,%s%s",SSID_QUERY_PREFIX,cfg.ssid,PW_QUERY_PREFIX,cfg.password);
			CoAP_SetPayloadBlockwise(pReq, pResp, pResp->Payload, TotalLength, false); //case 3) of function usage

	}else if(pReq->Code == REQ_PUT) {
		bool ssid_found=false;
		bool pw_found=false;
		uint8_t* pVal=NULL;
		uint8_t ValLen;

		for(pOpt =pReq->pOptionsList ;pOpt != NULL; pOpt = pOpt->next) { //iterate over all options of request and look for uri-query entries

			if(!ssid_found){
				pVal = CoAP_GetUriQueryVal(pOpt, SSID_QUERY_PREFIX, &ValLen);
				if(pVal && ValLen < 32) {
					coap_memcpy(cfg.ssid, pVal, ValLen);
					cfg.ssid[ValLen] = 0; //cstring termination
					ssid_found = true;
					continue;
				}
			}

			if(!pw_found){
				pVal = CoAP_GetUriQueryVal(pOpt, PW_QUERY_PREFIX, &ValLen);
				if(pVal && ValLen < 64) {
					coap_memcpy(cfg.password, pVal, ValLen);
					cfg.password[ValLen] = 0; //cstring termination
					pw_found = true;
					continue;
				}
			}
		}

		if(pw_found && ssid_found) {
			wifi_station_disconnect();
			if(wifi_station_set_config(&cfg)){
				ets_uart_printf("- Wifi config updated (ssid=%s, pw=%s)\r\nESP8266 is (re)connecting using new wifi params...\r\n", cfg.ssid,cfg.password);
				wifi_station_connect();
				pResp->Code=RESP_SUCCESS_CHANGED_2_04;
			}
			else return HANDLER_ERROR;
		}else {
			const char infoStr[] = "Usage: coap://[host:port]/[...]/?key=KEY&" SSID_QUERY_PREFIX "WIFI-SSID&" PW_QUERY_PREFIX "WIFI-PASSWORD" "\r\nUse only over softAP connection!";
			CoAP_SetPayloadBlockwise(pReq, pResp, (uint8_t*)infoStr, coap_strlen(infoStr), true);
			pResp->Code=RESP_ERROR_BAD_REQUEST_4_00;
		}
	}//end "PUT"

 return HANDLER_OK;
}


CoAP_Res_t* ICACHE_FLASH_ATTR Create_Wifi_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET | RES_OPT_PUT};
	return (pWifi_Res=CoAP_CreateResource("wifi/config", "WIFI-Parameter (protected by key)",Options, ResWifi_ReqHandler, NULL));
}
