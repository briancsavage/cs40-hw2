/*
 * Filename: uarray2.c
 * Authors: Robert Lester, Brian Savage
 * Assignment: HW2
 * Summary: This is the implementation of the uarray2.h interface, which is a
 *          representation of a 2D unboxed array based on the Hanson ADT
 *          for unboxed arrays. This implementation represents the 2D array
 *          as a 1D array which uses the column and row number of the 2D
 *          array to set the index for each element in the 1D UArray
 *
 */

#include <stdlib.h>
#include <uarray2.h>
#include "uarray.h"
#include "assert.h"


/* UArray2_T UArray2_new(int width, int height, int size)
 * Parameters:
 *          int width: constructed width for UArray2_T, as integer
 *          int height: constructed height for UArray2_T, as integer
 *          int size: allocated byte size of one element within UArray2_T
 * Returns: 
 *          UArray2_T: the constructed two dimensional array (represented as
 *          1D array), with each element’s allocation being the parameterized 
 *          size
 * Does: 
 *          creates a two dimensional array with the parameterized width, 
 *          height, and each elements size
 */
UArray2_T UArray2_new(int width, int height, int size)
{
        assert (width != 0); 
        assert(height != 0);
        assert(size != 0);
        UArray2_T uarray2 = malloc(sizeof(*uarray2));
        assert(uarray2 != NULL);
        uarray2->uarray = UArray_new(width * height, size);
        uarray2->size = UArray2_size(uarray2);
        uarray2->width = width;
        uarray2->height = height;
        return uarray2;

}


/* int UArray2_height(UArray2_T uarray)
 * Parameters: 
 *        UArray2_T uarray: the UArray2_T object that’s height is being 
 *        returned
 * Returns: 
 *        int: an integer representing the height of the UArray2_T object
 * Does: 
 *        gets the height of the passed UArray2_T object
 */
int UArray2_height(UArray2_T uarray)
{
        assert(uarray != NULL);
        return uarray->height;
}



/* int UArray2_width(UArray2_T uarray)
 * Parameters: 
 *             UArray2_T uarray: the UArray2_T object that’s width is being 
 *             returned
 * Returns: 
 *             Int: an integer representing the width of the UArray2_T object
 * Does: 
 *             gets the width of the passed UArray2_T object
 */
int UArray2_width(UArray2_T uarray)
{
        assert(uarray != NULL);
        return uarray->width;
}


/* int UArray2_size(UArray2_T uarray)
 * Parameters: 
 *              UArray2_T uarray: the UArray2_T object that’s element size is 
 *              being returned
 * Returns:
 *              Int: an integer representing the size of a single element in 
 *              the UArray2_T object
 * Does: 
 *              gets the element size of the passed UArray2_T object
 */
int UArray2_size(UArray2_T uarray2)
{
        assert(uarray2 != NULL);
        return UArray_size(uarray2->uarray);
}

/* void UArray2_free(UArray2_T *uarray2)
 * Parameters: 
 *            UArray2_T *uarray2 - pointer to uarray2 to be freed
 * Returns:
 *            Nothing
 * Does:
 *            frees memory
 */
void UArray2_free(UArray2_T *uarray2)
{
        assert(uarray2 != NULL);
        UArray_T *uarray = &((*uarray2)->uarray);
        UArray_free(uarray);
        free(*uarray2);
}

/* void *UArray2_at(UArray2_T uarray, int col, int row)
 * Parameters: 
 *             UArray2_T uarray: the UArray2_T object that’s being indexed
 *             int col: column index for UArray2_T object, as integer
 *             int row: row index for UArray2_T object, as integer
 * Returns: 
 *             void *: returns a void pointer to the element indexed, will be 
 *             set equal to an integer pointer to allow for dereferencing 
 *             without error
 * Does: 
 *             index the array and returns the element at a specified column
 *             and row
 */
void *UArray2_at(UArray2_T uarray2, int col, int row)
{
        assert(row >= 0 && row <= uarray2->height);
        assert(col >= 0 && col <= uarray2->width);
        int i = (UArray2_width(uarray2) * row) + col;
        return UArray_at(uarray2->uarray, i);
}

/* void UArray2_map_col_major(UArray2_T uarray, 
 *                         void apply(int i, int j, UArray2_T a, void *p1, 
 *                                    void *p2), void *cl)
 * Parameters: 
 *             UArray2_T uarray: the UArray2_T object being indexed
 *             int i: row index for UArray2_T object, as integer
 *             int j: column index for UArray2_T object, as integer
 *             UArray2_T a: is the same UArray2_T object passed as the 
 *             parameter uarray, but this is passed to the apply function
 *             void *p1: Checks corner for marker of uarray and prints out two
 *             dimensional array in column major indexing
 *             void *p2: Checks for correct width, height, and element size of 
 *             UArray2_T object
 * Returns: 
 *             Void: Nothing, only validates and prints
 * Does: 
 *             indexes the two dimensional array using column major indexing, 
 *             checks that corner of array indexing was correctly handled and 
 *             prints out the array as it is indexed
 */
void UArray2_map_col_major(UArray2_T uarray, void apply(int i, int j, 
                                                              UArray2_T a, 
                                                              void *p1, 
                                                              void *p2), 
                           void *cl)
{
        assert(uarray != NULL);
        for (int i = 0; i < uarray->width; i++){
                for (int j = 0; j < uarray->height; j++){
                        apply(i, j, uarray, UArray2_at(uarray, i, j), cl);
                }
        } 
}

/* void UArray2_map_row_major(UArray2_T uarray,                                
 *                         void apply(int i, int j, UArray2_T a, void *p1,     
 *                                    void *p2), void *cl)                     
 * Parameters:                                                                 
 *             UArray2_T uarray: the UArray2_T object being indexed            
 *             int i: row index for UArray2_T object, as integer               
 *             int j: column index for UArray2_T object, as integer           
 *             UArray2_T a: is the same UArray2_T object passed as the         
 *             parameter uarray, but this is passed to the apply function      
 *             void *p1: Checks corner for marker of uarray and prints out two 
 *             dimensional array in column major indexing                      
 *             void *p2: Checks for correct width, height, and element size of 
 *             UArray2_T object                                                
 * Returns:                                                                    
 *             Void: Nothing, only validates and prints                        
 * Does:                                                                       
 *             indexes the two dimensional array using row major indexing,  
 *             checks that corner of array indexing was correctly handled and  
 *             prints out the array as it is indexed                           
 */
void UArray2_map_row_major(UArray2_T uarray, void apply(int i, int j, 
                                                              UArray2_T a, 
                                                              void *p1, 
                                                              void *p2), 
                           void *cl)
{
        assert(uarray != NULL);
        assert(uarray->height != 0 && uarray->width != 0);
        for (int i = 0; i < uarray->height; i++){
                for (int j = 0; j < uarray->width; j++){
                        apply(j, i, uarray, UArray2_at(uarray, j, i), cl);
                }
        } 
}
