/* Feb. 10th, 2020
 * Brian Savage and Robert Lester
 * bsavag01         rleste01
 * HW 2 - iii
 * unblackedges.c
 * Does: Unblackedges takes in a pbm file from either the command line or stdin
 *       and removes all blackedges from the pbm file and prints the unedged
 *       image to terminal following the format specifications of a plain
 *       pbm file 
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "pnmrdr.h"
#include <except.h>
#include "bit2.h"
#include "seq.h"

const int BLACK_PIXEL = 1;
const int WHITE_PIXEL = 0;

typedef struct Index {
        int col;
        int row;
} Index;

Bit2_T open_file(FILE *fp, Bit2_T img_map, int argc, char *argv[]);
Bit2_T set_bit_array(FILE *fp, Bit2_T img_map);
void remove_black_edges(Bit2_T img_map);
void check_border(Bit2_T img_map, Seq_T seq);
int is_black_edge(Bit2_T img_map, int col, int row);
Index *new_index(int col, int row);
void find_edges(Bit2_T img_map,Seq_T seq);
void find_next_edge(Bit2_T img_map, Seq_T seq, int col, int row);
void print_as_pbm(Bit2_T img_map);
void print_bit(int col, int row, Bit2_T img_map, int bit, void *cl);
void error(char* msg, Bit2_T img_map, FILE *fp);

int main(int argc, char *argv[]) 
{
        FILE *fp = NULL;
        Bit2_T img_map = NULL;

        /* opens file from stdin or command line argument */
        img_map = open_file(fp, img_map, argc, argv);
        
        /* Removed blackedges from img_map and store new bitmap as new_map */
        remove_black_edges(img_map); 

        /* Prints as a plain pbm to terminal */
        print_as_pbm(img_map);

        /* Freeing memory for img_map and maps */
        Bit2_free(&img_map);
        return EXIT_SUCCESS;
}

/* Bit2_T open_file(FILE *fp, Bit2_T img_map, int argc, char *argv[])
 * Parameters: [FILE *fp] - file pointer that will be initialized
 *             [Bit2_T img_map] - the bitmap to be initialized by the 
 *                               set_bit_array function
 *             [int argc] - integer representing the argument
 *             [char *argv[]] - passed command line arguments
 *    Returns: Bit2_T, initialized bitmap from passed pbm file 
 *       Does: Initializes the map with the passed pbm from either the command
 *             line or stdin
 */
Bit2_T open_file(FILE *fp, Bit2_T img_map, int argc, char *argv[]) 
{
        if (argc > 2) {
                error("Error: invalid command line arguments\n", NULL, NULL);
        } else {
                if (argc == 2) { /* pbm file passed as command line argument */
                        fp = fopen(argv[1], "rb");
                        if (fp == NULL) {
                                error("Error: unable to open file\n", 
                                      NULL, NULL);
                        }
                        /* Initializes img_map to bitMap inside passed file */
                        img_map = set_bit_array(fp, img_map);
                } else { /* pbm file collected from stdin */
                        fp = stdin;
                        if (fp == NULL) {
                                error("Error: unable to open file\n",
                                      NULL, NULL);
                        }       
                        /* Initializes img_map to bitMap inside stdin file */
                        img_map = set_bit_array(fp, img_map);
                }
        }
        if (img_map == NULL) { /* Check for error with bitmap creation */
                error("Error: cannot create bitmap imgMap\n", NULL, fp);
        }
        return img_map;
}

/* Bit2_T set_bit_array(FILE *fp,  Bit2_T img_map)            
 * Parameters: [FILE *fp] - pointer to the file the reader will be initialized
 *                          to
 *             [Bit2_T img_map] - the bitmap that will be initialized with the
 *                               pbm inside the file 
 *    Returns: Bit2_T, the initialized bitmap from the passed file parameter
 *       Does: Initializes the passed img_map with the bit contents of the pbm 
 *             file
 */
Bit2_T set_bit_array(FILE *fp,  Bit2_T img_map)
{
        Pnmrdr_mapdata data;
        Pnmrdr_T rdr;

        /* Initialize reader */
        TRY;
                rdr = Pnmrdr_new(fp);
        /* Handles error if reader is badformated */
        EXCEPT(Pnmrdr_Badformat);
                error("Error: bad format, could not read image\n", img_map, 
                      fp);
        /* Handles error if bit count from reader is inconsistent */  
        EXCEPT(Pnmrdr_Count);
                error("Error: bad format, could not read bit pixels\n", 
                      img_map, fp);
        END_TRY;
        
        /* Get pbm width and height */
        data = Pnmrdr_data(rdr);        
        if (data.width == 0 || data.height == 0 || data.type != Pnmrdr_bit) {
                Pnmrdr_free(&rdr);
                error("Error: image not valid dimensions or type (requires PBM"
                      "file)\n", img_map, fp);
        }
        /* Initialize a new bitMap with the same height and width of the 
           original passed pbm */
        img_map = Bit2_new(data.width, data.height);
        for (int j = 0; j < img_map->height; j++) {
                for (int i = 0; i < img_map->width; i++) {
                        Bit2_put(img_map, i, j, Pnmrdr_get(rdr));
                }
        }

        fclose(fp);
        Pnmrdr_free(&rdr);
        return img_map;
}

/* void remove_black_edges(Bit2_T img_map)
 * Parameters: [Bit2_T img_map] - the bitmap from the originally passed file
 *    Returns: Nothing
 *       Does: First indexs border of img_map, then finds additional black
 *             edges branching from the border, finally frees the sequence
 *             of coordinates
 */
void remove_black_edges(Bit2_T img_map)
{
        Seq_T seq = Seq_new(1);
        /* collects border black edges in seq */
        check_border(img_map, seq);
        /* finds branching edges from the border blak edges */
        find_edges(img_map, seq);
        Seq_free(&seq);
}

/* void find_edges(Bit2_T img_map, Seq_T seq)
 * Parameters: [Bit2_T img_map] - the bitmap from the originally passed file 
 *             [Seq_T seq] - the sequence of border blackedge indexs
 *    Returns: Nothing
 *       Does: converts blackedges into white pixels by adding black edge
 *             branchs to the black edge index sequence, then updates the 
 *             lowest black edge index from the sequence to a white pixel 
 */
void find_edges(Bit2_T img_map, Seq_T seq)
{
        /* iterates until the sequence of coordinates to check and change
           is empty */
        while (Seq_length(seq) > 0) {
                Index *check_index = Seq_remlo(seq);
                int col = check_index->col;
                int row = check_index->row;
                if (Bit2_get(img_map, col, row) == BLACK_PIXEL) {
                        /* check for branching edges  */
                        find_next_edge(img_map, seq, col - 1, row);
                        find_next_edge(img_map, seq, col + 1, row);
                        find_next_edge(img_map, seq, col, row - 1);
                        find_next_edge(img_map, seq, col, row + 1);
                        /* change bit to white pixel */
                        Bit2_put(img_map, col, row, WHITE_PIXEL);
                }
                /* free index gotten from sequence */
                free(check_index);
        }
}

/* void find_next_edge(Bit2_T img_map, Seq_T seq, int col, int row)
 * Parameters: [Bit2_T img_map] - the map to check for black edges
 *             [Seq_T seq] - sequence for adding black edge indexs
 *             [int col] - col index
 *             [int row] - row index
 *    Returns: Nothing
 *       Does: checks bit at index row and column and if it is a black edge
 *             it adds this bits index to the seq of black edge indexs
 */
void find_next_edge(Bit2_T img_map, Seq_T seq, int col, int row)
{
        if (col > 0 && col < img_map->width - 1 && 
            row > 0 && row < img_map->height - 1) {
                /* Not a border pixel */
                if (is_black_edge(img_map, col, row) == BLACK_PIXEL) {
                        Seq_addhi(seq, new_index(col, row));
                }
        } 
}

/* void check_border(Bit2_T img_map, Seq_T seq)
 * Parameters: [Bit2_T img_map] - unmodified bitmap gotten from file
 *             [Seq_T seq] - initially empty, in function the indexs of the
 *                           black edges on the border are added to the this
 *                           sequence
 *    Returns: Nothing
 *       Does: collects indexs of blackedges on the border of the bitmap in
 *             seq
 */
void check_border(Bit2_T img_map, Seq_T seq)
{
        for (int i = 0; i < img_map->width; i++) {
                /* Search top border */
                if (Bit2_get(img_map, i, 0) == BLACK_PIXEL) {
                        Seq_addhi(seq, new_index(i, 0));
                }
                /* Search bottom border */
                if (Bit2_get(img_map, i, img_map->height - 1) == BLACK_PIXEL) {
                        Seq_addhi(seq, new_index(i, img_map->height - 1));
                }
        }
        for (int j = 0; j < img_map->height; j++) {
                /* Search left border */
                if (Bit2_get(img_map, 0, j) == BLACK_PIXEL) {
                        Seq_addhi(seq, new_index(0, j));
                }
                /* Search right border */
                if (Bit2_get(img_map, img_map->width - 1, j) == BLACK_PIXEL) {
                        Seq_addhi(seq, new_index(img_map->width - 1, j));
                }      
        }
}

/* int is_black_edge(Bit2_T img_map, int col, int row)
 * Parameters: [Bit2_T img_map] - bitmap that will have bit checked from at
 *                               index col and row
 *             [int col] - col index for bitmap
 *             [int row] - row index for bitmap
 *    Returns: WHITE_PIXEL if current bit is not black edges
 *             BLACK_PIXEL if current bit is a black edge
 *       Does: Check description inside body of function 
 */
int is_black_edge(Bit2_T img_map, int col, int row)
{
        /* -------------------------------   Checked
         * |   Not   |   (2)   |   Not   |       One or more of the four
         * | Checked | Checked | Checked |       checked pixels needs to be a 
         * |   bit   |   bit   |   bit   |       black edges for the current
         * |------------------------------       bit to be considered a 
         * |   (1)   |Currently|   (3)   |       black edge
         * | Checked | On this | Checked |      
         * |   bit   |   bit   |   bit   |   Currently On this Bit
         * |------------------------------       This bit needs to be a black
         * |   Not   |   (4)   |   Not   |       pixel for this bit to be
         * | Checked | Checked | Checked |       considered a black edge
         * |   bit   |   bit   |   bit   |       and for the conditions of
         * -------------------------------       checked to be true
         */
        if ((Bit2_get(img_map, col - 1, row) == BLACK_PIXEL ||  /* (1) */
             Bit2_get(img_map, col + 1, row) == BLACK_PIXEL ||  /* (3) */
             Bit2_get(img_map, col, row - 1) == BLACK_PIXEL ||  /* (2) */
             Bit2_get(img_map, col, row + 1) == BLACK_PIXEL) && /* (4) */
             Bit2_get(img_map, col, row)     == BLACK_PIXEL) {
                return BLACK_PIXEL;
        }
        return WHITE_PIXEL;
}

/* Index *new_index(int col, int row)
 * Parameters: [int col] - value to be initialized to the Index struct data
 *                         member col 
 *             [int row] - value to be initialized to the Index struct data
 *                         member row
 *    Returns: a pointer to an initialized Index object
 *       Does: initializes an index object
 */
Index *new_index(int col, int row) 
{
        Index *set = malloc(sizeof(Index *));
        if (set == NULL) {
                fprintf(stderr, "Error: memory allocation failed.\n");
                exit(EXIT_FAILURE);
        }
        set->col = col;
        set->row = row;
        return set;
}



/* void print_as_pbm(Bit2_T img_map)
 * Parameters: [Bit2_T img_map] - the bitmap to be printed
 *    Returns: Nothing
 *       Does: Prints the passed bitmap using row major indexing
 */
void print_as_pbm(Bit2_T img_map)
{
        printf("P1\n# Black Edges Removed\n%d %d\n", 
               img_map->width, img_map->height);
        /* Indexs the imgMap using row major indexing */
        Bit2_map_row_major(img_map, print_bit, NULL);
}

/* void print_bit(int col, int row, Bit2_T img_map, int bit, void *cl)
 * Parameters: [int col] - col index for img_map
 *             [int row] - row index for img_map
 *             [Bit2_T img_map] - the bitmap containing the bits to be printed
 *             [int bit] - the bit value to be printed
 *             [void *cl] - currently line length
 *    Returns: Nothing
 *       Does: Prints the bit at index row and col from the img_map, and adds  
 *             a space or newline character (when line length reached 70
 *             total characters include spaces and the line terminating new
 *             line character
 */
void print_bit(int col, int row, Bit2_T img_map, int bit, void *cl)
{
        (void) row;
        if (cl == NULL) {
                int *temp = cl;
                *temp = 0;
        }

        int *linelen = (int *)cl;
        /* Print bit at index col and row */
        printf("%d", bit);
        (*linelen)++;

        /* Print a new line at the end of row or if 35 bits have been printed
           in the current line */
        if ((col + 1) == img_map->width || *linelen == 35) {
                /* When the end of the line or the length of the line reaches 
                   69 total characters add a newline to the end of the line 
                   and reset the line length to zero */
                *linelen = 0;
                printf("\n");
        } else {
                printf(" ");    
        }
}

/* void error(char *msg, Bit2_T img_map, FILE *fp)
 * Parameters: [char *msg] - the error message to be printed when error occurs
 *             [Bit2_T img_map] - img_map will be freed if not null
 *             [FILE *fp] - file pointer will be closed if not null
 *    Returns: Nothing (Ends with EXIT_FAILURE) after freeing allocated memory
 *       Does: Frees up memory during the occurance of a error, prints message
 *             to terminal with the passed error message, and ends the program
 *             with exit failure
 */
void error(char *msg, Bit2_T img_map, FILE *fp)
{
        /* Print error message to stderr */
        fprintf(stderr, "%s", msg);
        /* Frees img_map if initialized */
        if (img_map != NULL) {
                Bit2_free(&img_map);
        }
        /* Closes file if open */
        if (fp != NULL) {
                fclose(fp);
        }
        /* Close with EXIT_FAILURE */
        exit(EXIT_FAILURE);
}
