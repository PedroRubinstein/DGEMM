#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Solução para o problema de compatibilidade de Windows
#ifdef _WIN32
#include <windows.h>
// Fallback implementation of clock_gettime
int clock_gettime(int dummy, struct timespec* ts) {
    LARGE_INTEGER freq, count;
    QueryPerformanceFrequency(&freq);
    QueryPerformanceCounter(&count);

    ts->tv_sec = count.QuadPart / freq.QuadPart;
    ts->tv_nsec = (long)((count.QuadPart % freq.QuadPart) * 1e9 / freq.QuadPart);
    return 0;
}
#define CLOCK_MONOTONIC 0
#endif

// Para poder usar _aligned_malloc, e assim funcionar com instruções AVX
#include <malloc.h>
#define _aligned_malloc(size, alignment) __mingw_aligned_malloc(size, alignment)
#define _aligned_free(ptr) __mingw_aligned_free(ptr)

#include <x86intrin.h>
void dgemm(int n, double* A, double* B, double* C) {
    for (size_t i = 0; i < n; i += 4) {
        for (size_t j = 0; j < n; j++) {
            __m256d c0 = _mm256_load_pd(C+i+j*n); // c0 = c[i][j]
            for (size_t k = 0; k < n; k++) {
                c0 =_mm256_add_pd(
                        c0, 
                        _mm256_mul_pd(
                            _mm256_load_pd(A+i+k*n),
                            _mm256_broadcast_sd(B+k+j*n)
                        )
                );
            }

            _mm256_store_pd(C+i+j*n, c0); // C[i][j] = c0
        }
    }
}

void save_to_csv(int size, double* times, int repetitions) {
    char filename[50];
    snprintf(filename, sizeof(filename), "results_c_%d.csv", size);

    FILE* file = fopen(filename, "w");
    if (file) {
        fprintf(file, "Matrix Size,Run,Time Taken (s)\n");
        for (int i = 0; i < repetitions; i++) {
            fprintf(file, "%d,%d,%.9f\n", size, i, times[i]);
        }
        fclose(file);
    }
}

int main() {
    int matrix_sizes[] = {4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192}; // Define matrix sizes. Tirei o 2, que não é divisível por 4
    int repetitions = 10;

    for (int s = 0; s < sizeof(matrix_sizes) / sizeof(matrix_sizes[0]); s++) {
        int size = matrix_sizes[s];
        double* times = (double*)malloc(repetitions * sizeof(double));

        for (int r = 0; r < repetitions; r++) {
            // Substituí malloc por _aligned_malloc, para funcionar com instruções AVX
            double* A = (double*) _aligned_malloc(size * size * sizeof(double), 32);
            double* B = (double*) _aligned_malloc(size * size * sizeof(double), 32);
            double* C = (double*) _aligned_malloc(size * size * sizeof(double), 32);
            
            srand(time(NULL));
            for (int i = 0; i < size * size; i++) {
                A[i] = (double) rand() / RAND_MAX;
                B[i] = (double) rand() / RAND_MAX;
            }

            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            dgemm(size, A, B, C);
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec)*1e-9;
            times[r] = time_taken;
            printf("Matrix Size: %d, Run: %d, Time Taken: %.9f s\n", size, r, time_taken);

            // Como dei _aligned_malloc, preciso dar _aligned_free
            _aligned_free(A);
            _aligned_free(B);
            _aligned_free(C);            
        }
        save_to_csv(size, times, repetitions);
        free(times);
    }

    return 0;
}
