import numpy as np
import matplotlib.pyplot as plt

g = 9.81  # Gravity (m/s^2)
v0_kmh = 100  # Initial velocity in km/h
v0 = v0_kmh * 1000 / 3600  # Convert velocity to m/s
h0 = 15  # Initial height (m)
angles = np.linspace(10, 80, 15)  # Angles in degrees

# Time array
t = np.linspace(0, 10, 500)  # Time from 0 to 10 seconds

# Function to calculate trajectory
def trajectory(theta_deg):
    theta = np.radians(theta_deg)  # Convert angle to radians
    vx = v0 * np.sin(theta)  # Horizontal component of velocity
    vy = v0 * np.cos(theta)  # Vertical component of velocity
    
    # Calculate x and y as functions of time
    x = vx * t
    y = h0 + vy * t - 0.5 * g * t**2
    
    return x, y

# Plot the trajectories
plt.figure(figsize=(10, 6))

for theta_deg in angles:
    x, y = trajectory(theta_deg)
    plt.plot(x, y, label=f"{theta_deg:.0f}°")
    
# Formatting the plot
plt.title("Trajectories of the rock for different launch angles")
plt.xlabel("Horizontal distance (m)")
plt.ylabel("Vertical distance (m)")
plt.ylim(0, 60)  # Limit vertical axis to keep the plot readable
plt.xlim(0, 120)  # Limit horizontal axis
plt.legend(title="Launch Angle", loc="upper right")
plt.grid(True)
plt.show()
