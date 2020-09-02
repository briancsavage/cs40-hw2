/* bit2.h
 * Brian Savage and Robert Lester
 * bsavag01         rleste01
 * HW 2 - iii
 * Interface for bit2, functions explained in implementation
 */

#include<bit.h>

#ifndef BIT2_INCLUDED
#define BIT2_INCLUDED

#define T Bit2_T
typedef struct T{
  int height; /* height of 2D Bitmap */
  int width; /* width of 2D Bitmap */
  Bit_T set; /* 1D Bitmap */
} *T;

extern T Bit2_new(int width, int height);
extern int Bit2_row_index(int col, int row);
extern void Bit2_free(T *set);
extern int Bit2_height(T set);
extern int Bit2_width(T set);
extern int Bit2_get(T set2, int col, int row);
extern int Bit2_put(T set2, int col, int row, int bit);
extern void Bit2_map_col_major(T set, void apply(int i, int j, T a, int b,
                                                 void *p1), void *cl);
extern void Bit2_map_row_major(T set, void apply(int i, int j, T a, int b,
                                                       void *p1), void *cl);

#undef T
#endif
