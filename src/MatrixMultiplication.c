/*
 ============================================================================
 Name        : MatrixMultiplication.c
 Author      : Antonio Tino
 Version     : 3.0.0
 Copyright   : Your copyright notice
 Description : Multiplying the matrix A of size m x n
 	 	 	   by the matrix B of size n x l leads to obtaining the matrix C
 	 	 	   of size m x l with each matrix C element.
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "mpi.h"
#define N 3100
#define M 3100

int main(int argc, char* argv[]){
	int rank; /* rank of process */
	int n_processes; /* number of processes */
	int **A, **B, **C;
	int row, col; /*count rows and count columns */
	double start, finish;
	int index_proce, shift = 0, work, send[2];
	int i, j, k;

	srand(13);

	/* Return status for receive */
	MPI_Status status;

	/* Start up MPI */
	MPI_Init(&argc, &argv);

	/* Find out process rank */
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	/* Find out number of processes */
	MPI_Comm_size(MPI_COMM_WORLD, &n_processes);

	/*Start to estimates of computation time */
	printf("Start to estimates of computation time of processes %d of %d...\n", rank, n_processes);
	fflush(stdout);
	start = MPI_Wtime();

	/* Allocation for the matrix A*/
	A = malloc(N * sizeof(int *));
	for (row = 0; row < N; row++)
		A[row] = (int *) malloc(M * sizeof(int *));

	/*Insert elements into matrix A*/
	for (row = 0; row < N; row++)
		for (col = 0; col < M; col++)
			A[row][col] = rand() % 2;

	/* Allocation for the matrix B*/
	B = malloc(M * sizeof(int *));
	for (row = 0; row < M; row++)
		B[row] = (int *) malloc(N * sizeof(int *));

	/*Insert elements into matrix B*/
	for (row = 0; row < N; row++)
		for (col = 0; col < M; col++)
			B[row][col] = rand() % 2;

	if (rank ==0){
		/* Print of the matrix A and matrix B*/
		printf("MATRIX A\n");
		fflush(stdout);
		for (row = 0; row < N; row++) {
			for (col = 0; col < M; col++)
				printf("%d", A[row][col]);
			printf("\n");
		}
		printf("**********\n");
		printf("MATRIX B\n");
		fflush(stdout);
		for (row = 0; row < N; row++) {
			for (col = 0; col < M; col++)
				printf("%d", B[row][col]);
			printf("\n");
		}
		printf("**********\n");

		/* Allocation for the matrix C*/
		C = malloc(N * sizeof(int *));
		for (row = 0; row < N; row++)
			C[row] = (int *) malloc(M * sizeof(int *));

		/* if the one process is the master, the work  will be done from it*/
		if(n_processes == 1){
			for (i = 0; i < N; i++) {
				for (j = 0; j < M; j++) {
					C[i][j] = 0;
					for (k = 0; k < M; k++) {
						C[i][j] += A[i][k] * B[k][j];
					}
				}
			}
		}else{
			/*The master prepare the work and send it*/
			work = N/(n_processes-1);
			int rest=N%(n_processes-1);

			for(index_proce=0;index_proce<n_processes-1;index_proce++){
				if(rest>0){
					send[0]=work+1; /*number of rows*/
					rest--;
				} else
					send[0] = work; /*number of rows*/

				shift += send[0];
				send[1] = shift; /*shift left*/
				printf("Send the work a to worker %d of %d...\n", index_proce+1, n_processes-1);
				fflush(stdout);
				MPI_Send(send, 2, MPI_INT, index_proce+1, 99, MPI_COMM_WORLD);
			}

			/* Receive the work from worker, allocation the matrix C and print it */
			int row_start, row_finish;

			for(index_proce=0;index_proce<n_processes-1;index_proce++){
				MPI_Recv(&send, 2, MPI_INT, index_proce+1, 99, MPI_COMM_WORLD, &status);

				int *risultato;
				risultato= malloc(send[0]*M*sizeof(int*));

				MPI_Recv(risultato, send[0]*M, MPI_INT, index_proce+1, 99, MPI_COMM_WORLD, &status);

				row_start = send[1] - send[0];
				row_finish = send[1] - 1;

				int i=0;
				for (row = row_start; row <= row_finish; row++) {
					for (col = 0; col < M; col++) {
						C[row][col] = risultato[i];
					}
					i++;
				}
				free(risultato);
			}
		}

		/* Print of the matrix C
		printf("MATRIX C\n");
		fflush(stdout);
		for (row = 0; row < N; row++) {
			for (col = 0; col < M; col++)
				printf("%d", C[row][col]);
			printf("\n");
		}
		printf("**********\n");*/

		/*Release the resource*/
		free(C);

		/*End to estimates of computation time */
		printf("\n...End to estimates of computation time\n");
		fflush(stdout);
		finish = MPI_Wtime();
		printf("Computation time: %f\n", finish - start);
		fflush(stdout);
	}else{
		/*The worker receive the work from master*/
		MPI_Recv(&send, 2, MPI_INT, 0, 99, MPI_COMM_WORLD, &status);

		/*Makes the operation and send the result a the master*/
		int row_start = send[1] - send[0];
		int *RESULT;
		RESULT= malloc(send[0]*M*sizeof(int*));
		int z = 0;
		for(i=0; i<send[0]; i++){
			for(j=0; j<M; j++){
				RESULT[z] = 0;
				for(k=0; k<M; k++)
					RESULT[z] += A[row_start][k] * B[k][j];
				z++;
			}
			row_start++;
		}

		/*Send the result a the master*/
		MPI_Send(send, 2, MPI_INT, 0, 99, MPI_COMM_WORLD);
		MPI_Send(RESULT, send[0]*M, MPI_INT, 0, 99, MPI_COMM_WORLD);

		free(RESULT);
	}

	/*Release the resources*/
	free(A);
	free(B);

	/* shut down MPI */
	MPI_Finalize(); 
	
	return 0;
}
