#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <malloc.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>

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
	arr[N * (N - 1)] = 20;        //Заполнение "Углов"
	arr[N * N -1] = 30;


	//#pragma acc parallel loop
	for (int i = 1; i < N; i++) {
		arr[i] = arr[0] + step * i;
		arr[N * (N - 1) + i] = arr[N - 1] + step * i;        //Заполнение "Рамки"
		arr[(N * i)] = arr[0] + step * i;
		arr[N - 1 + i * N] = arr[N - 1] + step * i;
	}

	memcpy(arr_new, arr, N * N * sizeof(double));                //Копия изначального массива

	int size = N * N;
	int iter = 0;
	clock_t start = clock();
	#pragma acc enter data copyin(arr[0:size],arr_new[0:size], error)
	for (; ((iter < ITER_MAX) && (error > accuracy)) ; iter++) {
#pragma acc kernels async(1)
		{
			error = 0.0;
		}
#pragma acc update device (error) async(1)
		#pragma acc data present(array,arraynew, error)
		#pragma acc parallel loop independent collapse(2) vector vector_length(256) gang num_gangs(256) reduction(max:error) async(1)
		for (int i = 1; i < N-1; i++) {
			for (int j = 1; j < N-1; j++) {
				int n = i * N + j;
				arr_new[n] = 0.25 * (arr[n - 1] + arr[n + 1] + arr[(i - 1) * N + j] + arr[(i + 1) * N + j]);  // Заполнение самого массива значаниями
				error = fmax(error, (arr_new[n] - arr[n]));  // Вычисление ошибки
			}
			
		}
		#pragma acc update host(error) async(1)
#pragma acc wait(1)
		double* temp = arr;
		arr = arr_new;
		arr_new = temp;
	}
	clock_t end = clock();

	printf("%0.15lf, %d\n", error, iter);
	printf("%lf\n", 1.0 * (end - start) / CLOCKS_PER_SEC);    // Вывод ошибки, кол-ва итераций и времени работы алгоритма


	free(arr);
	free(arr_new);   // Очистка памяти 

	return 0;
}
