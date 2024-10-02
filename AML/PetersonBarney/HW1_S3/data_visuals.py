import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib.widgets import Slider

# Load the dataset
file_path = '/Users/killercookie/Documents/GitHub/School-Code/AML/PetersonBarney/HW1_S3/datasets/train1.csv'  # Change as needed
df = pd.read_csv(file_path)

# Convert phoneme column to string and remove leading stars from phonemes
df['phoneme'] = df['phoneme'].astype(str).str.lstrip('*')

# Define color mapping for phonemes
unique_phonemes = df['phoneme'].unique()
phoneme_colors = {phoneme: plt.cm.jet(i / len(unique_phonemes)) for i, phoneme in enumerate(unique_phonemes)}
df['color'] = df['phoneme'].map(phoneme_colors)

# Create a 3D scatter plot
fig = plt.figure()
ax = fig.add_subplot(111, projection='3d')

# Initial plot
scatter = ax.scatter(df['F0'], df['F1'], df['F2'], c=df['color'], marker='o')

# Axis labels
ax.set_xlabel('F0')
ax.set_ylabel('F1')
ax.set_zlabel('F2')

# Slider for F3 dimension
axcolor = 'lightgoldenrodyellow'
ax_f3 = plt.axes([0.15, 0.01, 0.65, 0.03], facecolor=axcolor)

# Adjusting the slider to step by 100
slider_f3 = Slider(ax_f3, 'F3', df['F3'].min(), df['F3'].max(), valinit=df['F3'].min(), valstep=100)

# Function to update the plot based on slider value
def update(val):
    ax.cla()  # Clear the current axes
    ax.set_xlabel('F0')
    ax.set_ylabel('F1')
    ax.set_zlabel('F2')
    
    # Calculate transparency based on distance from the slider value
    slider_value = round(slider_f3.val / 100) * 100  # Round slider value to nearest 100
    # Assuming a standard deviation for the normal distribution curve
    std_dev = 10  # You can adjust this value
    # Calculate transparency based on the normal distribution
    transparency = np.exp(-0.5 * ((df['F3'] - slider_value) / std_dev) ** 2)
    
    # Normalize transparency to be in [0, 1]
    transparency = np.clip(transparency, 0, 1)

    # Scatter plot with varying transparency
    scatter = ax.scatter(df['F0'], df['F1'], df['F2'], 
                         c=df['color'], marker='o', alpha=transparency)

    plt.draw()

# Connect the slider to the update function
slider_f3.on_changed(update)

# Show the plot
plt.show()
