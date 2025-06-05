from utilities import *

figsize = (19.20, 10.80) # Isso é 1/100 da resolução
fontsize_x = 30
fontsize_y = 20
fontsize_legend= 20
barwidth = 0.15

categories = ['Python', 'C', 'SIMD', 'Unrolling', 'Blocking']

# Ler dados (no C, peguei o -O3)

data_list = [
    Data((3.69, 0.00682, 0.00218, 0.00129, 0.00108), '256x256'),
    Data((30.2, 0.131, 0.034, 0.0127, 0.0093), '512x512'),
    Data((248, 2.2, 0.55, 0.134, 0.076), '1024x1024'),
    Data((2350, 80, 7, 2.41, 0.646), '2048x2048'),
    Data((19710, 870, 175.5, 49.31, 6.725), '4096x4096'),
    Data((173900, 7250, 1757, 431.6, 56.63), '8192x8192')
]

#log_data_list = [data.to_log() for data in data_list]

make_figure(data_list, categories, figsize, barwidth, fontsize_x, fontsize_y, fontsize_legend, 'Tempo de execução (segundos)', 'TempoLog', log_scale=True)

# Figura da performance
relative_data_list = []
for data in data_list:
    values = [data.values[0]/data.values[i] for i in range(len(data.values))] # Tempo do Python dividido pelo tempo do tamanho de matriz em questão

    new_data = Data(values, data.label)
    relative_data_list.append(new_data)
    
make_figure(relative_data_list, categories, figsize, barwidth, fontsize_x, fontsize_y, fontsize_legend, 'Performance relativa ao Python', 'PerformanceLog', log_scale=True)