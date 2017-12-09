#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include "censusADT.h"

#define MAX_LINE 80
#define MAX_ASCII 128
#define OCCUPIED_VALUE 1
#define UNOCCUPIED_VALUE 2

//Erases line from buffer.
#define ERASE_LINE(c) while(((c)=getchar())!='\n' && (c)!=EOF)
//Validates that the economic status identif
#define STATUS_MAX_VALUE 3
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
	censusADT c = newCensus(OCCUPIED_VALUE, UNOCCUPIED_VALUE);
	if(c==NULL){
		fprintf(stderr, "Census could not be created. Not enough memory...Terminating program\n");
		
		return 1;
	}

	int status, errorStatus, k, memError=1;
	long totalLineErrors=0, totalMemErrors = 0;
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
			totalLineErrors += errorLineHandler(log, errorStatus, line);
		}
		else{
			memError = addElem(c, status, dept, prov);
			if (memError==0){
				fprintf(log, "E:M.L:%ld - UNAVAILABLE MEMORY...LINE WAS IGNORED\n", line);
				totalMemErrors++;
			}
		}
		
	}
	if(totalLineErrors>0 || totalMemErrors>0){
		fprintf(stderr, "%ld lines were ignored, check log.txt\n", totalLineErrors + totalMemErrors);
		fprintf(log, "\n\nTotal Ignored lines: %ld\n", totalLineErrors);
	}
	fclose(log);
	
	storeToFiles(c, PATH_COUNTRY, PATH_PROVINCE, PATH_DEPARTMENT);
	freeCensus(c);
	
	return 0;
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
	int size=0,option=0, errorStatus=1, readHomeID=0, c;
	*status = 0;
	//Sets department string pointer at the beginning of the s string.
	*dept = s;

	while(size<MAX_LINE && (c=getchar())!='\n' && c!=EOF){
		switch(option){

			//Builds status number until it reads a comma, then switches to case HOME_ID.
			//If the first character read is a comma, status number exceeds its maximun value,
			//or any character is invalid, it switches to ERROR instead.
			case ECONOMIC_STATUS:
				if (isdigit(c)){
					*status = (*status)*10 + c - '0';
					if (*status > STATUS_MAX_VALUE){
						option = ERROR;
						errorStatus = -1;
					}
				}
				else if (c==','&& size>0){
					option=HOME_ID;
				}
				else{
					errorStatus=-1;
					option=ERROR;
				}
				break;

			//Validates that HOME ID is composed of digits, otherwise switches to case ERROR.
			//If it reads a comma before any digit, also switches to case ERROR. 
			//Switches to case DEPT if it reads a comma after digits.
			case HOME_ID:
				if (isdigit(c))
					readHomeID=1;
				else if (c==',' && readHomeID)
					option=DEPT;
				else {
					errorStatus=-2;
					option=ERROR;
				}
				break;						
			
			//Copies to string s valid characters until it reads a comma, then finishes *dept
			//string, sets *prov string in the following position and switches to case PROV.
			//If *dept string and s string are equal, nothing was copied yet. If it reads 
			//a comma before copying anything, or character is not valid, switches to case ERROR.
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

			//Copies to string s valid characters until end of line. 
			//If any character is invalid it switches to case ERROR.
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

