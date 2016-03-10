/*
 * Test.cpp
 *
 *  Created on: 2016/2/25
 *      Author: D10307009
 */

#include "Test.h"
#include "main.h"
#include "Selection.h"

void mem() {
	int stack;
	int *heap = new int;
	pc.printf("mem: stack %x, heap %x, free %x\r\n", &stack, heap,
			&stack - heap);
	free(heap);
}

#include "wave_player.h"
#include "SDFileSystem.h"

AnalogOut DACout(p18);
wave_player waver(&DACout);

void test_wav() {
	mem();
	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
	sd.disk_initialize();

	FILE *fp = fopen("/sd/test.wav", "r");
	if (fp == NULL) {
		pc.printf("File couldn't open\n");
	}

	pc.printf("File Playing\n");
	lcd.cls();
	lcd.printf("Audio Testing");
	waver.play(fp);

	fclose(fp);
	sd.unmount();
}

#include "EthernetInterface.h"
#include "HTTPClient.h"
#include "HTTPSOAP.h"
#include "Base64.h"

void stat() {
	uint32_t s = LPC_EMAC->Status;
	pc.printf("S:%x", s);
}

void test_eth() {
	mem();
	EthernetInterface eth;
	mem();
	if (eth.getIPAddress()[0] == '\0') {
		eth.init();
	}
	mem();
	eth.connect();
	mem();
	pc.printf("IP Address is %s\r\n", eth.getIPAddress());
	mem();

//	const char * txt = "0123456789";
//	size_t tens;
//	char * txen;

//	mem();
	Base64 base64;
//	txen = base64.Encode(txt, strlen(txt), &tens);
//	pc.printf(txen);

	mem();
	SDFileSystem sd(p5, p6, p7, SD_EN, "sd");
	sd.disk_initialize();

	mem();
	FILE *source = fopen("/sd/0.txt", "r");
	if (source == NULL) {
		pc.printf("Could not open file\r\n");
	}

	mem();
	FILE *base = fopen("/sd/0.tmp", "w");
	if (base == NULL) {
		pc.printf("Could not open file\r\n");
	}

	mem();
	base64.Encode(base, source);

	mem();
	fclose(source);
	fclose(base);

	mem();
	FILE *file_base = fopen("/sd/test.txt", "r");
	if (file_base == NULL) {
		pc.printf("Could not open file\r\n");
	}

	mem();
	char str[512] = "mmm\r\n";
	HTTPMultipart outText("id");
	HTTPSOAP tt("myid", file_base);
	HTTPText inText(str, 512);

	HTTPClient http;

	mem();
	int ret = http.post("http://192.168.0.100:8080/MotorWeb/StreamingImpl", tt,
			&inText);
	pc.printf("Result: %s\n", str);
	if (!ret) {
		pc.printf("Page fetched successfully - read %d characters\n",
				strlen(str));
	} else {
		pc.printf("Error - ret = %d - HTTP return code = %d\n", ret,
				http.getHTTPResponseCode());
	}

	mem();
	fclose(file_base);

	mem();
	sd.unmount();

	mem();
	eth.disconnect();
	mem();
}

#include "SerRAM.h"

void test_ram() {
	lcd.cls();
	lcd.printf("SRAM Testing");
	pc.printf("sram");
	SerRAM sram(P1_24, P1_23, P1_20, P1_21, 1024);
	uint8_t i;
	uint8_t d[] = {0x12, 0x34, 0x56, 0x78, 0x9A, 0xBC, 0xDE, 0xF0, 0x12, 0x34};
	uint8_t r[10];
	pc.printf("read:");
	sram.read(0, (char*) &r, 10, false);
	for(i = 0; i < 10; i++) {
		pc.printf(" %x", r[i]);
	}
	pc.printf("\r\n");
	pc.printf("write\r\n");
	sram.write(0,(char*) &d, 10, false);
	pc.printf("read:");
	sram.read(0,(char*) &r, 10, false);
	for(i = 0; i < 10; i++) {
		pc.printf(" %x", r[i]);
	}
	pc.printf("\r\n");
	pc.printf("read:");
	sram.read(1,(char*) &r, 10, false);
	for(i = 0; i < 10; i++) {
		pc.printf(" %x", r[i]);
	}
	pc.printf("\r\n");
//	int r = sram.read(0);
//	pc.printf("r:%x", r);
}

const char * TXT_MENU = "Develop Test";

FunctionPointer fun_eth(&test_eth);
FunctionPointer fun_wav(&test_wav);
FunctionPointer fun_ram(&test_ram);

Test::Test(Menu *parent) :
		m_menu(TXT_MENU, parent) {
	m_menu.add(Selection(&fun_eth, 0, NULL, " Ethernet"));
	m_menu.add(Selection(&fun_wav, 1, NULL, " Audio"));
	m_menu.add(Selection(&fun_ram, 2, NULL, " SRAM"));
}

Menu *Test::getMenu() {
	return &m_menu;
}