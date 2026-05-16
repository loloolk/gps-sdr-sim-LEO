import numpy as np
import csv

# --- Constants ---
MU = 3.986004418e14         # Earth's gravitational constant (m^3/s^2)
OMEGA_E = 7.2921151467e-5   # Earth rotation rate (rad/s)
R_EARTH = 6378137.0         # WGS84 Semi-major axis (m)

# --- LEO Orbit Parameters ---
altitude = 500000.0         # 500 km LEO
r = R_EARTH + altitude
inclination = np.radians(51.6) # 51.6 degrees (ISS inclination)

# Orbital angular velocity (rad/s)
omega_orb = np.sqrt(MU / (r**3))

# --- Simulation Parameters ---
duration = 300.0  # Duration of the CSV in seconds
dt = 0.1         # 10 Hz update rate to match your outer loop

output_file = 'input_files/LEO/LEO.csv'

with open(output_file, 'w', newline='') as csvfile:
    writer = csv.writer(csvfile)
    
    num_steps = int(duration / dt) + 1
    for step in range(num_steps):
        t = step * dt
        
        # 1. Calculate position in the orbital plane (Circular Orbit)
        nu = omega_orb * t
        xo = r * np.cos(nu)
        yo = r * np.sin(nu)
        zo = 0.0
        
        # 2. Rotate by inclination (around the X-axis)
        xi = xo
        yi = yo * np.cos(inclination)
        zi = yo * np.sin(inclination)
        
        # 3. Rotate into ECEF frame (Account for Earth's rotation)
        theta = -OMEGA_E * t
        x_ecef = xi * np.cos(theta) - yi * np.sin(theta)
        y_ecef = xi * np.sin(theta) + yi * np.cos(theta)
        z_ecef = zi
        
        # Format: Time, X, Y, Z (Meters, accurate to 1 mm)
        writer.writerow([f"{t:.1f}", f"{x_ecef:.3f}", f"{y_ecef:.3f}", f"{z_ecef:.3f}"])

print(f"Success! Generated '{output_file}' with {num_steps} epochs.")