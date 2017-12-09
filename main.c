#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "censusADT.h"

#define MAX_LINE 80
#define MAX_ASCII 128

#define ERASE_LINE(c) while(((c)=getchar())!='\n' && (c)!=EOF)
#define IS_VALID_STATUS(c) (isdigit(c) && (c)>='0' && (c)<='3' && size==0)
#define ABS(c) ((c)>0?(c):(-(c))) 

#define ECONOMIC_STATUS 0
#define HOME_ID 1
#define DEPT 2
#define PROV 3
#define ERROR 4

#define PATH_COUNTRY "./country.csv"
#define PATH_PROVINCE "./province.csv"
#define PATH_DEPARTMENT "./department.csv"

/*
	Reads a line from stdin and returns as argumen the economic status value, in s
	it stores both the dept name followed by a '\0' and the province name followed by '\0'.
	It also returns a pointer to the beginning of the department name and anotherone to the
	beginning of the province name.
	It returns 1 if it managed to do all of this accordingly.

	In case of an invalid line it returns:
		-1 if the economic status is not valid.
		-2 if the home id is invalid (only checks if it is a number).
		-3 if the department name contains an invalid character or if its empty.
		-4 if the province name contains an invalid character or if its empty.
		-5 if the line is larger than the maximum allowed value.

*/
int readLine(int * status, char * s,char ** dept, char ** prov);
int errorLineHandler(FILE * logFile, int errorType, unsigned long line);

int main(void){
	censusADT c = newCensus();
	int status, errorStatus=0, k, totalErrors=0;
	unsigned long line = 0;
	char * dept, * prov;
	//4 is the minimum space used by the status, home id and commas.
	char s[MAX_LINE - 4];
	
	FILE * log;
	log = fopen("./log.txt", "w");

	while((k=getchar())!=EOF){
		line++;
		ungetc(k, stdin);

		errorStatus=0;
		errorStatus = readLine(&status, s, &dept, &prov);
		if (errorStatus!=1){
			totalErrors += errorLineHandler(log, errorStatus, line);
		}
		else
			addElem(c, status, dept, prov);
		
	}

	fprintf(log, "\n\nTotal Ignored lines: %d\n", totalErrors);
	fclose(log);
	
	storeToFiles(c, PATH_COUNTRY, PATH_PROVINCE, PATH_DEPARTMENT);
	freeCensus(c);
	
	return 1;
}
int errorLineHandler(FILE * logFile, int errorType, unsigned long line){

	fprintf(logFile,"E:%d.L:%ld - ", ABS(errorType), line);
	switch(ABS(errorType)){
		case 1:
			fprintf(logFile,"Invalid economic status identifier");
			break;
		case 2:
			fprintf(logFile,"Invalid home identifier");
			break;
		case 3:
			fprintf(logFile,"Invalid character in department name");
			break;
		case 4:
			fprintf(logFile,"Invalid character in province name");
			break;
		case 5:
			fprintf(logFile,"Line is longer than the maximum allowed value");
			break;
	}
	fprintf(logFile,"...Ignoring line %ld\n", line);

	return 1;		
}

int readLine(int * status, char * s,char ** dept, char ** prov){
	int size=0,option=0, errorStatus=1, c;
	//sets the at the beginning of the return string.
	*dept = s;

	while(size<MAX_LINE && (c=getchar())!='\n' && c!=EOF){
		switch(option){

			case ECONOMIC_STATUS:
				if(IS_VALID_STATUS(c))
					*status=c -'0';
				else if(c==','&& size==1){
					option=HOME_ID;
				}
				else{
					errorStatus=-1;
					option=ERROR;
				}
				break;


			case HOME_ID:
				if(c==',' && size>2){
					option=DEPT;
				}
				else if (!isdigit(c)){
					errorStatus=-2;
					option=ERROR;
				}
				break;						
			

			case DEPT:
				if(c!=',' && c<=MAX_ASCII){
					*s = c;
					s++;
				}
				else if (c==',' && s!=*dept){
					*s=0;
					option=PROV;
					s++;
					*prov = s;
				}
				else {
					errorStatus=-3;
					option=ERROR;
				}
				break;


			case PROV:
				if(c!=',' && c<MAX_ASCII){
					*s = c;
					s++;
				}
				else {
					errorStatus=-4;
					option=ERROR;
				}
				break;

			case ERROR:
				break;
		}
		size++;
	}

	if (option==DEPT)
		errorStatus = -3;

	else if (option==PROV)
			*s=0;

	//If the line reading was terminated because it was too long it is then erased.
	if(c!='\n' && c!=EOF){

		//If the line had no errors other than it being too long the error type will be -5
		//otherwise the other errors will be prioritized.
		if(errorStatus==1)
			errorStatus=-5;

		ERASE_LINE(c);
	}
	return errorStatus;
}

