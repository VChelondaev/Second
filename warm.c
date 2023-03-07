#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>

//#define ITER_MAX 1000000
//#define N 128

int main(int argc, char** argv) {

	double accuracy = 0.000001;
	int N, ITER_MAX;
	accuracy = atof(argv[1]);
	N = atoi(argv[2]);
	ITER_MAX = atoi(argv[3]);

	double* arr = (double*)calloc(N * N, sizeof(double));
	double* arr_new = (double*)calloc(N * N, sizeof(double));
	double step = 10.0 / (N-1);
	double error = 1.0;

	arr[0] = 10;
	arr[N-1] = 20;
	arr[N * (N - 1)] = 20;
	arr[N * N -1] = 30;


	for (int i = 1; i < N; i++) {
		arr[i] = arr[0] + step * i;
		arr[N * (N - 1) + i] = arr[N - 1] + step * i;
		arr[(N * i)] = arr[0] + step * i;
		arr[N - 1 + i * N] = arr[N - 1] + step * i;
	}

	memcpy(arr_new, arr, N * N * sizeof(double));

	int iter = 0;
	clock_t start = clock();
	for (; ((iter < ITER_MAX) && (error > accuracy)) ; iter++) {
		error = 0.0;
		for (int i = 1; i < N-1; i++) {
			for (int j = 1; j < N-1; j++) {
				int n = i * N + j;
				arr_new[n] = 0.25 * (arr[n - 1] + arr[n + 1] + arr[(i - 1) * N + j] + arr[(i + 1) * N + j]);
				//arr_new[N - n] = 0.25 * (arr[(N - n) - 1] + arr[(N - n) + 1] + arr[N - ((i - 1) * N + j)] + arr[N-((i + 1) * N + j)]);
				error = fmax(error, (arr_new[n] - arr[n]));
			}
			
		}
		double* temp = arr;
		arr = arr_new;
		arr_new = temp;
	}

	//for (int i = 0; i < N; i++) { 
	//	for (int j = 0; j < N; j++) {
	//		printf("%lf ", arr_new[i * N + j]);
	//	}
	//	printf("\n");
	//}


	printf("%lf, %d", error, iter);

	free(arr);
	free(arr_new);

	return 0;
}
