import numpy as np
import matplotlib.pyplot as plt

print("By Nguyen Ba Phi")


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
