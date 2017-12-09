#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "censusADT.h"

#define TRUE 1
#define FALSE 0

//If there are no occupied or deoccupied citizens, the unemployment index will be 0.

//#define UNEMP_INDX(o,dc) (dc)>0?(((double)(dc))/((o)+(dc))):(dc)

#define UNEMP_INDX(a, b) ((b)>0?(double)(b)/((a)+(b)):(b))

/*
	deptCDT is the structure corresponding to departments, it holds the department name,
	data regarding the amount of citizens according to economic status and a pointer
	to the next deptCDT, making a NULL terminated and alfabeticallly ordered list.
*/
struct deptCDT{
	char * title;
	long totalOc, totalUo, total;
	struct deptCDT * next;

};

/*
	Similar to deptCDT, provCDT corresponds to provinces and holds the province
	name and the data regarding citizens and their status.
	It also holds a pointer to the first element of a list of the departments in that province,
	as well as an iterative pointer to that list which makes going over it faster.
	It also holdsa pointer tothe next provCDT, making a NULL terminated and alfabeticallly ordered list.
*/
struct provCDT{
	char * title;
	long totalOc, totalUo, total;
	struct deptCDT * firstD;
	struct deptCDT *iterD;

	struct provCDT * next;

};

/*
	Similar to deptCDT and provCDT, censusCDT corresponds to the country and holds data regarding citizens
	and their status.
	The variables occupied and unoccupied hold the values corresponding to the economic status indicators
	for occupied and unoccupied citizens.
	It also holds a pointer to the first element of a list of the provinces in that country,
	as well as an iterative pointer to that list which makes going over it faster.
	
*/
struct censusCDT{
	long totalOc, totalUo, total;
	int occupied, unoccupied;
	struct provCDT * firstP;
	struct provCDT * iterP;
};


//Adds to the census, province and dept counters according to economic status.
static void addTotals(censusADT c, int status);

//Adds a department to the list of departments in a province recursively.
static struct deptCDT * addDeptRec(struct deptCDT * d, int status, char* dept, censusADT c, int * flag);

//Adds a province to the list of provinces in the census recursively.
static struct provCDT * addProvRec(struct provCDT * p, int status, char* prov, censusADT c, int * flag);

//Frees memory used by departments recursively.
static void freeDeptRec(struct deptCDT * d);

//Frees memory used by provinces recursively.
static void freeProvRec(struct provCDT * p);

//Auxiliary function used in storeProvincesAndDepartments.
static int storeDepartments(struct provCDT * p, FILE * fileDepartment);

//Auxiliary function used in storeToFile.
static int storeProvincesAndDeptartment(struct provCDT * p, FILE * fileProvince, FILE * fileDepartment);

//Auxiliary function used in storeToFile.
static int storeCountry(censusADT c, FILE * fileCountry);

//Creates a new census.
censusADT newCensus(int occupied, int unoccupied){

	censusADT c = malloc(sizeof(*c));

	if(c!=NULL){

		c->total = 0;
		c->totalOc = 0;
		c->totalUo = 0;	
		c->firstP = NULL;
		c->iterP = NULL;
		c->occupied = occupied;
		c->unoccupied = unoccupied;		
	}

	return c;
}

/*
Adds a department to its corresponding list in the list of provinces. If the province does
not exist, it adds the province to the list of provinces and creates a new list for 
the department and adds it. It also keeps track of the citizens according to their
economic status by propper use of the addTotals function.
*/

int addElem(censusADT c, int status, char *dept, char *prov){
	int fail = FALSE;
	if((c->iterP)!= NULL && strcmp(c->iterP->title, prov) <= 0){
		addProvRec(c->iterP, status, prov, c, &fail);
	}
	else 
		c->firstP = addProvRec(c->firstP, status, prov, c, &fail);
	
	if(fail == FALSE){
		if(c->iterP->iterD!= NULL && strcmp(c->iterP->iterD->title, dept) <= 0)
			addDeptRec(c->iterP->iterD, status, dept, c, &fail);
		else
			c->iterP->firstD = addDeptRec(c->iterP->firstD, status, dept, c, &fail);
	}

	return !fail;
	
}

static struct deptCDT * addDeptRec(struct deptCDT * d, int status, char* dept, censusADT c, int * flag){
	int f;
	if(d==NULL || (f=strcmp(d->title, dept))>0){
		struct deptCDT * aux = malloc(sizeof(*aux));
		if(aux == NULL){
			*flag = 0;
			return d;
		}

		aux->title = malloc(strlen(dept)+1);
		if(aux->title == NULL){
			*flag = 0;
			free(aux);
			return d;
		}

		aux->total=0;
		aux->totalOc=0;
		aux->totalUo=0;
		aux->next = d;

		c->iterP->iterD = aux;
		strcpy(c->iterP->iterD->title, dept);
		addTotals(c, status);
		
		return aux;
	}
	if(f == 0){
		c->iterP->iterD = d;
		addTotals(c, status);
		return d;
	}

	d->next = addDeptRec(d->next, status, dept, c, flag);
	return d;

}

static struct provCDT * addProvRec(struct provCDT * p, int status, char* prov, censusADT c, int * flag){
	int f;
	if(p==NULL || (f=strcmp(p->title, prov))>0){
		struct provCDT * aux = malloc(sizeof(struct provCDT));
		if(aux==NULL){
			*flag = 0;
			return p;
		}

		aux->title = malloc(strlen(prov)+1);
		if(aux->title == NULL){
			*flag = 0;
			free(aux);
			return p;
		}

		aux->total=0;
		aux->totalOc=0;
		aux->totalUo=0;
		aux->next = p;
		c->iterP = aux;

		strcpy(c->iterP->title, prov);
		aux->firstD = NULL;
		aux->iterD = NULL;
		return aux;
	}
	if(f==0){
		c->iterP = p;
		return p;
	}
	p->next = addProvRec(p->next, status, prov, c, flag);
	return p;
}

static void addTotals(censusADT c, int status){
	if(status == c->occupied){
			c->totalOc++;
			c->iterP->totalOc++;
			c->iterP->iterD->totalOc++;	
		}
	else if(status == c->unoccupied){
		c->totalUo++;	
		c->iterP->totalUo++;	
		c->iterP->iterD->totalUo++;	
	}
	c->total++; 
	c->iterP->total++;
	c->iterP->iterD->total++;
}

//Frees memory used by the census.
void freeCensus(censusADT c){
	freeProvRec(c->firstP);
	free(c);
	return;
}

static void freeProvRec(struct provCDT * p){
	if(p==NULL)
		return;
	freeProvRec(p->next);
	freeDeptRec(p->firstD);
	free(p->title);
	free(p);
	return;
}

static void freeDeptRec(struct deptCDT * d){
	if (d==NULL)
		return;
	freeDeptRec(d->next);
	free(d->title);
	free(d);
	return;
}

/*
	Stores values from the census into 3 files.
	#fileCountry
		Holds the total population of the country as well as the unemployment
		index as comma separated values.
	#fileProvince
		Ordered alfabetically.
		Each line corresponds to a province and hold the province name,
		the total population of the province and the unemployment index
		of the province as comma separated values.
	#fileDepartment
		Ordered alfabetically first by province name, then by department name.
		Each line corresponds to a department and holds the province name the 
		department is included in, the department name, the total population of the 
		department and the unemployment index of the department as comma separated
		values.
		
	Returns 0 if the files could not be created or if there was a problem while adding
	a line.
	Returns 1 if the files were created and the data stored properly.
	
*/

int storeToFiles(censusADT c, char *pathCountry, char * pathProvince, char * pathDepartment){
	FILE * fileCountry = fopen(pathCountry, "w");
	FILE * fileProvince = fopen(pathProvince, "w");
	FILE * fileDepartment = fopen(pathDepartment, "w");

	int flag;

	if(fileCountry == NULL || fileProvince == NULL || fileDepartment == NULL){
		if(fileCountry != NULL)
			fclose(fileCountry);
		if(fileProvince != NULL)
			fclose(fileProvince);
		if(fileDepartment != NULL)
			fclose(fileDepartment);
		return 0;
	}

	flag = storeCountry(c, fileCountry);
	flag = storeProvincesAndDeptartment(c->firstP, fileProvince, fileDepartment);

	fclose(fileCountry);
	fclose(fileProvince);
	fclose(fileDepartment);

	return flag;
}

static int storeCountry(censusADT c, FILE * fileCountry){
	int errorStatus;
	errorStatus = fprintf(fileCountry, "%ld,%.2f\n", c->total, UNEMP_INDX(c->totalOc, c->totalUo));

	return errorStatus>0;
}

static int storeProvincesAndDeptartment(struct provCDT * p, FILE * fileProvince, FILE * fileDepartment){
	int errorStatus;
	struct provCDT * aux = p;
	while (aux!=NULL){
		errorStatus=fprintf(fileProvince, "%s,%ld,%.2f\n",aux->title, aux->total, UNEMP_INDX(aux->totalOc, aux->totalUo));
		storeDepartments(aux, fileDepartment);
		aux = aux->next;
	}
	return errorStatus>0;
}

static int storeDepartments(struct provCDT * p, FILE * fileDepartment){
	int errorStatus;
	struct deptCDT * aux = p->firstD;
	while (aux!=NULL){
		errorStatus = fprintf(fileDepartment, "%s,%s,%ld,%.2f\n", p->title, aux->title, aux->total, UNEMP_INDX(aux->totalOc, aux->totalUo));
		aux = aux->next;
	}
	return errorStatus>0;
}
