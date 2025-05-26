#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <x86intrin.h>

#define UNROLL 4
#define BLOCKSIZE 32

void do_block(size_t n, size_t si, size_t sj, size_t sk, double* A, double* B, double* C) {
    for (size_t i = si; i < si + BLOCKSIZE; i += UNROLL * 4) {
        for (size_t j = sj; j < sj + BLOCKSIZE; j++) {
            __m256d c[UNROLL];
            for (int x = 0; x < UNROLL; x++) {
                c[x] = _mm256_load_pd(C + i + x * 4 + j * n);
            }

            for (size_t k = sk; k < sk + BLOCKSIZE; k++) {
                __m256d b = _mm256_broadcast_sd(B + k + j * n);
                for (int x = 0; x < UNROLL; x++) {
                    c[x] = _mm256_add_pd(
                        c[x],
                        _mm256_mul_pd(
                            _mm256_load_pd(A + n * k + x * 4 + i),
                            b
                        )
                    );
                }
            }

            for (int x = 0; x < UNROLL; x++) {
                _mm256_store_pd(C + i + x * 4 + j * n, c[x]);
            }
        }
    }
}

void dgemm(size_t n, double* A, double* B, double* C) {
    for (size_t sj = 0; sj < n; sj += BLOCKSIZE)
        for (size_t si = 0; si < n; si += BLOCKSIZE)
            for (size_t sk = 0; sk < n; sk += BLOCKSIZE)
                do_block(n, si, sj, sk, A, B, C);
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
    int matrix_sizes[] = {32, 64, 128, 256, 512, 1024, 2048, 4096, 8192};
    int repetitions = 10;

    for (int s = 0; s < sizeof(matrix_sizes) / sizeof(matrix_sizes[0]); s++) {
        int size = matrix_sizes[s];
        double* times = (double*)malloc(repetitions * sizeof(double));

        for (int r = 0; r < repetitions; r++) {

            double *A, *B, *C;
            size_t num_elements = (size_t)size * size;
            size_t total_bytes = num_elements * sizeof(double);
            int alignment = 32;

            // Using aligned_alloc for AVX-compatible memory alignment
            A = (double*)aligned_alloc(alignment, total_bytes);
            B = (double*)aligned_alloc(alignment, total_bytes);
            C = (double*)aligned_alloc(alignment, total_bytes);

            for (size_t i = 0; i < num_elements; i++) {
                A[i] = (double)rand() / RAND_MAX;
                B[i] = (double)rand() / RAND_MAX;
                // Matrix C is allocated but not explicitly initialized here, matching original behavior
                // where dgemm loads from C's initial (uninitialized) state after allocation.
            }


            struct timespec start, end;
            clock_gettime(CLOCK_MONOTONIC, &start);
            dgemm(size, A, B, C);
            clock_gettime(CLOCK_MONOTONIC, &end);

            double time_taken = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) * 1e-9;
            times[r] = time_taken;
            printf("Matrix Size: %d, Run: %d, Time Taken: %.9f s\n", size, r, time_taken);

            free(A);
            free(B);
            free(C);
        }

        save_to_csv(size, times, repetitions);
        free(times);
    }

    return 0;
}

