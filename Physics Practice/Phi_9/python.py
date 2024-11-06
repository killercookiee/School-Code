import numpy as np
from scipy.integrate import solve_ivp

rho = 1.293  # air density
C_d = 0.295  # drag coefficient
diameter = 0.00782  # m (7.82 mm)
A = np.pi * (diameter / 2) ** 2  # cross sectional area of bullet
m_bullet = 0.00603  # mass of bullet
v_initial = 1330 * 1000 / 3600  # initial velocity
target_height = 500  # height of the target

g = 9.81

def drag_force(v):
    return 0.5 * rho * C_d * A * v**2

def equations_with_theta(theta):
    def equations(t, y):
        x, y_pos, v_x, v_y = y
        v = np.sqrt(v_x**2 + v_y**2)
        F_drag = drag_force(v)
        F_drag_x = F_drag * (v_x / v)
        F_drag_y = F_drag * (v_y / v)
        dxdt = v_x
        dydt = v_y
        dvxdt = -F_drag_x / m_bullet
        dvydt = -g - F_drag_y / m_bullet
        return [dxdt, dydt, dvxdt, dvydt]
    return equations

def reach_target_height(t, y):
    return y[1] - target_height
reach_target_height.terminal = True

def minimum_distance(theta_degrees):
    theta = np.radians(theta_degrees)
    initial_conditions = [0, 0, v_initial * np.cos(theta), v_initial * np.sin(theta)]
    
    solution = solve_ivp(
        equations_with_theta(theta), [0, 100], initial_conditions,
        dense_output=True, events=reach_target_height
    )
    
    if solution.status == 1:
        distance = solution.y[0, -1]
        return distance
    else:
        return None

angle_1, angle_2 = None, None
for angle in range(1, 90):
    dist = minimum_distance(angle)
    if dist:
        if angle_1 is None:
            angle_1 = angle
        elif angle_2 is None:
            angle_2 = angle
            break

print(f"Two angles for hitting the target: {angle_1}° and {angle_2}°")
print(f"Minimum distance to hit the window with drag included: {minimum_distance(angle_1):.3f} m")