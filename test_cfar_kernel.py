import numpy as np

data_input = np.load("test_data.npy")

N_x = 7
N_y = 7

# guard_x = 1
# guard_y = 1

# kernel = np.ones((N_x, N_y))
# kernel[2:5, 2:5] = 0

import matplotlib.pyplot as plt


#
# cfar_conv = np.fft.fft2(data_input) * 

cfar_conv = []

for i in range(32, 33):

    test_cells_half = 40
    N=100
    kernel = np.ones(N)
    kernel[test_cells_half:-test_cells_half] = 0

    conv = np.convolve(data_input[i,:], kernel) / np.sum(kernel)
    cfar_conv.append(conv)

    x = np.arange(data_input.shape[1])
    x_conv = np.arange(conv.shape[0])-N/2

    plt.plot(x_conv, conv)
    plt.plot(x, data_input[i,:])

plt.show()

plt.imshow(np.log10(np.abs(data_input)))
plt.show()
