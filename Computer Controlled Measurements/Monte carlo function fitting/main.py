import numpy as np
import random
import matplotlib.pyplot as plt
import os

# ==========================================
# Core Mathematical Functions
# ==========================================

def model_function(t, A, tau, T):
    """The target fit function: y(t) = A * e^(-t/tau) * sin(2*pi*t/T)"""
    return A * np.exp(-t / tau) * np.sin(2 * np.pi * t / T)

def calculate_energy(y_obs, y_fit):
    """Calculates the objective 'energy' (L1 norm / Mean Absolute Error)"""
    return np.sum(np.abs(y_obs - y_fit))

# ==========================================
# Monte Carlo Fitting Engine
# ==========================================

def run_mc_fit(data_x, data_y, init_params, increments, vary_flags, beta_schedule, iterations):
    params = init_params.copy()
    temp_params = params.copy()
    
    history = {'energy': [], 'A': [], 'tau': [], 'T': []}
    
    curr_fit_y = model_function(data_x, **params)
    curr_energy = calculate_energy(data_y, curr_fit_y)
    
    for i in range(iterations):
        beta = beta_schedule[i]
        
        # Propose new parameters
        for key in params:
            if vary_flags[key]:
                change = params[key] * increments[key] * random.uniform(-1, 1)
                temp_params[key] = max(0.001, params[key] + change)
            else:
                temp_params[key] = params[key]

        # Evaluate new energy
        trial_y = model_function(data_x, **temp_params)
        new_energy = calculate_energy(data_y, trial_y)
        delta_e = new_energy - curr_energy
        
        # Metropolis Criterion
        if delta_e < 0 or random.random() < np.exp(-beta * delta_e):
            params = temp_params.copy()
            curr_energy = new_energy
            
        # Save History
        history['energy'].append(curr_energy)
        history['A'].append(params['A'])
        history['tau'].append(params['tau'])
        history['T'].append(params['T'])
        
    return params, curr_energy, history

# ==========================================
# Report Experiments
# ==========================================

def load_or_generate_data():
    script_dir = os.path.dirname(os.path.abspath(__file__))
    data_path = os.path.join(script_dir, "data.txt")
    try:
        data_x, data_y = np.loadtxt(data_path, unpack=True)
        print(f"Loaded {len(data_x)} points from data.txt")
    except Exception:
        print("data.txt not found. Generating dummy data.")
        data_x = np.linspace(0, 50, 500)
        true_y = model_function(data_x, 1.5, 15.0, 5.0)
        noise = np.random.normal(0, 0.05, len(data_x))
        data_y = true_y + noise
    return data_x, data_y

def task_standard_fit_and_save(data_x, data_y):
    print("\n--- Running Standard Fit (Baseline) ---")
    iterations = 10000
    beta_schedule = np.full(iterations, 100.0)
    
    init_params = {'A': 0.5, 'tau': 10.0, 'T': 10.0}
    increments = {'A': 0.05, 'tau': 0.05, 'T': 0.05}
    vary_flags = {'A': True, 'tau': True, 'T': True}
    
    best_params, best_energy, _ = run_mc_fit(
        data_x, data_y, init_params, increments, vary_flags, beta_schedule, iterations
    )
    print(f"Found Optimal Params: A={best_params['A']:.3f}, tau={best_params['tau']:.3f}, T={best_params['T']:.3f}")
    return best_params

def task_role_of_beta(data_x, data_y):
    print("\n--- Running Role of Beta (6 Betas, 3 Trials each) ---")
    iterations = 5000
    betas = [1.0, 5.0, 10.0, 20.0, 50.0, 200.0]
    
    init_params = {'A': 0.5, 'tau': 10.0, 'T': 10.0}
    increments = {'A': 0.05, 'tau': 0.05, 'T': 0.05}
    vary_flags = {'A': True, 'tau': True, 'T': True}
    
    plt.figure(figsize=(10, 6))
    
    overall_best_beta = 200.0
    overall_best_energy = float('inf')
    
    for b in betas:
        best_energy_for_b = float('inf')
        best_history = None
        for trial in range(3):
            beta_schedule = np.full(iterations, b)
            _, final_e, history = run_mc_fit(
                data_x, data_y, init_params, increments, vary_flags, beta_schedule, iterations
            )
            if final_e < best_energy_for_b:
                best_energy_for_b = final_e
                best_history = history
                
        if best_energy_for_b < overall_best_energy:
            overall_best_energy = best_energy_for_b
            overall_best_beta = b
                
        plt.plot(best_history['energy'], label=f'Beta = {b:<5.1f} (Best E={best_energy_for_b:.1f})')
        
    plt.title("Role of Inverse Temperature (Beta) on Convergence")
    plt.xlabel("Iteration Step")
    plt.ylabel("Energy E")
    plt.yscale('log')
    plt.legend()
    plt.grid(True, alpha=0.3)
    plt.show()
    
    print(f"Selected Best Beta for further tests: {overall_best_beta}")
    return overall_best_beta

def task_1d_landscape(data_x, data_y, best_beta, optimal_params):
    print("\n--- Running 1D Landscape (Varying A only) ---")
    iterations = 600  # 60% of original 1000
    betas_to_test = [1.0, 20.0, best_beta]

    # Start to the left of the minimum so the walker has to cross it
    init_params = {'A': optimal_params['A'] - 0.4, 'tau': optimal_params['tau'], 'T': optimal_params['T']}

    # Large increment so jumps and backtracking are clearly visible
    increments = {'A': 0.5, 'tau': 0.0, 'T': 0.0}
    vary_flags = {'A': True, 'tau': False, 'T': False}
    
    histories = {}
    for b in betas_to_test:
        beta_schedule = np.full(iterations, b)
        _, _, history = run_mc_fit(
            data_x, data_y, init_params, increments, vary_flags, beta_schedule, iterations
        )
        histories[b] = history
    
    # Generate True Landscape
    a_values = np.linspace(max(0.1, optimal_params['A'] - 0.8), optimal_params['A'] + 0.8, 300)
    energies = []
    for a in a_values:
        test_y = model_function(data_x, a, optimal_params['tau'], optimal_params['T'])
        energies.append(calculate_energy(data_y, test_y))
        
    fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 7))
    
    # Subplot 1: Full Range
    ax1.plot(a_values, energies, color='black', linewidth=2, label='Energy Landscape')
    for b in betas_to_test:
        ax1.plot(histories[b]['A'], histories[b]['energy'], linewidth=1, alpha=0.6, label=f'Beta={b}')
    ax1.set_title("1D Landscape (Full View)")
    ax1.set_xlabel("Parameter A")
    ax1.set_ylabel("Energy E")
    ax1.legend()
    ax1.grid(True, alpha=0.3)
    
    # Subplot 2: Zoomed — each beta keeps its own color for both line and dots
    ax2.plot(a_values, energies, color='black', linewidth=2, alpha=0.3, label='Landscape')

    line_colors = ['tab:blue', 'tab:orange', 'tab:green']
    all_zoomed_A = []
    all_zoomed_E = []

    for idx, b in enumerate(betas_to_test):
        col = line_colors[idx]
        # Connecting line in the same color as the dots
        ax2.plot(histories[b]['A'], histories[b]['energy'], color=col, linewidth=0.8, alpha=0.6)
        # Scatter dots in matching color
        ax2.scatter(histories[b]['A'], histories[b]['energy'],
                    color=col, s=15, alpha=0.8,
                    edgecolors='none', label=f'Path (Beta={b})')

        all_zoomed_A.extend(histories[b]['A'])
        all_zoomed_E.extend(histories[b]['energy'])

    # Tight horizontal zoom around visited A values
    a_margin = (max(all_zoomed_A) - min(all_zoomed_A)) * 0.05
    ax2.set_xlim(min(all_zoomed_A) - a_margin, max(all_zoomed_A) + a_margin)
    # Very tight vertical margin to stretch the landscape shape
    e_min, e_max = min(all_zoomed_E), max(all_zoomed_E)
    e_margin = (e_max - e_min) * 0.02
    ax2.set_ylim(e_min - e_margin, e_max + e_margin)

    ax2.set_title("1D Landscape (Zoomed — Stretched Vertical)")
    ax2.set_xlabel("Parameter A")
    ax2.set_ylabel("Energy E")
    ax2.legend()
    plt.tight_layout()
    plt.show()

def task_2d_landscape(data_x, data_y, best_beta, optimal_params):
    print("\n--- Running 2D Landscape (Varying A and tau) ---")
    iterations = 3000
    beta_schedule = np.full(iterations, best_beta)
    
    init_params = {
        'A': optimal_params['A'] * 0.2,    
        'tau': optimal_params['tau'] * 2.0, 
        'T': optimal_params['T']
    } 
    increments = {'A': 0.05, 'tau': 0.1, 'T': 0.0}
    vary_flags = {'A': True, 'tau': True, 'T': False}
    
    _, _, history = run_mc_fit(
        data_x, data_y, init_params, increments, vary_flags, beta_schedule, iterations
    )
    
    # Dynamic bounds around the visited region
    min_A_visited = min(history['A'])
    max_A_visited = max(history['A'])
    min_tau_visited = min(history['tau'])
    max_tau_visited = max(history['tau'])
    
    a_vals = np.linspace(max(0.01, min_A_visited * 0.8), max(optimal_params['A'], max_A_visited) * 1.1, 80)
    tau_vals = np.linspace(max(0.01, min_tau_visited * 0.8), max(optimal_params['tau'], max_tau_visited) * 1.1, 80)
    
    A_grid, Tau_grid = np.meshgrid(a_vals, tau_vals)
    E_grid = np.zeros_like(A_grid)
    
    for i in range(A_grid.shape[0]):
        for j in range(A_grid.shape[1]):
            test_y = model_function(data_x, A_grid[i,j], Tau_grid[i,j], optimal_params['T'])
            E_grid[i,j] = calculate_energy(data_y, test_y)
            
    plt.figure(figsize=(10, 8))
    
    cp = plt.contourf(A_grid, Tau_grid, E_grid, levels=60, cmap='coolwarm')
    plt.colorbar(cp, label="Energy E")
    
    plt.plot(history['A'], history['tau'], color='black', alpha=0.4, linewidth=0.8)
    plt.scatter(history['A'], history['tau'], c=range(iterations), cmap='viridis', 
                s=8, zorder=5, label='MC Trajectory')
    
    plt.scatter(history['A'][0], history['tau'][0], color='red', marker='X', s=150, label='Start', zorder=10)
    plt.scatter(optimal_params['A'], optimal_params['tau'], color='lime', marker='*', s=250,
                label='True Minimum', edgecolors='black', zorder=10)

    plt.title(f"2D Energy Landscape Contour and Trajectory\n(Fixed T={optimal_params['T']:.2f}, Beta={best_beta})")
    plt.xlabel("Parameter A")
    plt.ylabel("Parameter tau")
    plt.legend()
    plt.show()

def task_dynamic_beta(data_x, data_y, best_beta, optimal_params):
    print("\n--- Running Dynamic Beta: Step Jump Schedules ---")
    iterations = 5000

    init_params = {
        'A': optimal_params['A'] * 0.008,
        'tau': optimal_params['tau'] * 0.008,
        'T': optimal_params['T'] * 0.008
    }
    increments = {'A': 0.2, 'tau': 0.2, 'T': 0.2}
    vary_flags = {'A': True, 'tau': True, 'T': True}

    beta_cap     = 200.0
    target_beta  = 10.0
    target_iters = [1, 2, 5, 10, 50, 200, 500]
    n_trials     = 5

    i_steps = np.arange(iterations)

    def make_schedule(reach_at):
        return np.where(i_steps < reach_at, target_beta, beta_cap)

    # --- Constant beta reference (5 trials, best kept) ---
    const_beta = 200.0
    schedule_const = np.full(iterations, const_beta)
    best_const_energy = float('inf')
    best_const_history = None
    for trial in range(n_trials):
        _, final_e, history = run_mc_fit(
            data_x, data_y, init_params, increments, vary_flags, schedule_const, iterations
        )
        if final_e < best_const_energy:
            best_const_energy = final_e
            best_const_history = history
    print(f"  constant beta={const_beta} | best final_E={best_const_energy:.2f}")

    colors = plt.cm.plasma(np.linspace(0.1, 0.9, len(target_iters)))

    # --- Plot beta schedules ---
    fig_sched, ax_sched = plt.subplots(figsize=(10, 4))
    for i, reach_at in enumerate(target_iters):
        schedule = make_schedule(reach_at)
        ax_sched.plot(schedule, color=colors[i], linewidth=1.5,
                      label=f'jump at iter {reach_at}')
    ax_sched.axhline(target_beta, color='gray', linestyle=':', linewidth=1, label=f'β={target_beta}')
    ax_sched.axhline(beta_cap,    color='gray', linestyle='--', linewidth=1, label=f'β_cap={beta_cap}')
    ax_sched.set_title(f"Beta Schedules — β={target_beta} for first N iters, then jumps to β_cap={beta_cap}\n"
                       "target_iters = number of iterations before jump")
    ax_sched.set_xlabel("Iteration")
    ax_sched.set_ylabel("Beta")
    ax_sched.legend(fontsize=8)
    ax_sched.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()

    # --- Run all trials, store all energies and histories ---
    all_final_energies = np.zeros((len(target_iters), n_trials))
    best_histories = []
    best_energies = []

    for i, reach_at in enumerate(target_iters):
        schedule = make_schedule(reach_at)
        best_energy = float('inf')
        best_history = None
        for trial in range(n_trials):
            _, final_e, history = run_mc_fit(
                data_x, data_y, init_params, increments, vary_flags, schedule, iterations
            )
            all_final_energies[i, trial] = final_e
            if final_e < best_energy:
                best_energy = final_e
                best_history = history
        best_histories.append(best_history)
        best_energies.append(best_energy)
        print(f"  jump_at={reach_at:>4d} | "
              f"best_E={best_energy:.2f} | mean_E={all_final_energies[i].mean():.2f} | "
              f"trials={list(np.round(all_final_energies[i], 1))}")

    # --- Energy convergence plot (best of 5) ---
    plt.figure(figsize=(12, 6))
    plt.plot(best_const_history['energy'], color='black', linewidth=2.0, linestyle='--',
             alpha=0.9, label=f'Constant β={const_beta} (best of {n_trials})', zorder=5)

    for i, reach_at in enumerate(target_iters):
        label = f'jump at iter {reach_at} (best E={best_energies[i]:.1f})'
        plt.plot(best_histories[i]['energy'], alpha=0.85, color=colors[i], linewidth=1.5, label=label)

    plt.title(
        f"Simulated Annealing: β={target_beta} for N iters, then β={beta_cap}\n"
        f"Jump at different iterations | dashed = constant β={const_beta} | best of {n_trials} trials"
    )
    plt.xlabel("Iteration Step")
    plt.ylabel("Energy E")
    plt.yscale('log')
    plt.legend(fontsize=9)
    plt.grid(True, alpha=0.3)
    plt.tight_layout()
    plt.show()
    
if __name__ == "__main__":
    x, y = load_or_generate_data()
    
    optimal_params = task_standard_fit_and_save(x, y)
    best_beta = task_role_of_beta(x, y)
    
    task_1d_landscape(x, y, best_beta, optimal_params)
    task_2d_landscape(x, y, best_beta, optimal_params)
    task_dynamic_beta(x, y, best_beta, optimal_params)