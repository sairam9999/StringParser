// StringParser.cpp : Defines the entry point for the console application.
//

#include "Parser.h"
#include <iostream>
#include <string.h>
#include <time.h>
#include <stdint.h>
#include <conio.h>

#define LOT_MASK ((1<<4) - 1)
#define LINE_MASK ((1<<3) -1)<<4
#define DATE_MASK ((1<<9) -1)<<7
#define YEAR_MASK ((1<<6) -1)<<16
#define LOCATION_MASK ((1<<3) -1)<<22
#define PRODUCTID_MASK ((1<<6) -1)<<25
#define ENGINEERING_MASK (1<<1)<<30

enum ERROR_CODE{
	SUCCESS,
	NO_MEM,
	BUILD_LOOKUP_ERROR, 
	LOCATION_ERROR, 
	PRODUCT_ERROR,
	INVALID_DATE
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
//Sairam
	loctimeinfo = localtime(&loctime);
	instance = (DateContainer*) malloc(sizeof(DateContainer));
	memset(instance,0,sizeof(DateContainer));

	instance->day = loctimeinfo->tm_mday;
	instance->month = loctimeinfo->tm_mon;

	return instance;

	//printf ("The date for that day of the year is %s.\n", asctime(loctimeinfo));
}

/*uint32_t bstr_to_dec(const char * str)
{   
uint32_t val = 0;


while (*str != '\0')
val = 2 * val + (*str++ - '0');
return val;
}*/

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

/*Parser::~Parser(void)
{
}*/


int constructLotEntry (uint32_t _id, char** _lotEntry)
{
	//	const char * binary = "11101100010100110001111000100100";->3964870180
	//Parser instance (binary);
	//char lotentry[13];

	char* lotentry = (char*)malloc(13*sizeof(char));
	if(NULL==lotentry)
	{
		//log
		return NO_MEM;
	}
	//uint32_t id = (uint32_t) bstr_to_dec(binary);
	//uint32_t id = 3964870180;

	uint32_t id = _id;
	//To get the value of Lot
	//uint32_t mask = (1<<4) - 1;
	uint32_t lotValue = (id & LOT_MASK);
	//printf("%d\n",lotValue);

	//To get the value of Line
	//uint32_t mask1 = ((1<<3) -1)<<4;
	//uint32_t mask1 = 112;
	//uint32_t lineValue = (id & mask1)>>4;
	uint32_t lineValue = (id & LINE_MASK)>>4;
	//printf("%d\n",lineValue);

	//TO get the value of Date
	//uint32_t mask2 = ((1<<9) -1)<<7;
	//uint32_t dateValue = (id & mask2)>>7;
	uint32_t dateValue = (id & DATE_MASK)>>7;
	//printf("%d\n",dateValue);

	//To get the value of Year
	//uint32_t mask3 = ((1<<6) -1)<<16;
	//uint32_t yearValue = (id & mask3)>>16;
	uint32_t yearValue = (id & YEAR_MASK)>>16;
	//printf("%d\n",yearValue);

	//To get the value of Location
	//uint32_t mask4 = ((1<<3) -1)<<22;
	//uint32_t locationValue = (id & mask4)>>22;
	uint32_t locationValue = (id & LOCATION_MASK)>>22;
	//printf("%d\n",value4);
	/*if(locationValue == 1){
	printf("USA\n");
	}
	else if (locationValue == 0){
	printf("China\n");
	}
	else{ 
	printf("Invalid\n");
	}*/

	//TO get the value of ProductID
	//uint32_t mask5 = ((1<<6) -1)<<25;
	//uint32_t productIdValue = (id & mask5)>>25;
	uint32_t productIdValue = (id & PRODUCTID_MASK)>>25;
	////printf("%d\n",productIdValue);

	//To get the value of Enginering Build
	//uint32_t mask6 = (1<<1)<<30;
	//printf("$$$%d\n", mask6);
	//uint32_t mask6 = 2147483648;
	uint32_t engineeringValue = (id & ENGINEERING_MASK)>>31;
	//printf("%d\n",engineeringValue);

	//printf("%c\n", buildLookup(engineeringValue));
	//printf("%s\n", productIDLookup(productIdValue));
	//printf("%c\n", locationLookup(locationValue));

	DateContainer * date = dateLookup(dateValue, yearValue);
	//printf("%d - %d - %d\n",date->day,date->month + 1, value3 + 2000);
	if(NULL == date){
	//	fprintf(stderr,"\t%s\n","Invlid Date\n");
		return INVALID_DATE;
	}

	char locationId = locationLookup(locationValue);
	if('0' == locationId)
	{
		//fprintf(stderr,"\t%s\n","Invalid Location");
		return LOCATION_ERROR;
	}

	char buildId = buildLookup(engineeringValue);
	if('0' == buildId)
	{
	//	fprintf(stderr,"\t%s\n","Invalid Build");
		return BUILD_LOOKUP_ERROR;
	}

	char * productId = productIDLookup(productIdValue);
	if(NULL == productIdValue)
	{
		//fprintf(stderr,"\t%s\n","Invalid Product");
		return PRODUCT_ERROR;
	}

	sprintf(lotentry, "%c%s%c%02d%02d%02d%d%0X (%X)", buildId,productId, locationId, 
		date->day,date->month + 1, yearValue, lineValue, lotValue, _id);
	//printf("%s\n" , lotentry);
	*_lotEntry = lotentry;
	//getch();
	return SUCCESS;

}
//1 110111 000 111111 111111111 001 0001
//1 000100 000 010001 101000000 001 0010
int main(){

	int i = 0;
	unsigned long lots[10] = {3997171601, 2282856466, -1, 2222222222, 99, -99, 'A', 2282815633, 135362833, 806466713 };

	char * pLot = NULL;

	for (i=0; i<10; i++) {
		printf("%d ", (i+1));
		if(SUCCESS == constructLotEntry(lots[i] ,&pLot))
		{
			printf("%s\n", pLot);
		}
		else{
			printf("Invalid Lot (%X)\n", lots[i]);
		}
		//free (pLot);
	
	}
getch();
}

