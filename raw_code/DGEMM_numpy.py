import time
import numpy as np
import pandas as pd

# Define the sizes of the matrices and the number of repetitions
matrix_sizes = [2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096, 8192]
repetitions = 10

# Loop through each matrix size
for size in matrix_sizes:
    for a in range(repetitions):
        results = []

        # Generate random matrices
        matrix_a = np.random.rand(size, size)
        matrix_b = np.random.rand(size, size)

        # Measure time using NumPy's optimized matrix multiplication
        start_time = time.perf_counter()
        matrix_c = np.dot(matrix_a, matrix_b)  # **Optimized NumPy multiplication**
        end_time = time.perf_counter()

        # Append results
        results.append({"Matrix Size": size, "Time Taken (s)": end_time - start_time})

    # Save results incrementally
    df = pd.DataFrame(results)
    df.to_csv(f"./results_python_numpy_{size}.csv", index=False)

    print(f"End of N: {size}, Run: {a}, Time: {end_time - start_time:.6f} s")
