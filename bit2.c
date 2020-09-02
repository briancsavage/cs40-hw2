/*
 * Filename: bit2.c
 * Authors: Robert Lester, Brian Savage
 * Assignment: HW2
 * Summary: This is the implementation of the bit2.h interface, which is a
 *          representation of a 2D unboxed array based on the Hanson ADT       
 *          for bit arrays. This implementation represents the 2D array    
 *          as a 1D array which uses the column and row number of the 2D       
 *          array to set the index for each element in the 1D bit array 
 */

#include <stdlib.h>
#include <bit2.h>
#include "assert.h"
#include "bit.h"
#include <stdio.h>
#include <except.h>

/* Bit2_T Bit2_new(int width, int height, int size)
 * Parameters:
 *              int width: constructed width for Bit2_T, as integer
 *              int height: constructed height for Bit2_T as integer
 *              int size: allocated byte size of one element within Bit2_T
 * Returns: 
 *              Bit2_T: the constructed two dimensional array, with each 
 *              element’s allocation being the parameterized size
 * Does: 
 *              Creates a two dimensional array of bits with the parameterized 
 *              width, height, and each elements size
 */
Bit2_T Bit2_new(int width, int height)
{
        if (width == 0 || height == 0) {
                return NULL;
        }
        Bit2_T set2 = malloc(sizeof(*set2));
        assert(set2 != NULL);
        set2->set = Bit_new(width * height);
        set2->width = width;
        set2->height = height;
        return set2;
        
}

/* int Bit2_height(Bit2_T set)
 * Parameters:
 *              Bit2_T set: constructed height for Bit2_T object, as integer
 * Returns: 
 *              int: integer expressing the height of the Bit2_T array object
 * Does: 
 *              Gets the height of the Bit2_T array object
 */
int Bit2_height(Bit2_T set)
{
        assert(set != NULL);
        return set->height;
}

/* int Bit2_width(Bit2_T set)                                                 
 * Parameters:                                                                 
 *              Bit2_T set: constructed width for Bit2_T object, as integer   
 * Returns:                                                                    
 *              int: integer expressing the width of the Bit2_T array object  
 * Does:                                                                       
 *              Gets the width of the Bit2_T array object                     
 */
int Bit2_width(Bit2_T set)
{
        assert(set != NULL);
        return set->width;
}

/* void Bit2_free(Bit2_T *set2)
 * Parameters:
 *             Bit2_T *set2: a pointer to the Bit2_T array object to be freed
 * Returns: 
 *             void: Nothing
 * Does: 
 *             Frees up memory allocated by the object being pointer to by set2
 */
void Bit2_free(Bit2_T *set2)
{
        assert(set2 != NULL);
        Bit_T *set = &((*set2)->set);
        Bit_free(set);
        free(*set2);
}

/* int Bit2_get(Bit2_T set2, int col, int row)
 * Parameters:
 *               Bit2_T set2: the Bit2_T object which will be indexed
 *               int col: the column index for Bit2_T array object, as an 
 *                        integer
 *               int row: the row index for Bit2_T array object, as an integer
 * Returns: 
 *               int: the bit value at the index of col and row
 * Does: 
 *               Gets the bit value at the passed index of the Bit2_T array 
 *               object 
 */
int Bit2_get(Bit2_T set2, int col, int row)
{
        assert(set2 != NULL);
        assert(set2->width >= 0 && set2->height >= 0);
        int n = (set2->width * row) + col;
        return Bit_get(set2->set, n);
}

/* int Bit2_put(Bit2_T set2, int col, int row, int bit)
 * Parameters:
 *             Bit2_T set2: the Bit2_T object which will have its bit value 
 *             set at the col and row index
 *             int col: the column index for Bit2_T array object, as an integer
 *             int row: the row index for Bit2_T array object, as an integer
 *             int bit: the bit value which shall be set at the column index 
 *             and row index 
 * Returns: 
 *             int:   the previous bit at that address
 * Does: Sets an index as the bit value passed in as a parameter
 *       Sets a value in the Bit2_T array object as the parameter bit at the 
 *       column and row index
 */
int Bit2_put(Bit2_T set2, int col, int row, int bit)
{
        assert (set2 != NULL);
        assert(bit == 0 || bit == 1);
        assert(0 <= col && col <= set2->width);
        assert( 0 <= row && row <= set2->height);
        int n = (set2->width * row) + col;
        int prev = Bit_get(set2->set, n);
        Bit_put(set2->set, n, bit);
        return prev;
}

/* void Bit2_map_row_major(Bit2_T set, 
 *         void apply(int i, int j, Bit2_T a, int b,  void *p1), void *cl)
 * Parameters:
 *         Bit2_T set:  the Bit2_T object which will be validated and printed 
 *         to the user using row major indexing
 *         int i: row index for Bit2_T object, as integer
 *         int j: column index for Bit2_T object, as integer
 *         Bit2_T a: is the same Bit2_T object passed as the parameter set, 
 *         but this is passed to the apply function
 *         int b: the bit value
 *         void *p1: Checks corner for marker of Bit2_T and prints out two 
 *         dimensional array in row major indexing
 *         void *cl: Checks for correct width, height, and element size of 
 *         Bit2_T object
 * Returns: 
 *         void: Nothing, only validates and checks indexing
 * Does: 
 *         Indexes the two dimensional bit array using row major indexing, 
 *         checks the corner of array indexing was correctly handled and 
 *         prints the array as it is indexed 
 */
void Bit2_map_col_major(Bit2_T set, void apply(int i, int j, Bit2_T a, int b, 
                                               void *p1), void *cl)
{
        assert(set != NULL);
        for (int i = 0; i < set->width; i++){
                for (int j = 0; j < set->height; j++){
                  apply(i, j, set, Bit2_get(set, i, j), &cl);
                }
        }
}

/* void Bit2_map_col_major(Bit2_T set,                                         
 *     void apply(int i, int j, Bit2_T a, int b,  void *p1), void *cl)         
 * Parameters:                                                                 
 *         Bit2_T set:  the Bit2_T object which will be validated and printed  
 *         to the user using column major indexing                             
 *         int i: row index for Bit2_T object, as integer                      
 *         int j: column index for Bit2_T object, as integer                   
 *         Bit2_T a: is the same Bit2_T object passed as the parameter set,    
 *         but this is passed to the apply function                            
 *         int b: the bit value                                                
 *         void *p1: Checks corner for marker of Bit2_T and prints out two     
 *         dimensional array in column major indexing                          
 *         void *cl: Checks for correct width, height, and element size of     
 *         Bit2_T object                                                       
 * Returns:                                                                    
 *         void: Nothing, only validates and checks indexing                   
 * Does:                                                                       
 *         Indexes the two dimensional bit array using column major indexing,  
 *         checks the corner of array indexing was correctly handled and       
 *         prints the array as it is indexed                                   
 */
void Bit2_map_row_major(Bit2_T set, void apply(int i, int j, Bit2_T a, int b,
                                               void *p1), void *cl)
{
        assert(set != NULL);
        for (int i = 0; i < set->height; i++){
                for (int j = 0; j < set->width; j++){
                  apply(j, i, set, Bit2_get(set, j, i), &cl);
                }
        }
}
