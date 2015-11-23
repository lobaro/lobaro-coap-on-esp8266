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
#include "wifi_scan_res.h"

CoAP_Res_t* pWifi_scan_Res = NULL;

int lastScanResultLen = 0;
uint8_t* pScanResultStr = NULL;

#define SCAN_STATE_IDLE 		(0)
#define SCAN_STATE_RUNNING 		(1)
#define SCAN_STATE_DONE_OK		(2)
#define SCAN_STATE_DONE_FAIL 	(3)
uint8_t ScanState = SCAN_STATE_IDLE;

//ugly example from expressif samples...
static void ICACHE_FLASH_ATTR scan_done(void *arg, STATUS status)
{
  uint8 ssid[33];
  char temp[128];

  ets_uart_printf("scan status = %d\r\n",status);
  if (status == OK)
  {
    struct bss_info *bss_link = (struct bss_info *)arg;
    bss_link = bss_link->next.stqe_next;//ignore the first one , it's invalid.

    lastScanResultLen = 0;

    //"dry run" to count needed mem
    //todo implement more elegant solution... ;)
    while(bss_link!=NULL) {
		os_memset(ssid, 0, 33);
		if (os_strlen(bss_link->ssid) <= 32) {
		  os_memcpy(ssid, bss_link->ssid, os_strlen(bss_link->ssid));
		}
		else {
		  os_memcpy(ssid, bss_link->ssid, 32);
		}
		lastScanResultLen+=coap_sprintf(temp, "(%d,\"%s\",%d,\""MACSTR"\",%d)\r\n",
				   bss_link->authmode, ssid, bss_link->rssi,
				   MAC2STR(bss_link->bssid),bss_link->channel);

		ets_uart_printf("%s",temp); //at least use it to show on uart...

		bss_link = bss_link->next.stqe_next;
    }

    //Get needed memory
    //Porting notice: on non cooperative os (e.g. FreeRTOS) the block between alloc and SCAN_STATE_DONE_OK MUST be atomic!
	pScanResultStr = (uint8_t*)coap_mem_get0(lastScanResultLen);
	uint8_t* pStrWorking = pScanResultStr;

    bss_link = (struct bss_info *)arg;
    bss_link = bss_link->next.stqe_next;//ignore the first one , it's invalid.

    while (bss_link != NULL) {
      os_memset(ssid, 0, 33);
      if (os_strlen(bss_link->ssid) <= 32) {
        os_memcpy(ssid, bss_link->ssid, os_strlen(bss_link->ssid));
      }
      else {
        os_memcpy(ssid, bss_link->ssid, 32);
      }

      pStrWorking+= coap_sprintf((char*)pStrWorking, "(%d,\"%s\",%d,\""MACSTR"\",%d)\r\n",
			   bss_link->authmode, ssid, bss_link->rssi,
			   MAC2STR(bss_link->bssid),bss_link->channel);

      bss_link = bss_link->next.stqe_next;
    }
    ScanState = SCAN_STATE_DONE_OK;
  }
  else if (status == FAIL || status == CANCEL) {
	  ets_uart_printf("scan fail !!!\r\n");
	  ScanState = SCAN_STATE_DONE_FAIL;
  }

}

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR Res_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
  if(pReq->Code == REQ_GET) {

	  switch(ScanState){
	  	  case SCAN_STATE_IDLE:
	  		  wifi_station_scan(NULL,scan_done);
	  		  ScanState = SCAN_STATE_RUNNING;
	  		  return HANDLER_POSTPONE; //coap stack will comeback later

	  	  case SCAN_STATE_RUNNING:
	  		  return HANDLER_POSTPONE; //coap stack will comeback later

	  	  case SCAN_STATE_DONE_OK:
	  		  ScanState = SCAN_STATE_IDLE;

	  		 pResp->Payload = pScanResultStr; //will be freed together with response msg, same for overwritten memory location
	  		 pResp->PayloadLength = lastScanResultLen;

	  		  //we don't allow non-atomic/blockwise transfers here and do some work to give client diagnostic payload
	  		  CoAP_blockwise_option_t B2opt;
	  		  if(GetBlock2OptionFromMsg(pReq, &B2opt) == COAP_OK) {
	  			if((int)B2opt.BlockSize < lastScanResultLen) {
	  				pResp->Code=RESP_BAD_OPTION_4_02;
	  				if(pResp->PayloadBufSize>16) {
	  					coap_sprintf(pResp->Payload, "blk2 n/a [%d]",lastScanResultLen);
	  					pResp->PayloadLength = coap_strlen(pResp->Payload);
	  				}
	  				return HANDLER_OK;
	  			}
	  		  }

			return HANDLER_OK;

	  	  case SCAN_STATE_DONE_FAIL:
	  		ScanState = SCAN_STATE_IDLE;
	  		return HANDLER_ERROR;
	  }
  }
  return HANDLER_ERROR;
}


CoAP_Res_t* ICACHE_FLASH_ATTR Create_Wifi_Scan_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET};
	return (pWifi_scan_Res=CoAP_CreateResource("wifi/scan", "result of wifi network scan",Options, Res_ReqHandler, NULL));
}
