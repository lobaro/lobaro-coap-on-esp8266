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

static CoAP_HandlerResult_t ICACHE_FLASH_ATTR RequestHandler(CoAP_Message_t* pReq, CoAP_Message_t* pResp) {
	static uint16_t payloadSize = sizeof(CoapInfoStringInFlash)-1;

	CoAP_SetPayload(pReq, pResp, (uint8_t*)&(CoapInfoStringInFlash[0]), payloadSize, false);

	return HANDLER_OK;
}

CoAP_Res_t* ICACHE_FLASH_ATTR Create_About_Resource() {
	CoAP_ResOpts_t Options = {.Cf = COAP_CF_TEXT_PLAIN, .Flags = RES_OPT_GET};
	return (pAbout_Res=CoAP_CreateResource("about/coap", "CoAP Description",Options, RequestHandler, NULL));
}

