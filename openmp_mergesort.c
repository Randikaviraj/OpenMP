/* C Program to to merge sort of a list in ascending order 
Note that this only supports lists that are powers of 2 
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

//size of the list
#define SIZE 16*16

void merge(float *list,int l,int m,int r);
void mergesort(float *list);

//check whether a certain number is a power of 2
int isPowerOfTwo(int num){
	int i=0;
	int val=1;
	for(i=0;val<=num;i++){
		if((val=pow(2,i))==num){
			return 1;
		}
		
	}				
	return 0;	

}

int main(){
	
	//check the condition that check that checks whether the size is a power of 2
	if(!isPowerOfTwo(SIZE)){
		fprintf(stderr,"This implementation needs the list size to be a power of two\n");
		exit(1);
	}
	
	//allocate a list
	float *list = malloc(sizeof(float)*SIZE);
	if(list==NULL){
		perror("Mem full");
		exit(1);
	}
	
	int i;
	//generate some random values
	for(i=0;i<SIZE;i++){
		list[i]=rand()/(float)100000;
	}
	
	//print the input list
	printf("The input list is : \n");
	for(i=0;i<SIZE;i++){
		printf("%.2f ",list[i]);
	}
	printf("\n\n");
	
	//start measuring time
	double start = clock();
	
	//do sorting
	mergesort(list);
	
	//stop measuring time
	double stop = clock();
	
	//print the answer
	printf("The sorted list is : \n");
	for(i=0;i<SIZE;i++){
		printf("%.2f \n",list[i]);
	}
	printf("\n\n");	
	
	//print the elapsed time
	double elapsedtime = (stop-start)/(float)CLOCKS_PER_SEC;
	fprintf(stderr, "The elapsed time for soring is %f seconds\n",elapsedtime);
	
	return 0;
}


/* merge two lists while sorting them in ascending order
* For example say there are two arrays 
* while one being 1 3 5 and the other being 2 4 7
* when merge they become 1 2 3 4 5 7
* When storing the two lists they are stored in same array and the
* two arrays are specified using the index of leftmost element, middle element and the last element
* For example say the two arrays are there in memory as a single array 1 3 5 2 4 7
* Here l=0 m=3 and r=5 specifies the two arrays separately
* */

void merge(float *list, int left,int middle,int right){
	
	//calculate the total number of elements
	int n=right-left+1;
	
	//create a new temporary array to do merge
	float *temp=malloc(sizeof(float)*n);
	
	//i is used for indexing elements in left array and j is used for indexing elements in the right array
	int i=left;
	int j=middle;
	
	//k is the index for the temporary array
	int k=0;
	
	/*now merge the two lists in ascending order
	check the first element remaining in each list and select the lowest one from them. Then put it to temp
	put increase the relevant index i or j*/
	
	while(i<middle && j<=right){
		if(list[i]<list[j]){
			temp[k]=list[i];
			i++;
		}
		else{
			temp[k]=list[j];
			j++;
		}
		k++;
	}

	//if there are remaining ones in an array append those to the end
	while (i<middle){
		temp[k]=list[i];
		i++;
		k++;
	}
	while (j<=right){
		temp[k]=list[j];
		j++;
		k++;
	}
			
	//now copy back the sorted array in temp to the original
	for(i=left,k=0;i<=right;i++,k++){
		list[i]=temp[k];	
	}
	
	//free the temporary array
	free(temp);
}

/* carry out merge sort ascending*/
void mergesort(float *list){
	
	int left,middle,right;
	
	//step means the distance to the next list
	//loop till the merging happens for a list of the size of the original list
    int step=1;
    #pragma omp parallel
    {
        #pragma omp single
        {
            while(step<SIZE-1){
		
                //do for all lists in the main list
                int i=0;
                while(i+2*step<=SIZE){
                    
                    //calculate the index of the first element of the first list		
                    left=i;
                    
                    //calculate the index of the first element of  the second list
                    middle=i+step;		
                    
                    //calculate the last element of the second list			
                    right=i+2*step-1;

                    //merge them
                    #pragma omp task shared(list)	
                        merge(list,left,middle,right);
                    
                    //next list pair
                    i=right+1;
                    
                }
                #pragma omp taskwait
                //next list size
                step=step*2;		
            }
        }
    }
	
	

}
