#include <os_type.h>

#include "../lobaro-coap/coap.h"
#include "about_res.h"

static const char CoapInfoStringInFlash[] = {"\
The Constrained Application Protocol (CoAP) is a specialized web \
transfer protocol for use with constrained nodes and constrained \
(e.g., low-power, lossy) networks.  The nodes often have 8-bit \
microcontrollers with small amounts of ROM and RAM, while constrained \
networks such as IPv6 over Low-Power Wireless Personal Area Networks \
(6LoWPANs) often have high packet error rates and a typical \
throughput of 10s of kbit/s.  The protocol is designed for machine- \
to-machine (M2M) applications such as smart energy and building \
automation.\r\n\r\n\
\
CoAP provides a request/response interaction model between \
application endpoints, supports built-in discovery of services and \
resources, and includes key concepts of the Web such as URIs and \
Internet media types.  CoAP is designed to easily interface with HTTP \
for integration with the Web while meeting specialized requirements \
such as multicast support, very low overhead, and simplicity for \
constrained environments.\
"};

CoAP_Res_t* pAbout_Res = NULL;

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR ResAbout_ReqHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	static uint16_t payloadSize = sizeof(CoapInfoStringInFlash)-1;

	if(pReq->Code == REQ_GET) {
		CoAP_SetPayloadBlockwise(pReq, pResp, (uint8_t*)&(CoapInfoStringInFlash[0]), payloadSize, false);
	} else {
		pResp->Code=RESP_ERROR_BAD_REQUEST_4_00;
	}
	return HANDLER_OK;
}



CoAP_Res_t* ICACHE_FLASH_ATTR Create_About_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET};
	return (pAbout_Res=CoAP_CreateResource("about/coap", "CoAP Description",Options, ResAbout_ReqHandler, NULL));
}

