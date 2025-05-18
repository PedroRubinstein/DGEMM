#include <stdio.h>
#include <stdlib.h> // For general utilities, rand(), srand(), aligned_alloc(), malloc(), free()
#include <time.h> // For clock_gettime(), time()
#include <x86intrin.h> // For AVX intrinsics

// Using aligned_alloc (from stdlib.h) for AVX-compatible memory alignment

#define UNROLL (4)

void dgemm(int n, double* A, double* B, double* C) {
    for (size_t i = 0; i < n; i += UNROLL * 4) {
        for (size_t j = 0; j < n; j++) {
            __m256d c[UNROLL]; 
            for (size_t x = 0; x < UNROLL; x++) {
                c[x] = _mm256_load_pd(C + (i + x * 4 + (size_t)j * n));
            }

            for (size_t k = 0; k < n; k++) {
                __m256d b = _mm256_broadcast_sd(B + (k + (size_t)j * n));
                for (size_t x = 0; x < UNROLL; x++) {
                    c[x] = _mm256_add_pd(
                        c[x],
                        _mm256_mul_pd(
                            _mm256_load_pd(A + ((size_t)n * k + x * 4 + i)),
                            b
                        )
                    );
                }
            }

            for (size_t x = 0; x < UNROLL; x++) {
                _mm256_store_pd(C + (i + x * 4 + (size_t)j * n), c[x]);
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
            fprintf(file, "%d,%d,%.9f\n", size, i + 1, times[i]);
        }
        fclose(file);
        // printf("Results saved to %s\n", filename); // Removed my added printf
    }
    // else { // Removed my added error handling
    // fprintf(stderr, "Error: Could not open file %s for writing.\n", filename);
    // }
}

int main() {
    // Define matrix sizes. O 2, que não é divisível por 4, quebra o paralelismo de subpalavra. 4 e 8, que não são divisíveis por 16, quebram o loop unrolling.
    int matrix_sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192}; 
    int repetitions = 10;

    srand(time(NULL));

    for (int s = 0; s < sizeof(matrix_sizes) / sizeof(matrix_sizes[0]); s++) {
        int size = matrix_sizes[s];
        double* times = (double*)malloc(repetitions * sizeof(double));

        // printf("Testing matrix size: %d x %d\n", size, size); // Removed my added printf

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
            printf("Matrix Size: %d, Run: %d, Time Taken: %.9f s\n", size, r + 1, time_taken);

            // Memory allocated with aligned_alloc must be freed with free
            free(A);
            free(B);
            free(C);            
        }
        save_to_csv(size, times, repetitions);
        free(times);
        // printf("\n"); // Removed my added printf
    }

    return 0;
}
