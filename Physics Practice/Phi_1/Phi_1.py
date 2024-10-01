import math
print("By Nguyen Ba Phi (supported by Python GPT from OpenAI)")

### Homework I.1
print("\n \n Homework I.1")

# Number of terms to calculate
n_terms = 15
pi_true = math.pi

# Leibniz Series for Pi
def leibniz_series(n):
    pi_approx = 0
    for i in range(n):
        term = (-1) ** i / (2 * i + 1)
        pi_approx += term
        yield 4 * pi_approx

# Nilakantha Series for Pi
def nilakantha_series(n):
    pi_approx = 3
    sign = 1
    for i in range(1, n + 1):
        term = 4 * sign / (2 * i * (2 * i + 1) * (2 * i + 2))
        pi_approx += term
        sign *= -1
        yield pi_approx

# Basel Series for Pi
def basel_series(n):
    pi_square_approx = 0
    for i in range(1, n + 1):
        term = 6 / (i ** 2)
        pi_square_approx += term
        yield math.sqrt(pi_square_approx)

# Print series and differences
def print_series_results(series_gen, name):
    print(f"\n{name} approximation:")
    for i, pi_approx in enumerate(series_gen(n_terms), 1):
        print(f"Term {i}: {pi_approx:.15f}, Difference from pi: {abs(pi_approx - pi_true):.15f}")

# Leibniz Series
print_series_results(leibniz_series, "Leibniz Series")

# Nilakantha Series
print_series_results(nilakantha_series, "Nilakantha Series")

# Basel Series
print_series_results(basel_series, "Basel Series")




### Homework I.2
print("\n \n Homework I.2")
# Function to compute the Fibonacci numbers up to F_n
def fibonacci_sequence(n):
    fib = [0, 1]
    for i in range(2, n + 2):  # We need F_n+1 for R_n
        fib.append(fib[-1] + fib[-2])
    return fib

# Function to compute the ratios R_n = F_n / F_n+1 and differences R_n - R_n-1
def fibonacci_ratios(n):
    fib = fibonacci_sequence(n)
    ratios = []
    
    for i in range(1, n+1):
        R_n = fib[i] / fib[i+1]
        ratios.append(R_n)
        
        if i > 1:
            R_diff = R_n - ratios[i-2]
            print(f"R_{i} = {R_n:.10f}, R_{i} - R_{i-1} = {R_diff:.2e}")
        else:
            print(f"R_{i} = {R_n:.10f}, R_{i} - R_{i-1} = N/A")

# a) Output the Fibonacci ratios for n up to 40
print("\nSection A")
fibonacci_ratios(40)

print("\nSection B")
print("""
Proof Sketch for Limiting Value of R_n:

The Fibonacci sequence satisfies the recurrence relation:
    F_{n+2} = F_{n+1} + F_n

Dividing both sides by F_{n+1}, we get:
    F_{n+2} / F_{n+1} = 1 + F_n / F_{n+1}

Let R_n = F_n / F_{n+1}. As n approaches infinity, the ratios R_n stabilize, 
meaning R_n ≈ R_{n+1}. Thus, we get:

    R = 1 / (1 + R)

Solving the equation:
    R^2 + R - 1 = 0

This is a quadratic equation, and using the quadratic formula:
    R = (-b + sqrt(b^2 - 4ac)) / 2a

where a = 1, b = 1, and c = -1. Solving this:

    R = (-1 + sqrt(5)) / 2

This gives us the inverse of the golden ratio:
    R = 1 / phi ≈ 0.61803398875

Therefore, the limiting value of R_n as n → ∞ is approximately 0.61803398875.
""")





### Homework I.3
print("\n \n Homework I.2")
def pascal_triangle(depth):
    # Create a 2D list to store the triangle
    triangle = [[1]]  # Start with the top of the triangle
    
    for n in range(1, depth):
        row = [1]  # Start each row with 1
        for k in range(1, n):
            # Each element is the sum of the two elements directly above it
            row.append(triangle[n-1][k-1] + triangle[n-1][k])
        row.append(1)  # End each row with 1
        triangle.append(row)
    
    # Pretty-print the triangle
    for row in triangle:
        print(' '.join(map(str, row)).center(depth * 2))

# a) Run for a depth of 32
print("\nSection A")
pascal_triangle(32)


def pascal_triangle_mod2(depth):
    # Create a 2D list to store the triangle
    triangle = [[1]]  # Start with the top of the triangle
    
    for n in range(1, depth):
        row = [1]  # Start each row with 1
        for k in range(1, n):
            # Each element is the sum of the two elements directly above it, mod 2
            row.append((triangle[n-1][k-1] + triangle[n-1][k]) % 2)
        row.append(1)  # End each row with 1
        triangle.append(row)
    
    # Pretty-print the triangle using " " for 0 and "█" for 1
    for row in triangle:
        print(' '.join('\u2588' if x == 1 else ' ' for x in row).center(depth * 2))

# b) Run for a depth of 32
print("\nSection B")
pascal_triangle_mod2(32)


def pascal_triangle_mod3(depth):
    # Create a 2D list to store the triangle
    triangle = [[1]]  # Start with the top of the triangle
    
    for n in range(1, depth):
        row = [1]  # Start each row with 1
        for k in range(1, n):
            # Each element is the sum of the two elements directly above it, mod 3
            row.append((triangle[n-1][k-1] + triangle[n-1][k]) % 3)
        row.append(1)  # End each row with 1
        triangle.append(row)
    
    # Pretty-print the triangle using symbols for 0, 1, and 2
    for row in triangle:
        print(' '.join('\u2588' if x == 1 else '\u2591' if x == 2 else ' ' for x in row).center(depth * 2))

# d) Run for a depth of 32
print("\nSection D")
pascal_triangle_mod3(32)
