/* C program to generate a mandelbrot set image */

#include <math.h>
#include <stdio.h>
#include <time.h>
#include <omp.h>

//The width of the generated image in pixels
#define WIDTH 1027
//The height of the generated image in pixels
#define HEIGHT 768
//The value on the real axis which maps to the left most pixel of the image
#define XMIN -2.0
//The value of the real axis which maps to the right most pixel of the image
#define XMAX 1
//The value in the imaginary axis which maps to the top pixels of the image
#define YMIN -1.25
//The value in the imaginary axis which maps to the bottom pixels of the image
#define YMAX 1.25
//The value that we consider as infinity.
#define INF 4
//The maximum number of times  that is tried to check whether infinity is reached.
#define MAXN 3000
//find maximum of two numbers
#define max(a,b) (((a)>(b))?(a):(b))
//File name for output image
#define FILENAME "image.ppm"

//The matrix to save the m andelbrot set
int mandel_set[HEIGHT][WIDTH]; 
//The RGB values for the mandelbrot image
unsigned char image[WIDTH *HEIGHT * 3];

/***********************************Color transformations ***********************************************/

/*Calculate R value in RGB based on divergence.*/
unsigned char red(int i){
	if (i==0 )
		return 0 ;
	else 
	return ((i+10)%256);
}

/*Calculate B value in RGB based on divergence*/
unsigned char blue(int i){
	if (i==0)
	return  0;
	else
	return ((i + 234) % 7 * (255/7));
}

/*Calculate G value in RGB based on divergence*/
unsigned char green(int i){
	if (i==0)
		return  0 ;
	else
		return ((i+100) % 9 * (255/9));
}
	


/************************************ Pixel  transformations ***********************************************/

//Here x is the pixel number and this is linearly transformed to a value in the real axis to a value between XMIN and XMAX
float transform_to_x(int x){ 
	return XMIN+x*(XMAX-XMIN)/(float)WIDTH;
}
//Here y is the pixel number and this is linearly transformed to a value in the imaginary axis to a value between YMIN and YMAX
float transform_to_y(int y){
	return YMAX-y*(YMAX-YMIN)/(float)HEIGHT;
}


/**************************************Mandelbrot calculation ***********************************************/
//check a given complex number is in Mandelbrot set. return 0 if is in mandelbrot set else return a value based on divergence to later assign a color
int isin_mandelbrot(float realc,float imagc){
	
	//initialize values
	int i=0;
	float realz_next=0,imagz_next=0;
	float abs=0;
	float realz=0;
	float imagz=0;
	
	//do the calculations till the inifinity(a large number) is reached or the maximum number of iterations is reached
	while(i<MAXN && abs<INF){
		
		//mandelbrot equations
		realz_next=realz*realz-imagz*imagz+realc;
		imagz_next=2*realz*imagz+imagc;
		
		//absolute value
		abs=realz*realz+imagz*imagz;
		
		//next values
		realz=realz_next;
		imagz=imagz_next;
		i++;
	}
	
	//if the number of iterations had reached maximum that means hasnt reached infinity and we say the number of in not in mandelbrot set
	if (i==MAXN)
		return 0;
	//if the max number of iterations hasnt reached that means it has hit the infinity before that
	// then we say it is not in mandelbrot set and return the number of iterations, to later compute a color value
	else
		return i;
}


/* Create the mandelbrot matrix. If a pixel is in mandelbrot set value is 0. else the divergence */
void plot(int blank[HEIGHT][WIDTH]){
    int tid,noft,to,from;
    #pragma omp parallel default(shared) private(tid,noft,to,from)
    {
        tid=omp_get_thread_num();
        noft=omp_get_num_threads();
        from=((HEIGHT)/noft)*tid;
        if (tid==noft-1)
        {
            to=HEIGHT;
        }else
        {
            to=(HEIGHT/noft)*(tid+1);
        }
        
        int x,y;
        //calculate whether is in mandelbrot or not, for each pixel. If not, the divergence is entered as the value. If yes 0 is entered
        for (y = from; y < to; y++)
        {
            for (x=0;x<WIDTH;x++){
                #pragma omp critical
                {
                    blank[y][x]=isin_mandelbrot(transform_to_x(x),transform_to_y(y));
                }
			    // blank[y][x]=isin_mandelbrot(transform_to_x(x),transform_to_y(y));
		    }
            
        } 
    }	
}

/* Create the mandelbrot RGB image matrix mbased on the mandelbrot matrix. This is an array with RGB values*/
void createimage() {
    int tid,noft,to,from;
    #pragma omp parallel default(shared) private(tid,noft,to,from)
    {
        tid=omp_get_thread_num();
        noft=omp_get_num_threads();
        from=((HEIGHT)/noft)*tid;
        if (tid==noft-1)
        {
            to=HEIGHT;
        }else
        {
            to=(HEIGHT/noft)*(tid+1);
        }
        
        int x,y,n;int color;
        n=from*3*WIDTH;
        //Generate the RGB matrix based on divergence value
        for (y = from; y < to; y++)
        {
            for(x=0;x<WIDTH;x++){
                color=mandel_set[y][x];
                image[n]=red(color);
                image[n+1]=green(color);
                image[n+2]=blue(color);
                n=n+3;
            }
            
        } 
    }

}


/***********************************main function*********************************************************/

int main(int argc, char** argv) {

	//start measuring time
	clock_t begin,end;
	begin=clock();

	//create the mandelbrot matrix
	plot(mandel_set);
	//generate the mandelbrot RGB image matrix
	createimage();
  
	//stop measuring time and print
	end=clock();
	double cputime=(double)((end-begin)/(float)CLOCKS_PER_SEC);
	printf("Time using CPU for calculation is %.10f\n",cputime);

	/* Write the image to file*/
  
	//meta data for the file
    const int MaxColorComponentValue=255; 
    char *comment="# ";//comment should start with # 
        
    //create new file,give it a name and open it in binary mode     
    FILE * fp=fopen(FILENAME,"wb"); // b -  binary mode 
    
    //write ASCII metadata header to the file
    fprintf(fp,"P6\n %s\n %d\n %d\n %d\n",comment,WIDTH,HEIGHT,MaxColorComponentValue);
    
    //compute and write image data bytes to the file
    fwrite(image,1,WIDTH *HEIGHT * 3,fp);
			
    //close the file 
    fclose(fp);
		
	return 0;
}
