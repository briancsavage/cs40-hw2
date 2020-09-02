/*
 * Filename: sudoku.c
 * Authors: Robert Lester, Brian Savage
 * Assignment: HW2
 * Summary: This program is used to take in a sudoku board in pgm format,
 *          determine whether it is a valid solution for a sudoku board in 
 *          which case it will return zero. Otherwise it will return 1. 
 *
 */





#include <stdio.h>
#include <except.h>
#include <stdlib.h>
#include "pnmrdr.h"
#include "uarray2.h"
#include "uarray.h"
#include "seq.h"
#include "assert.h"


FILE *check_arguments(FILE *fp, UArray2_T board, int argc, char *argv[]);

void fill_board(FILE *fp, UArray2_T board);

void populate_board(int col, int row, UArray2_T board, void *p1, void *cl);

void check_board(UArray2_T board);

void check_rows(int col, int row, UArray2_T board, void *p1, void *cl);

void check_cols(int col, int row, UArray2_T board, void *p1, void *cl);

void check_box(UArray2_T board);

Seq_T check_inside_box(int col, int row, UArray2_T board, Seq_T seq);

void check_box_dupes(UArray2_T board, Seq_T seq);

void check_dupes(UArray2_T board, UArray2_T check_arr, void *p1);

void clear_arr(int col, int row, UArray2_T board, void *p1, void *cl);

void error(FILE *fp, UArray2_T board, char *msg);

void error_rdr(FILE *fp, Pnmrdr_T rdr, UArray2_T board, char *msg);

/* constant integers representing the height and width of the sudoku board*/
const int HEIGHT = 9;
const int WIDTH = 9;


int main(int argc, char *argv[])
{
        UArray2_T board = UArray2_new(WIDTH, HEIGHT, sizeof(int));
        FILE *fp = NULL;
        /* making sure arguments are valid */
        fp = check_arguments(fp, board, argc, argv);

        /* populating board with elements from pgm */
        fill_board(fp, board);

        fclose(fp);

        /* checks board for any duplicates */
        check_board(board);

        UArray2_free(&board);

        /* exits with code 0 if there are no duplicates */
        exit(EXIT_SUCCESS);
}



/* FILE *check_arguments(FILE *fp, int argc, char *argv[])
 * Parameters: File *fp - FILE object to store opened file
 *             int argc - number of command line arguments
 *             char *argv[] - command line arguments
 * Returns: opened FILE *
 * Does: Goes through to check the number of command line arguments,
 *       calls error and exits if there is a problem with the arguments,
 *       or opens the file and returns it
 */
FILE *check_arguments(FILE *fp, UArray2_T board, int argc, char *argv[])
{
        if (argc > 2) {
                error(NULL, board, "Error: too many arguments given\n");
        }
        
        if (argc == 2) {
                fp = fopen(argv[1], "rb");
                if (fp == NULL) {
                        error(NULL, board, "Error: trouble reading file\n");
                }
        }
        
        else {
                fp = stdin;
                if (fp == NULL) {
                        error(NULL, board, "Error: trouble reading file\n");
                }
        }
        
        return fp;
}



/* void fill_board(FILE *fp, UArray2_T board)
 * Parameters: FILE *fp - opened file to become a Pnmrdr_T object
 *             UArray2_T board - array representing full sudoku board
 * Returns: Nothing
 * Does: This function creates a Pnmrdr_T object, checks that the data for
 *       that object is correct for the formatting, and then uses
 *       UArray2_map_row_major to call a helper function to populate the array
 *
 */
void fill_board(FILE *fp, UArray2_T board) 
{
        Pnmrdr_T rdr;
        Pnmrdr_mapdata data;

        /* TRY/EXCEPT statements used to make sure that Pnmrdr_T object is
           in the correct format all around || this one creates an object used
           to check the pixel intensity */
         TRY
                 rdr = Pnmrdr_new(fp);
         
         EXCEPT(Pnmrdr_Badformat)
                 error_rdr(fp, rdr, board,
                           "Error: bad format, could not read image\n");
         
         EXCEPT(Pnmrdr_Count)
                 error_rdr(fp, rdr, board,
                           "Error: could not read pixels\n");
         END_TRY;
         
         data = Pnmrdr_data(rdr);

         /*makes sure board is the right size/type */
         if (data.type != Pnmrdr_gray) {
                error_rdr(fp, rdr, board,
                          "Error: incorrect file type, requires pgm file\n");
         }
         
         if (data.width != 9 || data.height != 9 || data.denominator != 9) {
                 error_rdr(fp, rdr, board,
                           "Error: incorrect dimensions or denominator\n");
        }
         
         /*fills board with the elemnts at each square of the sudoku board */
         UArray2_map_row_major(board, populate_board, rdr);
         
         Pnmrdr_free(&rdr);
}



/* void populate_board(int col, int row, UArray2_T board, void *p1, void *cl)
 * Parameters: int col - column number
 *             int row - row number
 *             UArray2_T board - array representing sudoku board
 *             void *p1 - holds value at position board(i, j)
 *             void *p2 - holds the Pnmrdr_T object
 * Returns: Nothing
 * Does: Called by UArray2_map_row_major which iterates through the board
 *       populating it with the values from p2
 *
 */
void populate_board(int col, int row, UArray2_T board, void *p1, void *cl)
{
        (void) col;
        (void) row;
        (void) board;
        
        /* makes sure arguments are correctly passed */ 
        assert(cl != NULL);
        assert(sizeof(*(int *)p1) == UArray2_size(board));
        
        /* sets index in board (p1) to the corresponding index on the sudoku 
           board in Pnmrdr_T format */
        int val = Pnmrdr_get(cl);
        assert(val > 0 && val < 10);
        *(int *)p1 = val;
}



/* void check_board(UArray2_T board)
 * Parameters: UArray2_T board - array representing the full sudoku board
 * Returns: Nothing
 * Does: Used to call functions which check that each row, column, and 3x3
 *       submap contains a set of the numbers 1-9
 *
 */
void check_board(UArray2_T board)
{
        UArray2_T check_arr = UArray2_new(9, 1, sizeof(int));
        
        /* checks all rows */
        UArray2_map_row_major(board, check_rows, &check_arr);
        /* checks all columns */
        UArray2_map_col_major(board, check_cols, &check_arr);
        
        
        UArray2_free(&check_arr);
        
        /* checks all 3x3 submaps */
        check_box(board);
}



/* void check_rows(int col, int row, UArray2_T board, void *p1, void *cl)
 * Parameters: int col - column number
 *             int row - row number                                      
 *             UArray2_T board - array representing the sudoku board
 *             void *p1 - holds value at position board(i, j)
 *             void *p2 - holds the temporary 9x1 array used to check for 
 *                        duplicates in a row
 * Returns: Nothing
 * Does: Checks to make sure that all rows have a set of numbers 1-9 with no 
 *       duplicates
 */
void check_rows(int col, int row, UArray2_T board, void *p1, void *cl)
{
        (void) row;
        (void) board;
        (void) p1;

        /* if the column number is zero, it resets the 9x1 array */
        if (col == 0) {
                UArray2_map_row_major(*(UArray2_T *)cl, clear_arr, NULL);
        }

        /* checks for duplicates */
        check_dupes(board, *(UArray2_T *)cl, p1);
}



/* void check_cols(int col, int row, UArray2_T board, void *p1, void *cl)
 * Parameters: int col - column number
 *             int row - row number
 *             UArray2_T board - array representing the sudoku board
 *             void *p1 - holds value at position board(i, j)
 *             void *p2 - holds the temporary 9x1 array used to check for     
 *                        duplicates in a column 
 * Returns: Nothing
 * Does:Checks to make sure that all rows have a set of numbers 1-9 with no    
 *       duplicates 
 *
 */
void check_cols(int col, int row, UArray2_T board, void *p1, void *cl)
{
        (void) col;
        (void) board;
        (void) p1;

        /* if the row number is zero, it resets the 9x1 array */
        if (row == 0) {
                UArray2_map_row_major(*(UArray2_T *)cl, clear_arr, NULL);
        }

        /* checks for duplicates */
        check_dupes(board, *(UArray2_T *)cl, p1);
}



/* void check_box(UArray2_T board)
 * Parameters: UArray2_T board - array representing the sudoku board
 * Returns: Nothing
 * Does: Goes to each 3x3 submap and creates a sequence containing the values 
 *       inside it to be checked for duplicates
 *
 */
void check_box(UArray2_T board)
{
        /* hits every submap, one per iteration of inner loop */
        for (int i = 3; i <= UArray2_width(board); i += 3) {                 
                for (int j = 3; j <= UArray2_height(board); j += 3) {
                        /* new sequence initialized to be passed for each 
                           check of a submap */         
                        Seq_T seq = Seq_new(9);                              
                        seq = check_inside_box(i, j, board, seq);              
                        check_box_dupes(board, seq);                          
                }
        }                                
}



/* Seq_T check_inside_box(int col, int row, UArray2_T board, Seq_T seq)
 * Parameters: int col - the column value of the index pair
 *             int row - the row value of the index pair
 *             UArray2_T board - The array representing the sudoku board
 *             Seq_T seq - The sequence to be filled
 * Returns: Seq_T object containing the int's in one of the nine 3x3 boxes of
 *          the sudoku board
 * Does: Iterates through one of the nine 3x3 boxes important in evaluating a 
 *       correct sudoku board, and returns a sequence of those int's to be 
 *       checked for duplicates
 *
 */
Seq_T check_inside_box(int col, int row, UArray2_T board, Seq_T seq)
{
        /* adds every element in a submap to a sequence to be returned */
        for (int i = col - 3; i < col; i++) {
                for (int j = row - 3; j < row; j++){
                        Seq_addlo(seq, UArray2_at(board, i, j));
                }
        }
        
        return seq;
}



/* void check_box_dupes(UArray2_T board, Seq_T seq)
 * Parameters: UArray2_T board - array representing the full sudoku board
 *             Seq_T seq - sequence to be checked for duplicates 
 * Returns: Nothing
 * Does: Checks the sequence holding the values in an individual 3x3 submap
 *       for any duplicates
 *
 */
void check_box_dupes(UArray2_T board, Seq_T seq)
{
        for (int i = 0; i < Seq_length(seq); i++) {
                for (int j = i+1; j < Seq_length(seq); j++) {
                  int val_i = *(int *)Seq_get(seq, i);
                  int val_j = *(int *)Seq_get(seq, j);
                        if (val_i == val_j){
                                /* frees both objects in case this case is 
                                   hit */
                                Seq_free(&seq);
                                UArray2_free(&board);
                                exit(EXIT_FAILURE);
                        } 
                }
        }
        /* frees the sequence in case there are no duplicates */
        Seq_free(&seq);
}



/* void check_dupes(UArray2_T board, UArray2_T check_arr, void *p1)
 * Parameters: UArray2_T board - array holding the sudoku board
 *             UArray2_T check_arr - holds a single row or column to be 
 *                                   checked
 *             void *p1 - holds vaue at check_arr(i, j)
 * Returns: Nothing
 * Does: Checks the array holding either a column or a row for any duplicates
 *
 */
void check_dupes(UArray2_T board, UArray2_T check_arr, void *p1)
{
        int cur = *(int *)p1;
        int *next = NULL;
        
        next = (int *)UArray2_at(check_arr, (cur - 1), 0);
        
        if (*next == 0){
                *next = 1;
        } else {
                UArray2_free(&check_arr);
                UArray2_free(&board);
                exit(EXIT_FAILURE);
        }
        
}



/* void clear_arr(int col, int row, UArray2_T board, void *p1, void *cl)
 * Parameters: int col - column value
 *             int row - row value
 *             UArray2_T board - array holding single row or column to be 
 *                               reset each time a new one is encountered 
 *             void *p1 - holds value at board(row, col)
 *             void *p2 - NULL
 * Returns: Nothing
 * Does: Resets the array holding a single row or column each time a new row or
 *       column is encountered by the mapping functions
 *
 */
void clear_arr(int col, int row, UArray2_T board, void *p1, void *cl)
{
        (void) col;
        (void) row;
        (void) board;
        (void) cl;
        
        assert(sizeof(*(int *)p1) == UArray2_size(board));
        
        /* used to reset each value in the array to 0 */
        *(int *)p1 = 0; 

}



/*                                                                         
 * void error_rdr(FILE *fp, Pnmrdr_T rdr, char *msg)                         
 * Parameters: FILE *fp - pointer to opened FILE object, to be closed if an   
 *                        error is encountered                                
 *            Pnmrdr_T rdr - Pnmrdr_T object passed to be freed if an error is 
 *                           encountered                                       
 *            char *msg - custom error message                                
 * Returns: Nothing                                                            
 * Does: When a file exists and a Pnmrdr_T object is created, but is in the    
 *       wrong format, or the Pnmrdr_T is empty, this function is called in    
 *       average_brightness in order to print an error message, close the file,
 *       free the Pnmrdr_T object, and exit the program with EXIT_FAILURE.     
 */ 
void error_rdr(FILE *fp, Pnmrdr_T rdr, UArray2_T board, char *msg)
{
        void *rdr_p = rdr;
        fprintf(stderr, "%s", msg);
        
        if (rdr_p != NULL) {
                Pnmrdr_free(&rdr);
        }
        if (fp != NULL) {
                fclose(fp);
        }
        if (board != NULL) {
                UArray2_free(&board);
        }
        exit(EXIT_FAILURE);
}



/*                                                                             
 * void error(FILE *fp, char *msg)                                        
 * Parameters: FILE *fp - pointer to opened FILE object, to be closed if an    
 *                        error is encountered                                
 *             char *msg - custom error message                                
 *    Returns: Nothing                                                         
 *    Does: When there are too many command line arguments provided, or the    
 *          file provided in either the command line arguments or stdin are    
 *          is empty, this function is called in proccess_input to close       
 *          any open file, print an error message the program with             
 *          EXIT_FAILURE.                                                      
 */ 
void error(FILE *fp, UArray2_T board, char *msg)
{
        fprintf(stderr, "%s", msg);
        if (fp != NULL) {
                fclose(fp);
        }
        if (board != NULL) {
                UArray2_free(&board);
        }
        exit(EXIT_FAILURE);
}
