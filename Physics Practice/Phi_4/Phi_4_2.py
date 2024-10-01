import numpy as np
import matplotlib.pyplot as plt

# Constants
mu_s = 0.5  # Coefficient of static friction
g = 9.81  # Gravitational acceleration
r_initial = 0.3  # Initial radius from the center
m = 0.01  # Mass of the coin
r_max = 10  # Maximum radius

rpms = [38.8, 39.5, 41, 45]

def rpm_to_omega(rpm):
    return 2 * np.pi * rpm / 60

class Coin:
    def __init__(self, m, r_initial, omega):
        self.m = m  # Mass of the coin
        self.r = r_initial  # Radial position
        self.theta = 0  # Initial angular position
        self.v_r = 0  # Initial radial velocity
        self.v_theta = omega * r_initial  # Tangential velocity
        self.omega = omega  # Angular velocity
        self.friction = mu_s * m * g  # Max static friction

    def calculate_forces(self):
        F_centrifugal = self.m * self.omega**2 * self.r
        return F_centrifugal
    
    def update_velocity(self, dt):
        F_centrifugal = self.calculate_forces()
        if F_centrifugal > self.friction:
            a_r = (F_centrifugal - self.friction) / self.m
            self.v_r += a_r * dt
        self.v_theta = self.omega * self.r

    def update_position(self, dt):
        self.r += self.v_r * dt
        self.theta += self.omega * dt

    def get_position(self):
        x = self.r * np.cos(self.theta)
        y = self.r * np.sin(self.theta)
        return x, y

def simulate_coin_motion(rpm, dt=0.01):
    omega = rpm_to_omega(rpm)
    coin = Coin(m, r_initial, omega)
    
    x_vals = []
    y_vals = []
    
    while coin.r < r_max:
        x, y = coin.get_position()
        x_vals.append(x)
        y_vals.append(y)
        
        coin.update_velocity(dt)
        coin.update_position(dt)
    
    return x_vals, y_vals




plt.figure(figsize=(8, 8))
for rpm in rpms:
    x_vals, y_vals = simulate_coin_motion(rpm)
    plt.plot(x_vals, y_vals, label=f'RPM = {rpm}')

circle = plt.Circle((0, 0), r_max, color='r', fill=False, linestyle='--', label='Turntable Boundary (10 m)')
plt.gca().add_artist(circle)

plt.scatter([r_initial], [0], color='black', label='Start Point (0.3, 0)', zorder=5)

plt.xlabel('X Position (m)')
plt.ylabel('Y Position (m)')
plt.title('Trajectory of the Coin on the Turntable')
plt.legend()
plt.grid(True)
plt.axis('equal')
plt.show()
