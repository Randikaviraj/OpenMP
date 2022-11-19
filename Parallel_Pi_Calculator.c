#include <stdio.h>
#include <omp.h>

int main(int argc, char const *argv[])
{
    unsigned long nsteps = 1<<27; /* around 10^8 steps */
    double dx = 1.0 / nsteps;
    double start_time = omp_get_wtime();
    double pi = 0.0;
    int tid,noft,to,from;
    #pragma omp parallel default(shared) private(tid,noft,to,from)
    {
        tid=omp_get_thread_num();
        noft=omp_get_num_threads();
        from=(nsteps/noft)*tid;

        if (tid==noft-1)
        {
            to=nsteps;
        }else
        {
            to=(nsteps/noft)*(tid+1);
        }
        
        unsigned long i;
        double temp_pi = 0;
        for (i = from; i < to; i++)
        {
            double x = (i + 0.5) * dx;
            temp_pi += 1.0 / (1.0 + x * x);
            
        } 
        #pragma omp critical
        {
            pi += temp_pi;
        }
    }
    pi *= 4.0 * dx;
    double end_time = omp_get_wtime();
    printf("Answer PI: %lf\n",pi);
    printf("Execution Time: %lf\n",(end_time-start_time));
    return 0;
}