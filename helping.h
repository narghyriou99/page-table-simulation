#ifndef HELPING_H_
#define HELPING_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <time.h>


#define TRACE_FILE1 "./trace_files/bzip.trace"
#define TRACE_FILE2 "./trace_files/gcc.trace"

#define PAGE_SIZE 4096
#define MAX_TRACES 20000

typedef struct PageEntry {	//Node
	unsigned int page_number;
	int pid;
	char mode;
	int count, R, dirtyBit;
	struct PageEntry *next;
}PE;


typedef struct Element {
	PE* pe;
}Element;

typedef struct PageTable {
	int p_faults, hits, reads, writes, maxFrames, usedFrames;
	char* algorithm;
	Element* el;
}PT;

PT *initializeTable(int maxFrames, char *algorithm);
void delete(PT *pt);
void printStatistics(PT* pt);
void trace(PT *pt,int pid , unsigned int pageNumber, char mode, int count);
int pageEntry(PT *pt, int pid,  unsigned int pageNumber, char mode, int count);
int hash(unsigned int pageNumber, int maxFrames);
int pageExists(PT *pt, unsigned int pageNumber);
void findReplacePage(PT *pt, int pid, unsigned int pageNumber, char mode, int count);
#endif /* HELPING_H_ */