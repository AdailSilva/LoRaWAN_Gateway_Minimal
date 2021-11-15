/* 
 *   
 *  Project:          IoT Energy Meter with C/C++, Java/Spring, TypeScript/Angular and Dart/Flutter;
 *  About:            End-to-end implementation of a LoRaWAN network for monitoring electrical quantities;
 *  Version:          1.0;
 *  Backend Mote:     ATmega328P/ESP32/ESP8266/ESP8285/STM32;
 *  Radios:           RFM95w and LoRaWAN EndDevice Radioenge Module: RD49C;
 *  Sensors:          Peacefair PZEM-004T 3.0 Version TTL-RTU kWh Meter;
 *  Backend API:      Java with Framework: Spring Boot;
 *  LoRaWAN Stack:    MCCI Arduino LoRaWAN Library (LMiC: LoRaWAN-MAC-in-C) version 3.0.99;
 *  Activation mode:  Activation by Personalization (ABP) or Over-the-Air Activation (OTAA);
 *  Author:           Adail dos Santos Silva
 *  E-mail:           adail101@hotmail.com
 *  WhatsApp:         +55 89 9 9433-7661
 *  
 *  WARNINGS:
 *  Permission is hereby granted, free of charge, to any person obtaining a copy of
 *  this software and associated documentation files (the “Software”), to deal in
 *  the Software without restriction, including without limitation the rights to
 *  use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 *  the Software, and to permit persons to whom the Software is furnished to do so,
 *  subject to the following conditions:
 *  
 *  The above copyright notice and this permission notice shall be included in all
 *  copies or substantial portions of the Software.
 *  
 *  THE SOFTWARE IS PROVIDED “AS IS”, WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 *  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 *  FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 *  COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 *  IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 *  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *  
 */

/********************************************************************
 _____              __ _                       _   _             
/  __ \            / _(_)                     | | (_)            
| /  \/ ___  _ __ | |_ _  __ _ _   _ _ __ __ _| |_ _  ___  _ __  
| |    / _ \| '_ \|  _| |/ _` | | | | '__/ _` | __| |/ _ \| '_ \ 
| \__/\ (_) | | | | | | | (_| | |_| | | | (_| | |_| | (_) | | | |
 \____/\___/|_| |_|_| |_|\__, |\__,_|_|  \__,_|\__|_|\___/|_| |_|
                          __/ |                                  
                         |___/                                   
********************************************************************/

#include <WiFiUdp.h>
#include <ESP8266WiFi.h>

#define STASSID             "LoRaWAN  [ 2.4 GHz ]"
#define STAPSK              "@Hacker101"
#define _NETWORKSERVER      "thethings.meshed.com.au"
#define _NETWORKSERVER_PORT 1700

/*****************************
 _____           _      
/  __ \         | |     
| /  \/ ___   __| | ___ 
| |    / _ \ / _` |/ _ \
| \__/\ (_) | (_| |  __/
 \____/\___/ \__,_|\___|
*****************************/

IPAddress networkServer;

WiFiUDP NETWORKSERVER_UDP;

uint8_t MAC_array[6];
uint32_t proximoEnvioDados = 0;

/*****************************
 _____      _               
/  ___|    | |              
\ `--.  ___| |_ _   _ _ __  
 `--. \/ _ \ __| | | | '_ \ 
/\__/ /  __/ |_| |_| | |_) |
\____/ \___|\__|\__,_| .__/ 
                     | |    
                     |_|    
******************************/

void setup() {

	Serial.begin(115200);
	WiFi.mode(WIFI_STA);
	WiFi.begin(STASSID,STAPSK);
	Serial.println();
	Serial.println("Conectado ao WiFi:");
	
	while(WiFi.status() != WL_CONNECTED) {

		Serial.print('.');

		delay(500);	

	}

	Serial.print("Connected! IP address: ");
	Serial.println(WiFi.localIP());

	Serial.print("IP do servidor: ");
	WiFi.hostByName(_NETWORKSERVER,networkServer);

	bool conectadoUDP = ( NETWORKSERVER_UDP.begin((int) _NETWORKSERVER_PORT) == 1 );

	if (conectadoUDP) {
		Serial.print("Conectado ao servidor: ");
		Serial.println(networkServer);
	}

	char MAC_char[18] = {0};
	WiFi.macAddress(MAC_array);
	for (int i = 0; i < sizeof(MAC_array); ++i) {
		sprintf(MAC_char,"%s%02x:",MAC_char,MAC_array[i]);
	}
	
	Serial.print( F( "MAC: " ) );
	Serial.println(MAC_char);
	
}

/*****************************
 _                       
| |                      
| |     ___   ___  _ __  
| |    / _ \ / _ \| '_ \ 
| |___| (_) | (_) | |_) |
\_____/\___/ \___/| .__/ 
                  | |    
                  |_|    
*****************************/

void loop {

	yield();

	/* Leitura da porta UDP */
	int packetSize = NETWORKSERVER_UDP.parsePacket();
	if( packetSize > 0 ) {

	uint8_t receiveBuffer[100];
	NETWORKSERVER_UDP.read(receiveBuffer,packetSize);
	receiveBuffer[packetSize] = 0;

	IPAddress remoteIpNo = NETWORKSERVER_UDP.remoteIP();
	unsigned int remotePortNo = NETWORKSERVER_UDP.remotePort();
	
	Serial.print( F("Pacote recebido: ") );
	Serial.print(packetSize);
	Serial.print( F(" bytes de ") );
	Serial.print(remoteIpNo);
	Serial.print( F(", porta ") );
	Serial.print(remotePortNo);
	Serial.print( F(", contendo: 0x") );

	for (int i = 0; i < packetSize; i++){
		Serial.printf("%02X:", receiveBuffer[i]);
		}
	Serial.println();
	
	}

	/* envia dados */
	if (millis() > proximoEnvioDados) {
		proximoEnvioDados = millis() + 2000;
		enviaDados();
	}

}

void enviaDados() {
	
	char msg[] = "000000000000"
	"{"
	"\"rxpk\":[{"
	"\"tmst\":13202941,"
	"\"chan\":0,"
	"\"rfch\":0,"
	"\"freq\":916.800000,"
	"\"stat\":1,"
	"\"modu\":\"LORA\","
	"\"datr\":\"SF7BW125\","
	"\"codr\":\"4/5\","
	"\"lsnr\":0,"
	"\"rssi\":-60,"
	"\"size\":23,"
	"\"data\":\"QJkUASYAAAABodsMSXtu+qxauVfYT/U=\""
	"}]"
	"}";

	int bytesEnviar = sizeof(msg) -1;
	
	msg[0]  = 1; //PROTOCOL_VERSION;
	msg[1]  = (uint8_t)rand();
	msg[2]  = (uint8_t)rand();
	msg[3]  = 0; //PKT_PUSH_DATA;
	msg[4]  = MAC_array[0];
	msg[5]  = MAC_array[1];
	msg[6]  = MAC_array[2];
	msg[7]  = 0xFF;
	msg[8]  = 0xFE;
	msg[9]  = MAC_array[3];
	msg[10] = MAC_array[4];
	msg[11] = MAC_array[5];

	Serial.println();
	
	for (int i = 0; i < 12; i++) {
		Serial.printf("%02X:",msg[i]);
	}

	for (int i = 12; i < bytesEnviar; i++) {
		Serial.write(msg[i]);
	}

	Serial.println();

	NETWORKSERVER_UDP.beginPacket(networkServer, (int)_NETWORKSERVER_PORT);

	if (NETWORKSERVER_UDP.write(msg,bytesEnviar)!= bytesEnviar) {
		Serial.println( F("Erro ao enviar pacote") );
		} else {
			Serial.println(String(bytesEnviar) + "bytes enviados");
			}
		NETWORKSERVER_UDP.endPacket();

}

/*
01:42:17:00:5C:CF:7F:FF:FE:06:C0:3E // VERIFICAR PARA EM SEGUIDA ALTERAR

{
	"rxpk":[{
		"tmst": 13202941,
		"chan": 0,
		"rfch": 0,
		"tmst": 916.800000,
		"stat": 1,
		"modu": "LORA",
		"datr": "SF7BW125",
		"codr": "4/5",
		"lsnr": 0,
		"rssi": -60,
		"size": 23,
		"data": "QJkUASYAAAABodsMSXtu+qxauVfYT/U=" // VERIFICAR PARA EM SEGUIDA ALTERAR
	}]
}
*/

// Protocol:
// https://github.com/Lora-net/packet_forwarder/blob/master/PROTOCOL.TXT
