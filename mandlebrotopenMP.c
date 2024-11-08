#include <stdio.h>
#include <time.h>
#include <omp.h>

#define WIDTH 640
#define HEIGHT 480
#define MAX_ITER 255

struct complex{
  double real;
  double imag;
};


int cal_pixel(struct complex c) {
    

            double z_real = 0;
            double z_imag = 0;

            double z_real2, z_imag2, lengthsq;

            int iter = 0;
            do {
                z_real2 = z_real * z_real;
                z_imag2 = z_imag * z_imag;

                z_imag = 2 * z_real * z_imag + c.imag;
                z_real = z_real2 - z_imag2 + c.real;
                lengthsq =  z_real2 + z_imag2;
                iter++;
            }
            while ((iter < MAX_ITER) && (lengthsq < 4.0));

            return iter;

}

void save_pgm(const char *filename, int image[HEIGHT][WIDTH]) {
    FILE* pgmimg; 
    int temp;
    pgmimg = fopen(filename, "wb"); 
    fprintf(pgmimg, "P2\n"); // Writing Magic Number to the File   
    fprintf(pgmimg, "%d %d\n", WIDTH, HEIGHT);  // Writing Width and Height
    fprintf(pgmimg, "255\n");  // Writing the maximum gray value 
    int count = 0; 
    
    for (int i = 0; i < HEIGHT; i++) { 
        for (int j = 0; j < WIDTH; j++) { 
            temp = image[i][j]; 
            fprintf(pgmimg, "%d ", temp); // Writing the gray values in the 2D array to the file 
        } 
        fprintf(pgmimg, "\n"); 
    } 
    fclose(pgmimg); 
} 


int main() {

    int image[HEIGHT][WIDTH];
    
    int MAX_NUM_THREADS = 6; // maximum number of threads
    double runtime;
    struct complex c;
    
    
    
    for (int k=1; k<=MAX_NUM_THREADS; k++){ // using this loop for changing the num of threads and comparing the execution time, getting the runtime for each number of thread
      omp_set_num_threads(k);  //setting each time a num of threads from 1 to 6
      int i, j;
      double start_time = omp_get_wtime(); // Start measuring time
      
      #pragma omp parallel for schedule(dynamic, 1) private(c)  // parallizing the outer loop and setting the chunk size to one row and the work to be dynamic
      	for (i = 0; i < HEIGHT; i++) {  //iterate over the row , Row-wise chuncking, each thread handles a row 
          	for (j = 0; j < WIDTH; j++) {  
              		c.real = (j - WIDTH / 2.0) * 4.0 / WIDTH;
              		c.imag = (i - HEIGHT / 2.0) * 4.0 / HEIGHT;
              		image[i][j] = cal_pixel(c);
          }
        }
      double end_time = omp_get_wtime(); // End measuring time
      runtime = ((double)(end_time - start_time));
      printf("Execution time with %d thread: %f seconds\n", k , runtime);
      
      char arrayimage[50];
      sprintf(arrayimage, "mandlebrot_%d_threads.pgm", 2);
      save_pgm(arrayimage, image);
      
        
    }


    
    return 0;
}

