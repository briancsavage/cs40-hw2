/*
 * Brian Savage and Robert Lester
 * bsavag01         rleste01
 * HW2 - iii
 * uarray2.h
 * Interface for uarray2, functions explained in implementation
 */
#include <uarray.h>

#ifndef UARRAY2_INCLUDED
#define UARRAY2_INCLUDED

#define T UArray2_T
typedef struct T{
  int height; /* height of 2D UArray */
  int width; /* width of 2D UArray */
  int size; /* size of element */
  UArray_T uarray; /* 1D UArray */
} *T;

T UArray2_new(int width, int height, int size);
void UArray2_free(T *uarray2);
int UArray2_height(T uarray);
int UArray2_width(T uarray);
int UArray2_size(T uarray2);
void *UArray2_at(T uarray2, int col, int row);
void UArray2_map_col_major(T uarray, void apply(int i, int j, T a,
                                                             void *p1, 
                                                             void *p2),
                                  void *cl);
void UArray2_map_row_major(T uarray, void apply(int i, int j, T a,
                                                             void *p1,
                                                             void *p2),
                                  void *cl);

#undef T
#endif
