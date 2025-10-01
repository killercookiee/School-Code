import numpy as np
import matplotlib.pyplot as plt
from typing import Tuple
from ipywidgets import interact

# Lorenz Attractor
def generate_data(nr_iter: int, dt: float) -> Tuple[np.ndarray, np.ndarray, np.ndarray]:
    x = np.zeros(nr_iter)
    y = np.zeros(nr_iter)
    z = np.zeros(nr_iter)

    # Initial conditions
    x[0], y[0], z[0] = 0, 1, 1.05

    for i in range(1, nr_iter):
        x[i] = x[i-1] + dt * (10 * (y[i-1] - x[i-1]))
        y[i] = y[i-1] + dt * (28 * x[i-1] - y[i-1] - x[i-1] * z[i-1])
        z[i] = z[i-1] + dt * (x[i-1] * y[i-1] - (8/3) * z[i-1])

    return x, y, z


def graph_lorenz_attractor(color: str, X: int, R: float):
    # Precompute trajectory
    x, y, z = generate_data(X, R)

    # Interactive plotting function
    def update(step=1000):
        fig = plt.figure(figsize=(10, 7))
        ax = fig.add_subplot(111, projection='3d')
        ax.plot(x[:step], y[:step], z[:step], color=color, lw=0.7)

        ax.set_title(f"Lorenz Attractor (dt={R}, steps={step}/{X})")
        ax.set_xlabel("X axis")
        ax.set_ylabel("Y axis")
        ax.set_zlabel("Z axis")

        plt.show()

    # Slider: from 1 → X
    interact(update, step=(1, X, 100))


# Example run
graph_lorenz_attractor("blue", 10000, 0.01)
