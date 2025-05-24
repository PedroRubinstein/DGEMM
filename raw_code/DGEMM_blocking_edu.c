#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// Para poder usar _aligned_malloc, e assim funcionar com instruções AVX
#include <malloc.h>

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
#define _aligned_malloc(size, alignment) __mingw_aligned_malloc(size, alignment)
#define _aligned_free(ptr) __mingw_aligned_free(ptr)
#define CLOCK_MONOTONIC 0
#endif

#include <x86intrin.h>
#define UNROLL (4)
# define BLOCKSIZE 32

void do_block(
    size_t n, size_t si, size_t sj, size_t sk,
    double* A, double* B, double* C
) {
    for (size_t i = si; i < si + BLOCKSIZE; i += UNROLL*4) {
        for (size_t j = sj; j < sj + BLOCKSIZE; j++) {
            __m256d c[4];
            for (size_t x = 0; x < UNROLL; x++) {
                c[x] = _mm256_load_pd(C+i+x*4+j*n);
            }

            for (size_t k = sk; k < sk+BLOCKSIZE; k++) {
                __m256d b = _mm256_broadcast_sd(B+k+j*n);

                for (int x = 0; x < UNROLL; x++) {
                    c[x] = _mm256_add_pd(
                        c[x],
                        _mm256_mul_pd(
                            _mm256_load_pd(A+n*k+x*4+i),
                            b
                        )
                    );
                }
            }

            for (size_t x = 0; x < UNROLL; x++) {
                _mm256_store_pd(C+i+x*4+j*n, c[x]);
            }
        }
    }
}

void dgemm(size_t n, double* A, double* B, double* C) {
    for (size_t sj = 0; sj < n; sj += BLOCKSIZE) {
        for (size_t si = 0; si < n; si += BLOCKSIZE) {
            for (size_t sk = 0; sk < n; sk += BLOCKSIZE) {
                do_block(n, si, sj, sk, A, B, C);
            }
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
    int matrix_sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192}; // Define matrix sizes. O 2, que não é divisível por 4, quebra o paralelismo de subpalavra. 4 e 8, que não são divisíveis por 16, quebram o loop unrolling.
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
