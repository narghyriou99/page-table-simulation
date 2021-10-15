#include "helping.h"

PT* initializeTable(int maxFrames, char *algorithm){
	PT *ptr = malloc(sizeof(PT));	//Aloc
	ptr->p_faults = 0;
	ptr->hits = 0;
	ptr->reads = 0;
	ptr->writes = 0;
	ptr->usedFrames = 0;
	ptr->maxFrames = maxFrames;
	ptr->algorithm = malloc(strlen(algorithm) + 1);
	strcpy(ptr->algorithm, algorithm);
	ptr->el = (Element *)malloc(maxFrames * sizeof(Element));
	memset(ptr->el, 0, maxFrames * sizeof(Element));
	return ptr;
}



void delete(PT* pt) {
	free(pt->algorithm);
	for (int i = 0; i < (pt->maxFrames); i++) {
		PE *cur = pt->el[i].pe, *next = NULL;
		while (cur != NULL) {
			next = cur->next;
			free(cur);
			cur = next;
		}
	}
	free(pt->el);
	free(pt);
}
	


void printStatistics(PT *pt){
	
	for(int i=0; i<(pt->maxFrames); i++){
		printf("Element %d\n",i);
		PE* cur = pt->el[i].pe;
		while (cur != NULL) {
			printf("PageNumber: %x ", cur->page_number);
			printf("Mode: %c ", cur->mode);
			printf("Pid: %d\n", cur->pid);
			cur = cur->next;
		}	
		printf("--------------------------\n");
	}
	printf("Printing statistics...\n");
	printf("%d reads, %d writes\n", pt->reads, pt->writes);
	printf("%d hits, %d page faults\n", pt->hits, pt->p_faults);
	printf("%s is used\n", pt->algorithm);
}




void trace(PT *pt, int pid, unsigned int pageNumber, char mode, int count){
	int res = pageExists(pt, pageNumber);
	if(res < 0){	//Page does not exist
		int res2 = pageEntry(pt, pid, pageNumber, mode, count);
		if(res2 < 0){	//Page cannot be saved / Replacement
			findReplacePage(pt, pid, pageNumber, mode, count);	//LRU or SCA
		}
		pt->reads++;
		pt->p_faults++;
	}else{	//Page exists
		pt->hits++;
		int index = hash(pageNumber, pt->maxFrames);
		PE *cur = pt->el[index].pe;
		while(cur != NULL){
			if(cur->page_number == pageNumber){
				cur->count = count; //Timestamp
				cur->R = 1;
				if (mode == 'W')
					cur->dirtyBit = 1;
				return;
			}
			cur = cur->next;
		}
	}
}


int pageEntry(PT *pt, int pid, unsigned int pageNumber, char mode, int count){
	if(pt->usedFrames < pt->maxFrames){	// Exei xwro
		int index = hash(pageNumber, pt->maxFrames);	//Hash value
		PE* top = pt->el[index].pe, *tmp;
		tmp = malloc(sizeof(PE));
		tmp->page_number = pageNumber;
		tmp->mode = mode;
		tmp->next = NULL;
		tmp->R = 0;
		tmp->count = count;
		tmp->pid = pid;
		tmp->dirtyBit = 0;
		if (mode == 'W')
			tmp->dirtyBit = 1;
		tmp->next = top;
		top = tmp;
		pt->el[index].pe = top;
		pt->usedFrames++;
		return index;
	}else{
		return -1;	//Its time for LRU or SCA
	}
}

int hash(unsigned int pageNumber, int maxFrames){
	return pageNumber % maxFrames;
}

int pageExists(PT *pt, unsigned int pageNumber){
	PE *cur = pt->el[hash(pageNumber, pt->maxFrames)].pe;
	while(cur != NULL){
		if(cur->page_number == pageNumber){
			return 1;
		}
		cur = cur->next;
	}
	return -1;
}

void findReplacePage(PT *pt, int pid, unsigned int pageNumber, char mode, int count){
	if(!strcmp(pt->algorithm, "LRU")){	//LRU
		int min = INT_MAX;
		int pos;
		for(int i=0; i<(pt->maxFrames); i++){	//Find min
			PE *cur = pt->el[i].pe;
			while (cur != NULL){
				if(cur->count < min){
					min = cur->count;
					pos = i;
				}
				cur = cur->next;
			}	
		}

		//Replacement
		
		PE *cur = pt->el[pos].pe;
		PE *prev =NULL, *temp = cur;
		if(cur->count == min){	//Head node
			if(cur->dirtyBit == 1)
				pt->writes++;	
			if (cur->next == NULL) {
				free(cur);
				pt->el[pos].pe = NULL;
			}
			else {
				cur = temp->next;
				free(temp);
				pt->el[pos].pe = cur;
			}
		}else{					//Inner node
			while (cur != NULL){
				if(cur->count == min){
					if (cur->dirtyBit == 1)
						pt->writes++;
					break;
				}
				prev = cur;
				temp = cur;				
				cur = cur->next;
			}
			prev->next = cur->next;
			free(cur);
		}
		
		//Add the new page

		int index = hash(pageNumber, pt->maxFrames);	//Hash value
		PE* top = pt->el[index].pe, * tmp;
		tmp = malloc(sizeof(PE));
		tmp->page_number = pageNumber;
		tmp->mode = mode;
		tmp->next = NULL;
		tmp->R = 0;
		tmp->count = count;
		tmp->pid = pid;
		tmp->dirtyBit = 0;
		if (mode == 'W')
			tmp->dirtyBit = 1;
		tmp->next = top;
		top = tmp;
		pt->el[index].pe = top;
		
	}
	else {	//SCA

		int min = INT_MAX;
		int pos;
		PE* temp = NULL;
		for (int i = 0; i < (pt->maxFrames); i++) {	//Find min
			PE* cur = pt->el[i].pe;
			while (cur != NULL) {
				if (cur->count < min) {
					min = cur->count;
					pos = i;
					temp = cur;
				}
				cur = cur->next;
			}
			
		}
		if (temp->R == 0) {
			//delete and insert the new one
			PE* cur = pt->el[pos].pe;
			PE* prev = NULL, * temp = cur;
			if (cur->count == min) {	//head node
				if (cur->dirtyBit == 1)
					pt->writes++;
				if (cur->next == NULL) {
					free(cur);
					pt->el[pos].pe = NULL;
				}
				else {
					cur = temp->next;
					free(temp);
					pt->el[pos].pe = cur;
				}
			}
			else {					//inner node
				while (cur != NULL) {
					if (cur->count == min) {
						if (cur->dirtyBit == 1)
							pt->writes++;
						break;
					}
					prev = cur;
					temp = cur;
					cur = cur->next;
				}
				prev->next = cur->next;
				free(cur);
			}
			pt->usedFrames--;
			pageEntry(pt, pid, pageNumber, mode, count);

		}else {
			temp->R = 0;
			//temp->count = count;
			//Paei sto telos
			if (temp->next != NULL) {
				PE* cur = pt->el[pos].pe;
				PE* prev = NULL, * temp = cur;
				PE* tmp2 = cur;
				if (cur->count == min) {	//head node
					if (cur->dirtyBit == 1)
						pt->writes++;
					if (cur->next != NULL) {
						cur = temp->next;
						tmp2 = temp;
						//free(temp);
						pt->el[pos].pe = cur;
					}
				}
				else {					//inner node
					while (cur != NULL) {
						if (cur->count == min) {
							if (cur->dirtyBit == 1)
								pt->writes++;
							break;
						}
						prev = cur;
						temp = cur;
						cur = cur->next;
					}
					prev->next = cur->next;
					tmp2 = cur;
					//free(cur);
				}
				//To vazw sto telos
				cur = pt->el[pos].pe;
				while (cur->next != NULL) {
					cur = cur->next;
				}
				tmp2->next = NULL;
				cur->next = tmp2;
			}
			findReplacePage(pt, pid, pageNumber, mode, count);
		}


	}
	return;
}
