import numpy as np
import matplotlib.pyplot as plt

# --- Data for Task 1 ---
degrees = np.array([0, 10, 20, 30, 40, 50, 60, 70, 80, 90, 
                    100, 110, 120, 130, 140, 150, 160, 170, 180, 190, 
                    200, 210, 220, 230, 240, 250, 260, 270, 280, 290, 
                    300, 310, 320, 330, 340, 350])

Umeasured_t1 = np.array([1030, 833, 593, 353, 161, 51, 23, 70, 186, 403, 
                         670, 944, 1180, 1400, 1525, 1518, 1434, 1258, 978, 764, 
                         536, 295, 121, 39, 27, 90, 280, 448, 692, 952, 
                         1152, 1364, 1466, 1461, 1414, 1225])

Ubackground = 21

# Corrected voltage for Task 1
U_corr_t1 = Umeasured_t1 - Ubackground

# --- Data for Task 2 ---
# TODO: Replace this zero array with your actual measured 36 data points for Task 2
Umeasured_t2 = np.zeros_like(Umeasured_t1) 
U_corr_t2 = Umeasured_t2 - Ubackground

# Convert degrees to radians for the polar plot
theta = np.radians(degrees)

# Close the plot loop by appending the first point to the end of the arrays
theta_closed = np.append(theta, theta[0])
U_corr_t1_closed = np.append(U_corr_t1, U_corr_t1[0])
U_corr_t2_closed = np.append(U_corr_t2, U_corr_t2[0])

# Plotting
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 6))

# Subplot 1: Linear Cartesian Plot (Corrected Voltage vs Angle)
ax1.plot(degrees, U_corr_t1, label='Task 1 (Film Polarizers)', color='blue', marker='o')
ax1.plot(degrees, U_corr_t2, label='Task 2 (LC Cell)', color='red', marker='s')
ax1.set_xlabel('Analyzer Angle (Degrees)')
ax1.set_ylabel('Corrected Voltage (mV)')
ax1.set_title('Corrected Voltage vs Analyzer Angle')
ax1.grid(True)
ax1.legend()

# Subplot 2: Polar Plot
# Recreate the second axes specifically as a polar projection
ax2.remove()
ax2 = fig.add_subplot(1, 2, 2, projection='polar')

ax2.plot(theta_closed, U_corr_t1_closed, label='Task 1', color='blue', marker='o')
ax2.plot(theta_closed, U_corr_t2_closed, label='Task 2', color='red', marker='s')
ax2.set_title("Polar Plot Comparison", va='bottom')
ax2.set_theta_zero_location("E")  # 0 degrees on the right
ax2.set_theta_direction(1)        # Counterclockwise
ax2.legend(loc='upper right', bbox_to_anchor=(1.3, 1.1))

plt.tight_layout()
plt.show()