import numpy as np
import matplotlib.pyplot as plt

print("By Nguyen Ba Phi")


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
