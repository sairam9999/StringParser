// StringParser.cpp : Defines the entry point for the console application.
//

#include "Parser.h"
#include <iostream>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <conio.h>

#define LINE_BITS 4
#define DATE_BITS 7
#define YEAR_BITS 16
#define LOCATION_BITS 22
#define PRODUCTID_BITS 25
#define ENGINEERING_BITS 30

#define LOT_MASK ((1<<4) - 1)
#define LINE_MASK ((1<<3) -1)<<LINE_BITS
#define DATE_MASK ((1<<9) -1)<<DATE_BITS
#define YEAR_MASK ((1<<6) -1)<<YEAR_BITS
#define LOCATION_MASK ((1<<3) -1)<<LOCATION_BITS
#define PRODUCTID_MASK ((1<<6) -1)<<PRODUCTID_BITS
#define ENGINEERING_MASK (1<<1)<<ENGINEERING_BITS

enum ERROR_CODE{
	SUCCESS,
	NO_MEM,
	BUILD_LOOKUP_ERROR, 
	LOCATION_ERROR, 
	PRODUCT_ERROR,
	INVALID_DATE,
	INVALID_LOT,
	INVALID_LINE
};

struct DateContainer{
	int day;
	int month;
};

Parser::Parser(const char *_str)
{
	strcpy(str,_str);
	str[32] =NULL;
}

/* 
 * We are expecting year to be from 2000 to 2063 hence leap year checks for 100 and 400 are not
 * necessary. only %4 should be sufficient for the range of years we are going to get as input
 */
DateContainer* dateLookup(int _dayOfYear, int _year)
{
	time_t loctime;
	struct tm timeinfo, *loctimeinfo;
	int year, day;
	bool bLeapYear = false;

	if (_dayOfYear < 0)
		return NULL;

	if (_year < 0)
		return NULL;

	if((_year % 4) == 0)
		bLeapYear = true;

	if (bLeapYear && _dayOfYear > 366) {
		return NULL;
	}
	if (!bLeapYear && _dayOfYear > 365) {
		return NULL;
	}

	year = 2000 + _year;
	DateContainer *instance = NULL;
	day = _dayOfYear;

	/* initialize timeinfo and modify it to the user's choice */
	// bzero(&timeinfo, sizeof(struct tm));

	memset(&timeinfo,0,sizeof(tm));
	timeinfo.tm_isdst = -1;  /* Allow mktime to determine DST setting. */
	timeinfo.tm_mon   = 0;
	timeinfo.tm_mday = day;
	timeinfo.tm_year = year - 1900;

	loctime = mktime (&timeinfo);
	if(-1==loctime)
	{
		fprintf(stderr, "%s\n","Invalid Date");
		return NULL;

	}

	loctimeinfo = localtime(&loctime);
	instance = (DateContainer*) malloc(sizeof(DateContainer));
	memset(instance,0,sizeof(DateContainer));
	instance->day = loctimeinfo->tm_mday;
	instance->month = loctimeinfo->tm_mon;

	return instance;

	//printf ("The date for that day of the year is %s.\n", asctime(loctimeinfo));
}

char buildLookup(int buildValue) {
	switch(buildValue)
	{
	case 0:
		return 'P';
	case 1:
		return 'E';
	default: 
		//Enter log
		return '0';
	}
}

char * productIDLookup (int productValue) {
	switch(productValue)
	{
	case 4:
		return "D1";
	case 24:
		return "D2";
	case 54:
		return "D5";
	case 2:
		return "E1";
	case 22:
		return "E2";
	case 52:
		return "E5";
	case 3:
		return "A0";
	case 5:
		return "P1";
	case 25:
		return "P2";
	case 55:
		return "P5";
	default: 
		//Enter log
		return NULL;
	}
}

char locationLookup(int locationValue){
	switch(locationValue)
	{
	case 1:
		return 'U';
	case 0:
		return 'C';
	default: 
		//Enter log
		return '0';
	}
}



int constructLotEntry (uint32_t _id, char** _lotEntry)
{
	char* lotentry = (char*)malloc(13*sizeof(char));
	if(NULL==lotentry)
	{
		return NO_MEM;
	}
	
	uint32_t id = _id;
	uint32_t lotValue = (id & LOT_MASK);
	uint32_t lineValue = (id & LINE_MASK)>>4;
	uint32_t dateValue = (id & DATE_MASK)>>7;
	uint32_t yearValue = (id & YEAR_MASK)>>16;
	uint32_t locationValue = (id & LOCATION_MASK)>>22;
	uint32_t productIdValue = (id & PRODUCTID_MASK)>>25;
	uint32_t engineeringValue = (id & ENGINEERING_MASK)>>31;
	DateContainer * date = dateLookup(dateValue, yearValue);
	
	if (lotValue == 0){
	return INVALID_LOT;

	}
	if (lineValue == 0){
	return INVALID_LINE;
	}
	if(NULL == date){
		return INVALID_DATE;
	}

	char locationId = locationLookup(locationValue);
	if('0' == locationId)
	{
		return LOCATION_ERROR;
	}

	char buildId = buildLookup(engineeringValue);
	if('0' == buildId)
	{
		return BUILD_LOOKUP_ERROR;
	}

	char * productId = productIDLookup(productIdValue);
	if(NULL == productId)
	{
		return PRODUCT_ERROR;
	}
	

	sprintf(lotentry, "%c%s%c%02d%02d%02d%d%0X (%X)", buildId,productId, locationId, 
		date->day,date->month + 1, yearValue, lineValue, lotValue, _id);
	*_lotEntry = lotentry;
	return SUCCESS;

}

int main(){

	int i = 0;
		
	unsigned long lots[20] = {2282815632, 135362817, 806485913, 2324905745, 2119463337,
		2323607913, 846732844, 3996861652, 2960140517, 70340159, 742104136, 3898969437, 103396586, 
		2960924286, 1817263830, 2282856466, 2991791660, 1746303581, 2991849388, 33686018 };
	
	char * pLot = NULL;

	for (i=0; i<20; i++) {
		printf("%d ", (i+1));
		if(SUCCESS == constructLotEntry(lots[i] ,&pLot))
		{
			printf("%s\n", pLot);
		}
		else{
			printf("Invalid Lot (%X)\n", lots[i]);
		}
	}
getch();
}
