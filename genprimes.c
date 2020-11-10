#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mpi.h>
#include <time.h>
#include <math.h>

// There is an error in this file when the n is too small 

void populate_array(char *local_array, int start, int end) {
    for (int i = 0; i + start <= end; i++) {
        int number_at_hand = i + start + 3;
        for (int j = 2; j < (number_at_hand/2) + 1; j++) {
            // printf("Number at hand: %i Divisor: %i\n", number_at_hand, j); 
            if (local_array[i] == 1 && number_at_hand % j == 0) {
                local_array[i] = 0;
                // printf("New: %i\n", local_array[i]); 
            }
        }
    }

}


int main(int argc, char *argv[]) {
    int rank, number_of_processes;
    int input = (unsigned int)atoi(argv[1]);
    int number_of_numbers = input - 2;
    char *final_output;
    char file_name[20] = "";
    FILE *file_pointer; 
    int start_time;
    int end_time; 

    if (rank == 0) {
        if(input <= 3) {
            strcpy(file_name, argv[1]); 
            strcat(file_name, ".txt");

            // printf("Filename is %s\n", file_name); 
            file_pointer = fopen(file_name, "w+t");

            if(file_pointer) {
                if(input >= 2) {
                    fprintf(file_pointer, "%i ", 2);  
                }
                if(input == 3) {
                    fprintf(file_pointer, "%i ", 3);                      
                }
                // Close the file pointer 
                fclose(file_pointer);

                return(0);
            }
            else{
                printf("Error occured, cannot create file.\n");
                exit(1);
            }

        }
    }


    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &number_of_processes);
    
    // Begin the clock 
    if(rank == 0) {
        start_time = MPI_Wtime();
        // printf("The start time is %i\n", start_time); 
    }

    if (rank == 0) {
        // Create an array with numbers 2 - input 
        // Handle 2 and 1 edge cases later 
        int *number_array = malloc(number_of_numbers * sizeof(int));
        int counter = 3;
        for (int i = 0; i < number_of_numbers; i ++) {
            number_array[i] = counter;
            counter++;
        }

        // for (int i = 0; i < (input-2); i++) {
        //     printf("Integer: %i\n", number_array[i]);
        // }

    }

    // Divide the task 
    int start, end;

    float estimate = (float)number_of_numbers / (float)number_of_processes;
    float rem = remainder(estimate, floor(estimate));
    int determined_allocation;
    if (rem < 0) {
        rem *= -1;
    }
    // printf("Remainder is %f\n", rem);  
    if (rem != 0.0) {
        // printf("This is not round\n"); 
        if(rem < 0.5) {
            determined_allocation = floor(estimate);
        }
        else {
            determined_allocation = ceil(estimate);
        }
        // printf("Estimate is %f\n", estimate);
        // printf("determined allocation is %i\n", determined_allocation); 

        start = determined_allocation * rank;
        if (rank != number_of_processes - 1) {
            end = start + determined_allocation - 1;
        }
        else {
            end = input - 3;
        }
    }
    else{
        determined_allocation = (unsigned int)(estimate);
        // printf("determined allocation is %i\n", determined_allocation);  
        start = determined_allocation * rank;
        if (rank != number_of_processes - 1) {
            end = start + determined_allocation - 1;
        }
        else {
            end = input - 3;
        }
    }


    int local_size = end - start + 1;

    // printf("Process %i Start: %i End: %i Size: %i\n", rank, start, end, local_size); 

    char *local_array = malloc(local_size * sizeof(char));
    memset(local_array, 1, local_size);

    // for (int i = 0; i < local_size; i++) {
    //     printf("%i ", local_array[i]);
    // }
    // printf("\n");


    // Now populate the array 
    populate_array(local_array, start, end);
    
    // printf("Process %i ", rank);
    // for (int i = 0; i < local_size; i++) {
    //     printf("%i ", local_array[i]);
    // }
    // printf("\n");




    // printf("Process %i: Start %d End %d\n", rank, start, end); 

    // printf("Estimate: %f\n", estimate); 

    // Process 1 not creates an empty array to recieve data in 
    if (rank == 0) {
        final_output = malloc(sizeof(char) * (input));
        memset(final_output, 1, number_of_numbers);
        // printf("Number of slots: %i\n", sizeof(final_output) / sizeof(final_output[1])); 
    }

    MPI_Gather(
        local_array,       /* data sent */
        local_size,    /* sent buffer size */
        MPI_CHAR, /* sent datatype */
        final_output,        /* recv buffer */
        local_size,    /* recv size */
        MPI_CHAR, /* recv datatype */
        0,        /* root */
        MPI_COMM_WORLD /* communicator */
    );    

    MPI_Finalize();

    if (rank == 0) {
        strcpy(file_name, argv[1]); 
        strcat(file_name, ".txt");

        // printf("Filename is %s\n", file_name); 
        file_pointer = fopen(file_name, "w+t");

        if(file_pointer) {
            // int final_counter = 1; 
            fprintf(file_pointer, "%i ", 2);  
            for (int i = 0; i < number_of_numbers; i++) {
                if (final_output[i] == 1) {
                    // final_counter++; 
                    // printf("%i ", i + 3);        
                    fprintf(file_pointer, "%i ", i + 3); 
                }
            }
            // printf("%i ", final_counter); 

            // Close the file pointer 
            fclose(file_pointer);
        }
        else{
            printf("Error occured, cannot create file.\n");
            exit(1);
        }

    }

    if(rank == 0) {
        end_time = MPI_Wtime();
        // printf("The end time is %i\n", end_time); 
    }
    

    return 0; 


}