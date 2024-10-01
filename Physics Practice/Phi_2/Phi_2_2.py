import numpy as np
import matplotlib.pyplot as plt

print("By Nguyen Ba Phi")


# Homework 2.2: Gaussian control parameters
def gaussian(x, mu, sigma):
    return (1/(np.sqrt(2 * np.pi) * sigma)) * np.exp(-(x - mu) ** 2 / (2 * sigma ** 2))

x = np.linspace(-10, 10, 400)

plt.figure(figsize=(8, 6))
# Plot for sigma changes
sigmas = np.linspace(1, 2, 5)
for sigma in sigmas:
    plt.plot(x, gaussian(x, 0, sigma), color='black')

# Plot for mu changes with sigma = 2
mus = np.linspace(0, 4, 4)
for mu in mus:
    plt.plot(x, gaussian(x, mu, 2), color='black', linestyle='dashed')

plt.xlabel("x")
plt.ylabel("Gaussians")
plt.title("Homework 2.2: Gaussian control parameters")
plt.show()
