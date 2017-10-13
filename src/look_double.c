#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>


FILE *openfile(char *filename, char *pattern){
  FILE *fp;
  
  fp=fopen(filename, pattern);
  if (fp==NULL){
    fprintf(stderr,"Error: cannot open file: %s\n", filename);
    exit(1);
  }

  return fp;
}

void readdata(void *data, size_t blocksize, FILE *fp){
  if(fread(data, blocksize, 1, fp) != 1){
    fprintf(stderr,"Error: cannot read data\n");
    exit(1);
  }
}

void writedata(void *data, size_t blocksize, FILE *fp){
  if(fwrite(data, blocksize, 1, fp) != 1){
    fprintf(stderr,"Error: cannot write data\n");
    exit(1);
  }
}

long file_length(FILE* fp, long width, long element_size){
  long length;
  
  fseeko(fp,0L,SEEK_END);
  length = ftello(fp) / element_size / width;
  rewind(fp);
  
  return length;
}

double *array1d_double(long nc){

  double *fv;

  fv = (double*) malloc(nc * sizeof(double));
  if(!fv){
    fprintf(stderr,"Error: cannot allocate 1-D double vector\n");
    exit(1);
  }

  return fv;
}

void free_array1d_double(double *fv){
  free(fv);
}

double **array2d_double(long nl, long nc){
/* allocate a double 2-D matrix */

  double **m;
  int i;

  /* allocate pointers to rows */
  m = (double **) malloc(nl * sizeof(double *));
  if(!m){
    fprintf(stderr,"Error: cannot allocate 2-D matrix\n");
    exit(1);
  }
 
  /* allocate rows */ 
  m[0] = (double*) malloc(nl * nc * sizeof(double));
  if(!m[0]){
    fprintf(stderr,"Error: cannot allocate 2-D matrix\n");
    exit(1);
  }

   /* set pointers */
  for(i = 1; i < nl; i++){
    m[i] = m[i-1] + nc;
  }

  return m;
}

void free_array2d_double(double **m){
/* free a double matrix allocated by farray2d() */
  free(m[0]);
  free(m);
}


int main(int argc, char *argv[]){

  FILE *infp;
  FILE *outfp;
  double **in;
  double *a;
  double sum;
  double *out;
  long nrg, naz;
  long nrg1, naz1;
  int nrlks, nalks;
  int i, j, k;

  if(argc < 4){
    fprintf(stderr, "\nUsage: %s infile outfile nrg nrlks nalks\n\n", argv[0]);
    fprintf(stderr, "  infile:  input file\n");
    fprintf(stderr, "  outfile: output file\n");
    fprintf(stderr, "  nrg:     file width\n");
    fprintf(stderr, "  nrlks:   number of looks in range (default: 4)\n");
    fprintf(stderr, "  nalks:   number of looks in azimuth (default: 4)\n\n");
    exit(1);
  }

  infp  = openfile(argv[1], "rb");
  outfp = openfile(argv[2], "wb");
  
  nrg = atoi(argv[3]);
  naz = file_length(infp, nrg, sizeof(double));

  if(argc > 4)
    nrlks = atoi(argv[4]);
  else
    nrlks = 4;

  if(argc > 5)
    nalks = atoi(argv[5]);
  else
    nalks = 4;
  
  nrg1 = nrg / nrlks;
  naz1 = naz / nalks;

  in = array2d_double(nalks, nrg);
  a  = array1d_double(nrg);
  out= array1d_double(nrg1);


  for(i = 0; i < naz1; i++){

    if((i + 1) % 100 == 0)
      fprintf(stderr,"processing line: %6d of %6d\r", i+1, naz1);

    readdata((double *)in[0], (size_t)nalks * (size_t)nrg * sizeof(double), infp);
    //take looks in azimuth
    for(j = 0; j < nrg; j++){
      a[j] = 0.0;
      for(k = 0; k < nalks; k++){
        a[j] += in[k][j];
      }
    }
    //take looks in range
    for(j = 0; j < nrg1; j++){
      sum = 0.0;
      for(k = 0; k < nrlks; k++){
        sum += a[j * nrlks + k];
      }
      out[j] = (double)(sum / nrlks / nalks);
    }
  
    writedata((double *)out, nrg1 * sizeof(double), outfp);
  }
  fprintf(stderr,"processing line: %6d of %6d\n", naz1, naz1);

  free_array2d_double(in);
  free_array1d_double(a);
  free_array1d_double(out);
  fclose(infp);
  fclose(outfp);

  return 0;
}
