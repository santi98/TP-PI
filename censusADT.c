#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "censoTAD.h"



struct deptCDT{
	char * title;
	long totalOc, totalDc, total;
	struct deptCDT * next;
};

struct provCDT{
	char * title;
	long totalOc, totalDc, total;
	struct deptCDT * first;
};

struct censusCDT{
	long totalOc, totalDc, total;
	int totalProv;
	struct provCDT *  provinces;
};


censusADT newCensus(int totalProv, char ** provList){

	censusADT c = malloc(sizeof(censusADT));

	if(c!=NULL){

		c->total = 0;
		c->totalOc = 0;
		c->totalDc = 0;	
		c->provinces = malloc(sizeof(provCDT)*totalProv);
		if(c->provinces == NULL)
			return NULL;

		for(int i = 0; i<totalProv; i++){
			c->provinces[i].title = malloc(strlen(provList[i]) + 1);

			if(c->provinces[i].title == NULL)
				return NULL;
			
			strcpy(c->provinces[i].title, provList[i]);

			c->provinces[i].totalOc = 0;
			c->provinces[i].totalDc = 0;
			c->provinces[i].total = 0;
			c->provinces[i].first = NULL;
		}		
	}

	return c;
}


int addElem(censusADT c, int state, char *dept, char *prov){
	int i, aux;
	int foundProv = 0;
	for(i=0; i<c->totalProv && !foundProv; i++){
		if((aux = strcmp(c->provinces[i].title, prov)) == 0)
			foundProv = 1;

		//As the the province string is ordered alfabetically, if it is alfabetically greater 
		//than the province name passed as an argument it exits the for loop as that means the
		//province name is not valid.
		else if(aux > 0)
			break;
	}
	if(foundProv)
		addDptoRec(c->provinces[i].first, state, dept, &foundProv);

	return foundProv;
}

static struct deptCDT * addDptoRec(struct deptCDT * d, int state, char* dept, int * flag){
	int c;
	if(d==NULL || (c=strcmp(d->title, dept))>0){
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
		if(state>4)

	}
}
