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

struct deptCDT{
	char * title;
	long totalOc, totalUo, total;
	struct deptCDT * next;

};

struct provCDT{
	char * title;
	long totalOc, totalUo, total;
	struct deptCDT * firstD;
	struct deptCDT *iterD;

	struct provCDT * next;

};

struct censusCDT{
	long totalOc, totalUo, total;
	int occupied, unoccupied;
	struct provCDT * firstP;
	struct provCDT * iterP;
};


//Adds to the census, province and dept counters according to economic status.
static void addTotals(censusADT c, int status);

//Adds a department to the list of departments in a province.
static struct deptCDT * addDeptRec(struct deptCDT * d, int status, char* dept, censusADT c, int * flag);

//Adds a province to the list of provinces in the census.
static struct provCDT * addProvRec(struct provCDT * p, int status, char* prov, censusADT c, int * flag);

static void freeDeptRec(struct deptCDT * d);

static void freeProvRec(struct provCDT * p);

static int storeDepartments(struct provCDT * p, FILE * fileDepartment);

static int storeProvincesAndDeptartment(struct provCDT * p, FILE * fileProvince, FILE * fileDepartment);

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
