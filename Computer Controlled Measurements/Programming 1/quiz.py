# Fibonacci

n1 = 1
n2 = 2
def fib(n):
    if n == 1:
        return n1
    elif n == 2:
        return n2
    else:
        return fib(n-1) + fib(n-2)
print(fib(20)-fib(17))



# 