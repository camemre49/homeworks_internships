/********************************************************
 * Kernels to be optimized for the CS:APP Performance Lab
 ********************************************************/

#include <stdio.h>
#include <stdlib.h>
#include "defs.h"

/*******************************************************
 * BATCHED MATRIX MULTIPLICATION \W SUM REDUCTION KERNEL
 *******************************************************/

/*********************************************************************************
 * Your different versions of the batched matrix multiplication functions go here
 *********************************************************************************/

/*
 * naive_batched_mm - The naive baseline version of batched matrix multiplication
 */
char naive_batched_mm_descr[] = "naive_batched_mm: Naive baseline implementation";
void naive_batched_mm(int dim, int *b_mat, int *mat, int *dst) {
    int i,j,k,l;

    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j++) {
            for (k = 0; k < dim; k++) {
            	if (i == 0){
            	    dst[RIDX(j, k, dim)] = 0;
            	}
            	for (l = 0; l < dim; l++){
                    dst[RIDX(j, k, dim)] += b_mat[RIDX(i*dim+j, l, dim)] * mat[RIDX(l, k, dim)];
                }
            }
        }
    }
}

/*
 * batched_mm - Your current working version of batched matrix multiplication
 * IMPORTANT: This is the version you will be graded on
 */
char batched_mm_descr[] = "Batched MM with sum reduction: Current working version";
void batched_mm(int dim, int *b_mat, int *mat, int *dst)
{
    int i,j,k, i2, j2, k2;
    int temp2 = dim * dim;

    int mat1 = 0;
    int mat2 = temp2;
    int dimS = temp2 << 2;
    int mat3 = temp2 << 1;
    int mat4 = temp2 * 3;

    int yeni[temp2];
    for(int i=0; i<temp2; i++){
        yeni[i] = 0;
    }

    for(int i=0; i<dim; i+=4) {
        for(int jl=0; jl<temp2; jl+=8) {
            yeni[jl] += (b_mat[mat1+jl] + b_mat[mat2+jl]) + (b_mat[mat3+jl] + b_mat[mat4+jl]);
            yeni[jl+1] += (b_mat[mat1+jl+1] + b_mat[mat2+jl+1]) + (b_mat[mat3+jl+1] + b_mat[mat4+jl+1]);
            yeni[jl+2] += (b_mat[mat1+jl+2] + b_mat[mat2+jl+2]) + (b_mat[mat3+jl+2] + b_mat[mat4+jl+2]);
            yeni[jl+3] += (b_mat[mat1+jl+3] + b_mat[mat2+jl+3]) + (b_mat[mat3+jl+3] + b_mat[mat4+jl+3]);
            yeni[jl+4] += (b_mat[mat1+jl+4] + b_mat[mat2+jl+4]) + (b_mat[mat3+jl+4] + b_mat[mat4+jl+4]);
            yeni[jl+5] += (b_mat[mat1+jl+5] + b_mat[mat2+jl+5]) + (b_mat[mat3+jl+5] + b_mat[mat4+jl+5]);
            yeni[jl+6] += (b_mat[mat1+jl+6] + b_mat[mat2+jl+6]) + (b_mat[mat3+jl+6] + b_mat[mat4+jl+6]);
            yeni[jl+7] += (b_mat[mat1+jl+7] + b_mat[mat2+jl+7]) + (b_mat[mat3+jl+7] + b_mat[mat4+jl+7]);
        }
        mat1 += dimS;
        mat2 += dimS;
        mat3 += dimS;
        mat4 += dimS;
    }





    int *rdst, *rmat, *rsrc;

    int bSize = 32;
    int iDim, iDimK, iDimJ, kDimJ ;

    for(int idx = 0; idx<temp2; idx++)
        dst[idx] = 0;

    for (i = 0; i < dim; i += bSize) {
        iDim = i*dim;
        for (j = 0; j < dim; j += bSize) {
            iDimJ = iDim + j;
            for (k = 0; k < dim; k += bSize){
                iDimK = iDim + k;
                kDimJ = k*dim + j;
                // dst[RIDX(i, j, dim)] += b_mat[RIDX(matrix*dim+i, k, dim)] * mat[RIDX(k, j, dim)];
                for(i2 = 0,
                    rdst = &dst[iDimJ],
                    rsrc = &yeni[iDimK];

                    i2 < bSize; ++i2, rdst += dim, rsrc += dim){
                    for(k2 = 0,
                        rmat = &mat[kDimJ];

                        k2 < bSize; ++k2, rmat += dim){
                        for(j2 = 0; j2 < bSize; ++j2){
                            rdst[j2] += rsrc[k2] * rmat[j2];
                        }
                    }
                }
            }
        }
    }
}

/*********************************************************************
 * register_batched_mm_functions - Register all of your different versions
 *     of the batched matrix multiplication functions  with the driver by calling the
 *     add_batched_mm_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 *********************************************************************/

void register_batched_mm_functions() {
    add_batched_mm_function(&naive_batched_mm, naive_batched_mm_descr);
    add_batched_mm_function(&batched_mm, batched_mm_descr);
    /* ... Register additional test functions here */
}




/************************
 * POINT REFLECTION KERNEL
 ************************/

/*********************************************************
 * Your different versions of the point reflection go here
 *********************************************************/

/*
 * naive_reflect - The naive baseline version of point reflection
 */
char naive_reflect_descr[] = "Naive Point Reflection: Naive baseline implementation";
void naive_reflect(int dim, int *src, int *dst) {
    int i, j;

    for (i = 0; i < dim; i++) {
	for (j = 0; j < dim; j++) {
	    dst[RIDX(dim-1-i, dim-1-j, dim)] = src[RIDX(i, j, dim)];
	}
    }
}


/*
 * reflect - Your current working version of reflect
 * IMPORTANT: This is the version you will be graded on
 */
char reflect_descr[] = "Point Reflection: Current working version";
void reflect(int dim, int *src, int *dst)
{
    int i, j;
    int temp4 = 0;
    int temp3 = dim*dim - 1;
    // #define RIDX(i,j,n) ((i)*(n)+(j))
    for (i = 0; i < dim; i++) {
        for (j = 0; j < dim; j+=32) {
            //dst[RIDX(dim-1-i, dim-1-j, dim)] = src[RIDX(i, j, dim)];
            dst[temp3] = src[temp4];
            dst[temp3-1] = src[temp4+1];
            dst[temp3-2] = src[temp4+2];
            dst[temp3-3] = src[temp4+3];
            dst[temp3-4] = src[temp4+4];
            dst[temp3-5] = src[temp4+5];
            dst[temp3-6] = src[temp4+6];
            dst[temp3-7] = src[temp4+7];
            dst[temp3-8] = src[temp4+8];
            dst[temp3-9] = src[temp4+9];
            dst[temp3-10] = src[temp4+10];
            dst[temp3-11] = src[temp4+11];
            dst[temp3-12] = src[temp4+12];
            dst[temp3-13] = src[temp4+13];
            dst[temp3-14] = src[temp4+14];
            dst[temp3-15] = src[temp4+15];
            dst[temp3-16] = src[temp4+16];
            dst[temp3-17] = src[temp4+17];
            dst[temp3-18] = src[temp4+18];
            dst[temp3-19] = src[temp4+19];
            dst[temp3-20] = src[temp4+20];
            dst[temp3-21] = src[temp4+21];
            dst[temp3-22] = src[temp4+22];
            dst[temp3-23] = src[temp4+23];
            dst[temp3-24] = src[temp4+24];
            dst[temp3-25] = src[temp4+25];
            dst[temp3-26] = src[temp4+26];
            dst[temp3-27] = src[temp4+27];
            dst[temp3-28] = src[temp4+28];
            dst[temp3-29] = src[temp4+29];
            dst[temp3-30] = src[temp4+30];
            dst[temp3-31] = src[temp4+31];
            temp3 -= 32;
            temp4 += 32;
        }
    }
}

/******************************************************************************
 * register_reflect_functions - Register all of your different versions
 *     of the reflect with the driver by calling the
 *     add_reflect_function() for each test function. When you run the
 *     driver program, it will test and report the performance of each
 *     registered test function.
 ******************************************************************************/

void register_reflect_functions() {
    add_reflect_function(&naive_reflect, naive_reflect_descr);
    add_reflect_function(&reflect, reflect_descr);
    /* ... Register additional test functions here */
}

