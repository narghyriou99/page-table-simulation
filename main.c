#include "helping.h"

int main(int argc, char *argv[]){

    if(argc < 4){
        printf("Give more arguments\n");
        return -1;
    }
    
    int max_frames = atoi(argv[2]);   
    int q = atoi(argv[3]);
    int max_traces;

    if(argc == 5){
        max_traces = atoi(argv[4]);
    }else{
        max_traces = MAX_TRACES;
    }
    
    FILE *f1, *f2;
    unsigned int address;    //Address of memory
    char mode;      //R or W
    int i=0, s=0;
    

    if((f1 = fopen(TRACE_FILE1, "r")) == NULL){
        printf("File 1 open error\n");
        return -1;
    }

    if((f2 = fopen(TRACE_FILE2, "r")) == NULL){
        printf("File 2 open error\n");
    }

    
    PT* pt = initializeTable(max_frames, argv[1]);   //Init
    

    while(i<max_traces){
        switch (s)
        {
        case 0: // pid 1
            fscanf(f1, "%8x %c", &address, &mode);
            address = address >> 12;
            trace(pt, 1, address, mode, i);
            i++;
            q--;
            break;
        
        case 1: //pid 2
            fscanf(f2, "%8x %c", &address, &mode);
            address = address >> 12;
            trace(pt, 2, address, mode, i);
            i++;
            q--;
            break;
        }
        if (!q) {
            s = !s;
            q = atoi(argv[3]);
        }
    }
    /*-----------------------------------*/
    printStatistics(pt);
    printf("Frames =  %d, q = %d, Traces = %d\n", max_frames, atoi(argv[3]), max_traces);
    /*-----------------------------------*/

    delete(pt);
    fclose(f1);
    fclose(f2);  
    return 0;
}