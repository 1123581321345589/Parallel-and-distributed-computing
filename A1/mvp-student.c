#include <stdio.h>
#include <stdlib.h>
#include "mvp.h"

double* mvp1(double* mat, double* vec, int n){
    double *resms = (double*) malloc(n*sizeof(*resms));
    double sum=0;

    for(int i=0;i<n;i++){
       sum=0;
        for(int j=0; j < n; j++){
            sum += mat[i*n+j]*vec[j];
        } 
       resms[i] = sum;
    }

    return resms;
}

double* mvp2(double** mat, double* vec, int n){
  double *resms = (double*) malloc(n*sizeof(*resms));
  double sum = 0;

    for(int i = 0; i < n ; i++){
		sum = 0;
    for(int j = 0; j < n ; j++){
		sum += mat[i][j]*vec[j];
}
    resms[i] = sum;
}

  return resms;

}

void assignMatrix1(double* mat, double* vec, int n){
    for(int i=0;i<n;i++){
        vec[i]=1;
        for(int j=0;j<n;j++){
            if(i==j) mat[i*n+j]=2;
            else if(j==i+1) mat[i*n+j]=1;
            else if(j==i-1) mat[i*n+j]=1;
            else mat[i*n+j]=0;
        }
    }
}

void printMatVec1(double* mat, double* vec, double* res, int n){
    for (int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%5.2f ",mat[i*n+j]);
        }
        if(i==n/2) printf("  %5.2f  =  %5.2f \n",vec[i],res[i]);
        else printf("  %5.2f     %5.2f\n",vec[i],res[i]);
    }
}

void assignMatrix2(double** mat, double* vec, int n){
	for(int i=0;i<n;i++){
		vec[i] = 1;
		for( int j=0;j<n;j++){
			if(i==j) mat[i][j]=2;
			else if(j==i+1) mat[i][j]=1;
			else if(j==i-1) mat[i][j]=1;
			else mat[i][j] =0;
		}
	}
}

void printMatVec2(double** mat, double* vec, double* res, int n){
    for (int i=0;i<n;i++){
        for(int j=0;j<n;j++){
            printf("%5.2f ",mat[i][j]);
        }
        if(i==n/2) printf("  %5.2f  =  %5.2f \n",vec[i],res[i]);
        else printf("  %5.2f     %5.2f\n",vec[i],res[i]);
    }

}

void freeMatrix(void **mat, int n){
    for(int i=0;i<n;i++){
	free(mat[i]);
    }

}

//Main function
int main(int argc, char **argv){
    double* vec;
    double** mat2;

    int n=5;

    printf("Matrix-Vector product using int** matrix:\n");

    vec  =     (double*) malloc(n*sizeof(double));
    mat2 =     (double**) malloc(n*sizeof(double));
    for(int i=0;i<n;i++)
        mat2[i] =  (double*) malloc(n*sizeof(double));

    assignMatrix2(mat2,vec,n);

    double* resms = mvp2(mat2,vec,n);
    printMatVec2(mat2,vec,resms,n);
    printf("\n");
    freeMatrix((void**)mat2,n);






    printf("Matrix-Vector product using int* matrix:\n");

    double* mat1;
    mat1 = (double*) malloc(n*n*sizeof(double));

    assignMatrix1(mat1,vec,n);
    double* res=mvp1(mat1,vec,n);
    printMatVec1(mat1,vec,res,n);
    printf("\n");
    free(mat1);
    free(vec);
    free(resms);

    return 0;
}


