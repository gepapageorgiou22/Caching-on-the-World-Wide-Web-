//------ Include Files-----
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

//---- Define Values----
#define SIZE 250             // Size of elements to create
#define theta 5            // Theta for zipfian distribution
#define min 1              // Min item size in units
#define max 1000000        // Max item size in units
#define Cachepercent 0.05   // Cache percent of total size
#define asking_number 10000   // Numbers to ask the cache
#define file_sketch 1    // Number for sketch the numbers near min file size


//= Function prototypes
void IDs(int array[2][SIZE]);                                                               // Set random IDs
void zipfian(int array[2][SIZE]);                                                           // Zipfian distributed file sizes
void asking_numbers(int IDs[]);                                                             // Main array as files in memory and IDs are the ids he is going top ask for.
int cacheSize(int array[2][SIZE]);                                                          // Find cache size
int FindelEmentsInCache(int *ptr, int CSize);                                               // Count how many elements are in cache
int *FindinCache(int *cache, int element, int CSize);                                       // Find Element in cache NULL if not exists
int cacheleft(int *cache, int CSize);                                                       // Compute how much size we have left
void write_dt(int DT[2][SIZE],int element,int ITcounter);                                   // Write Dt when its not in dt
void find_and_write_dt(int DT[2][SIZE],int element,int ITcounter);                          // Write dt when it already exists
void multiply(int array[2][SIZE], int DT[2][SIZE], int multiplied[2][SIZE], int ITcounter); // Function to do multiplies between Size and Dt
void kick(int DT[2][SIZE], int multiplied[2][SIZE],int *cache, int element_size, int Rleft, int CSize, int array[2][SIZE]);
void cache_fix(int *cache, int element_at_max, int CSize);


//=========================
//=                       =
//=       Driver          =
//=                       =
//=========================
int main(int argc, char *argv[]){
    int counter;
    int array[2][SIZE];         // main array with stored data
    int asking_IDs[asking_number]; //array of items to ask
    int CSize;  //cache size
    int *cache; //pointer to cache
    int toobig=0, miss=0, hit=0; //too big to get into cache
    int asked_id; //id randomly asked
    int ITcounter; //Counter to see when we access something
    int el; //count of elements in cache
    int *found; //pos of element
    int R; //Size of cache left
    int *PosToWrite; //Where to write
    int Dt[2][SIZE]; //Dt values array
    int multiplied[2][SIZE];
    int temp;
    
    srand(time(NULL));
    
    //Set array to 0
    for(counter=0; counter<SIZE; counter++){
        Dt[0][counter]=0;
        Dt[1][counter]=0;
        multiplied[0][counter]=0;
        multiplied[1][counter]=0;
    }
    
    
    IDs(array);                   //Random IDs
    zipfian(array);
    
    asking_numbers(asking_IDs);
    
        
    CSize=cacheSize(array); //Cache size
    temp=CSize;
    CSize=(int)CSize*Cachepercent;
    
    
    cache = (int *)malloc((CSize)*sizeof(int)); //Allocating memory
    if(cache==NULL){ //Check for error
        printf("Error allocating memory!\nExiting...\n");
        exit(-1);
    }
    
    for(ITcounter=0; ITcounter<asking_number; ITcounter++){
        
        asked_id = asking_IDs[ITcounter];
        el=FindelEmentsInCache(cache, CSize); //find how many elements are in cache
        
        if(array[1][asked_id-1]>=CSize){
            //printf("Not worth to get it into the cache\n");
            toobig++;
            multiply(array, Dt, multiplied, ITcounter);
            continue;
        }
        
        //search if exists in cache
        found=FindinCache(cache, asked_id, CSize);
        if(found==NULL){
           R=cacheleft(cache, CSize);
           if(R>array[1][asked_id-1]){
              PosToWrite=cache; //start from the beggining of the cache  
              // fint pointer of last element and write the asked element in cache
                while(*PosToWrite!=0){
                    PosToWrite++;
                }
                *PosToWrite=array[1][asked_id-1]; //Write size here
                for(counter=0; counter<array[1][asked_id-1]; counter++){
                    PosToWrite = PosToWrite + 1;
                    *PosToWrite = asked_id; //write value
                }
                write_dt(Dt, asked_id, ITcounter);
           }
           else{
                kick(Dt, multiplied, cache, array[1][asked_id-1], R, CSize, array);
            }
            miss++;
        }
        else if(found != NULL){
            find_and_write_dt(Dt, asked_id, ITcounter);
            hit++;
        }
    //printf("IT counter: %d\n", ITcounter);
    multiply(array, Dt, multiplied, ITcounter);
    
    }
    
    for(counter=0; counter< CSize; counter++){
        printf("%d ", *(cache+counter));
    }
    
    free(cache);// Free memory allocated
    
   
    
    printf("Array Ids:");
    for(counter=0; counter<SIZE; counter++){
        printf(" %d,", array[0][counter]);
    }
    
    printf("Array size: ");
    for(counter=0; counter<SIZE; counter++){
    printf(" %d,", array[1][counter]);
    }
    
    printf("\n Asked numbers:");
    
    for(counter=0; counter<asking_number; counter++){
    printf(" %d,", asking_IDs[counter]);
    }
    
    printf("We had %d hits!\nWe had %d misses!\nAnd total elemnts that we did not get into the cache were %d\n", hit, miss, toobig);
    printf("Total element size is: %d\nwhile cache size is %d\n", temp, CSize);
    
    
    return(0);
}









//----Functions----

//Create IDs in range of SIZE(working)
void IDs(int array[2][SIZE]){
    int counter; //Counter for loop
    
    for(counter=1; counter<SIZE+1; counter++){
        array[0][counter-1]=counter;
    }
}

//Random zipfian for file size(working)
void zipfian(int array[2][SIZE]){
    
    int counter;
    double probs[SIZE];
    double sum=0;
    double temp, power;
    
    // Create probs
    for(counter=0; counter<SIZE; counter++){
        temp=(double) 1/(double)array[0][counter];
        power=pow(temp, theta);
        probs[counter]=power;
        sum=sum+probs[counter];
    }
    
    for(counter=0; counter<SIZE; counter++){
        probs[counter]=probs[counter]/sum;
    }
    
    //Size equal to a plus prob*b
    for(counter=0; counter<SIZE; counter++){
        array[1][counter]=(int)(min + ((max/file_sketch)*probs[counter])*rand()/RAND_MAX);
    }
    
}

//Function asks for random ids(working)
void asking_numbers(int IDs[]){
    int counter;  // Counter for loop to create random IDs in range of SIZE
    int rid;      // Random ID to ask the cache
    
    for(counter=0; counter<asking_number; counter++){
        rid = (int)(rand()/(float)RAND_MAX * (float)SIZE);
        if(rid==0){
            if(counter>=0){
                counter--;
            }
        }
        else{
            IDs[counter]=rid;
        }
    }

}

//Find cache size to allocate(Working properly)
int cacheSize(int array[2][SIZE]){
    int counter;//counter
    int size=0;//Size
    
    for(counter=0; counter<SIZE; counter++){
        size = size + array[1][counter];
    }
    
    return(size);

}

//Return number of elements in cache direct(working)
int FindelEmentsInCache(int *ptr, int CSize){
    
    int count=0; //number of elements in cache
    int *loop;
    
    loop=ptr;
    //pointer aritmetic to go throu the whole cache
    while(loop <= ptr+CSize ){
        if(*loop==0){break;}
        count++;
        loop=loop + 1 + *loop;
    }
    
    return(count);
}  


//return position in cache(Working)
int *FindinCache(int *cache, int element, int CSize){
    int size; //element size
    int *pos; //position
    
    pos=cache;
    
    
    
    while(pos<cache+CSize){
        size=*pos;
        pos++;
        
        if(*pos==element){
            return(pos);
        }
        else{
            pos=pos+size;
        }
    }
    
    return(NULL);
}

//Free storage left(Working)
int cacheleft(int *cache, int CSize){
    int size=0;
    int *pos;
    
    pos=cache;
    
    while(pos<cache+CSize){
        
        size = size + *pos;
        pos = pos + *pos;
        if(*pos==0){break;}
        
    }
    
    return(CSize-size);
}

//Writing dt's
void write_dt(int DT[2][SIZE],int element,int ITcounter){
    int counter;

    for(counter=0; counter<SIZE; counter++){
        if(DT[0][counter]==0){
            DT[0][counter]=element;
            DT[1][counter]=ITcounter;
            return;
        }
    }
    
}

// Update dt's
void find_and_write_dt(int DT[2][SIZE],int element,int ITcounter){
    int counter;
    
    for(counter=0; counter<SIZE; counter++){
        if(DT[0][counter]==element){
            DT[1][counter]=ITcounter;
            return;
        }
    }
}


void multiply(int array[2][SIZE], int DT[2][SIZE], int multiplied[2][SIZE], int ITcounter){
    int counter;
    
    for(counter=0; counter<SIZE; counter++){
        if(DT[0][counter]==0){
            return;
        }
        multiplied[0][counter]=DT[0][counter]; //Write element's id
        multiplied[1][counter]=(ITcounter-DT[1][counter])*array[1][(DT[0][counter])-1]; // compute size*dt
    }
}

// fix multiplied, dt and cache 
void kick(int DT[2][SIZE], int multiplied[2][SIZE],int *cache, int element_size, int Rleft, int CSize, int array[2][SIZE]){
    int counter, counter1;
    int size_have=0;
    int size, max_sdt, element_at_max;
    
    size_have=Rleft;
    
    do{
        //Find bigest s*dt at multiplied 
        max_sdt=multiplied[1][0];
        element_at_max=multiplied[0][0];
        for(counter=1; counter<SIZE; counter++){
            if(max_sdt<multiplied[1][counter]){
                max_sdt=multiplied[1][counter];
                element_at_max=multiplied[0][counter];
            }
        }
    
        //erase item from multiplied
        for(counter=0; counter<SIZE; counter++){
            if(multiplied[0][counter]==element_at_max){
                multiplied[0][counter]=0;
                multiplied[1][counter]=0;
                break;
            }
        }
    
        //fix multiplied array
        for(counter=0; counter<SIZE; counter++){
            if(multiplied[0][counter]==0){
                if(counter==SIZE-1){
                    break;
                }
                else if(multiplied[0][counter+1]!=0){
                    for(counter1=counter; counter1<SIZE; counter1++){
                        if(counter1==SIZE-1){
                            break;
                        }
                        if(multiplied[0][counter1+1]==0){
                            break;
                        }
                        multiplied[0][counter1]=multiplied[0][counter1+1];
                        multiplied[1][counter1]=multiplied[1][counter1+1];
                    }
                }
            }
        }
    
        size=array[1][element_at_max-1];
    
        //erase item from DT
        for(counter=0; counter<SIZE; counter++){
            if(DT[0][counter]==element_at_max){
                DT[0][counter]=0;
                DT[1][counter]=0;
                break;
            }
        }
        
        // Fix DT array
        for(counter=0; counter<SIZE; counter++){
            if(DT[0][counter]==0){
                if(counter==SIZE-1){
                    break;
                }
                else if(DT[0][counter+1]!=0){
                    for(counter1=counter; counter1<SIZE; counter1++){
                        if(counter1==SIZE-1){
                            break;
                        }
                        if(DT[0][counter1+1]==0){
                            break;
                        }
                        DT[0][counter1]=DT[0][counter1+1];
                        DT[1][counter1]=DT[1][counter1+1];
                    }
                }
            }
        }
        
        //fix cache now
        cache_fix(cache, element_at_max, CSize);

        size_have=size_have+size;
    }
        
    while(size_have<=element_size);
    
}

void cache_fix(int *cache, int element_at_max, int CSize){
    int *pos, *ptr;    
    int counter, size;
    
    pos=cache;
    
    while(pos<cache+CSize){
        size=*pos;
        pos++;
        
        if(*pos==element_at_max){
            break;
        }
        else{
            pos=pos+size;
        }
    }
        
    pos=pos-1;// go to size
    size=*pos;
    if(pos+size==cache+CSize){return;}
    ptr=pos+size+1;
    if(*ptr==0){return;}
    
    while(ptr<cache+CSize){
        if(*ptr==0){return;}
        for(counter=0;counter<*ptr; counter++){
            *(pos+counter)=*(ptr+counter);
        }
        ptr=ptr+*ptr+1;
    }
    
}

















