#ifndef __listADT_h_
#define __listADT_h_

typedef struct censusCDT * censusADT;

//Returns NULL if the new census could not be created due to unavailable memory.
censusADT newCensus(int occupied, int unocuppied);

//Frees the memory used by te census structure.
void freeCensus(censusADT c);

//Adds data to the census. 
//Returns 0 if there's no available memory.
//Returns 1 if the data was added correctly.
int addElem(censusADT c, int status, char *dept, char *prov); 

/*
	Stores values from the census into 3 files.
	pathCountry, patchProvince and pathDepartment should be paths to .csv files
	where the data will be stored. If the files exist they will be overwritten 
	otherwise they will be created.
	
	#pathCountry
		Holds the total population of the country as well as the unemployment
		index as comma separated values.
	#pathProvince
		Ordered alfabetically.
		Each line corresponds to a province and hold the province name,
		the total population of the province and the unemployment index
		of the province as comma separated values.
	#pathDepartment
		Ordered alfabetically first by province name, then by department name.
		Each line corresponds to a department and holds the province name the 
		department is included in, the department name, the total population of the 
		department and the unemployment index of the department as comma separated
		values.
		
	Returns 0 if the files could not be created or if there was a problem while adding
	a line.
	Returns 1 if the files were created and the data stored properly.
	
*/
int storeToFiles(censusADT c, char *pathCountry, char * pathProvince, char * pathDepartment);

#endif
