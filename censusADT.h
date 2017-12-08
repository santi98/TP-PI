#ifndef __listADT_h_
#define __listADT_h_

typedef struct censusCDT * censusADT;

censusADT newCensus(void);

void freeCensus(censusADT c);

//Adds data to the census. 
//Returns 0 if there's no available memory.
//Returns 1 if the data was added correctly.
int addElem(censusADT c, int status, char *dept, char *prov); 

/*
	Stores the data from the census into 3 csv files.
	
	#Country.csv
		One line including the total amount of citizens and the unemployment index.
	#Province.csv
		One line per province, ordered alfabetically including the total amount of
		citizens and the unemployment index.
	#Department.csv
		One line per department ordered alfabetically first by province then by
		department, including the total amount of citizens and the unemployment
		index.

	Returns 0 in case of error and 1 if it excecuted correctly.

*/

int storeToFiles(censusADT c, char *pathCountry, char * pathProvince, char * pathDepartment);

#endif