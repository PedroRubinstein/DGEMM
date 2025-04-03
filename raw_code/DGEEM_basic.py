import time
import numpy as np
import pandas as pd

# Define the sizes of the matrices and the number of repetitions
matrix_sizes = [8192]
repetitions = 10

# Loop through each matrix size
for size in matrix_sizes:

    

    for a in range(repetitions):
        # Generate two random matrices of the current size, and a third matrix to store the result

        results = []

        matrix_a = np.random.rand(size, size)
        matrix_b = np.random.rand(size, size)
        matrix_c = np.zeros((size, size))

        # Measure the time taken for matrix multiplication
        start_time = time.perf_counter()

        for i in range(size):
            for j in range(size):
                for k in range(size):
                    matrix_c[i, j] += matrix_a[i, k] * matrix_b[k, j]

        end_time = time.perf_counter()

        # Append the results to the list
        results.append({"Matrix Size": size, "Time Taken (s)": end_time - start_time})

        #Since computation takes A LOT for bigger sizes of n, we store results step by step in case of a crash
        
        df = pd.DataFrame(results)
        df.to_csv(f"./results_python_{size}_{a}.csv", index=False)
        print("End of N:", size, a)