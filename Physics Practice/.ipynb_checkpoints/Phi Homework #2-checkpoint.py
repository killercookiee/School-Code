import numpy as np
import matplotlib.pyplot as plt

print("By Nguyen Ba Phi (Helped by ChatGPT4o)")


# Homework 2.1: Basic statistics with lists
def stat_with_lists(list_of_numbers):
    list_mean = np.mean(list_of_numbers)
    list_median = np.median(list_of_numbers)
    list_standard_deviation = np.std(list_of_numbers)
    
    return list_mean, list_median, list_standard_deviation

# Example usage:
numbers = [1, 2, 3, 4, 5, 6, 7, 8, 9]
mean, median, std_dev = stat_with_lists(numbers)
print("### Homework 2.1 Output ###")
print(f"Mean: {mean}, Median: {median}, Standard Deviation: {std_dev}\n")


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


# Homework 2.3: Approximate π by random numbers
def approximate_pi(N):
    # Generate random x and y points
    x = np.random.uniform(-1, 1, N)
    y = np.random.uniform(-1, 1, N)
    
    # Calculate distance from origin
    distance = np.sqrt(x**2 + y**2)
    
    # Count points inside the unit circle
    M = np.sum(distance < 1)
    
    # Approximate pi
    pi_approx = (4 * M) / N
    
    # Plot the points
    plt.figure(figsize=(6,6))
    plt.scatter(x, y, color='blue', s=1)
    
    # Plot the unit circle
    phi = np.linspace(0, 2 * np.pi, 400)
    plt.plot(np.cos(phi), np.sin(phi), color='red')
    
    plt.xlim(-1, 1)
    plt.ylim(-1, 1)
    plt.gca().set_aspect('equal', adjustable='box')
    plt.title(f'N = {N}, Approximated Pi = {pi_approx}')
    plt.show()
    
    return pi_approx

# Test for different values of N
print("### Homework 2.3 Output ###")
for N in [100, 1000, 10000, 100000]:
    pi_value = approximate_pi(N)
    print(f"Approximation of Pi for N = {N}: {pi_value}\n")
