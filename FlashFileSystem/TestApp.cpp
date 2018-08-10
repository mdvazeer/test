// FlashFileSystem.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include "ffs.h"
#include "ffs_api.h"

#define NUMBER_OF_PAGES 3
#define NUMBER_OF_FILES  3

int main(int argc, char** argv) {


	uint16_t length = 10;
	uint16_t *j = NULL;
	uint8_t i;

	uint8_t *page1 = (uint8_t*)malloc(8000);
	uint8_t *page2 = (uint8_t*)malloc(8000);
	uint8_t *page3 = (uint8_t*)malloc(8000);


	//int *AddrList[] = {0x124, 0x125, 0x1222}

	//uint16_t AddrList[NUMBER_OF_PAGES] = { (uint16_t)page1, (uint16_t)page2, (uint16_t)page2 };
	uint8_t *AddrList[NUMBER_OF_PAGES] = { (uint8_t *)0x100000, (uint8_t *)0x102000, (uint8_t *)0x104000};

	i = initFlashFileSystem(AddrList, NUMBER_OF_PAGES, NUMBER_OF_FILES);
	
	//uint8_t data[] = "This is a boy";
	//i = writeFile(1, data, strlen((char*)data));

	//i = getFile(2, &j, &length);

	getchar();
	return 0;
}

