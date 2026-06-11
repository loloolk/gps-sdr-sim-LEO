import csv
import sys

def interpolate():
    input_file = 'antenna_models/antenna_model.csv'
    output_file = 'antenna_models/antenna_model_full.csv'
    
    data = {}
    
    # Read existing data
    try:
        with open(input_file, 'r') as f:
            reader = csv.reader(f)
            header = next(reader) # Skip header
            for row in reader:
                if not row: continue
                p, t, g = map(float, row)
                data[(int(p), int(t))] = g
    except FileNotFoundError:
        print(f"Error: {input_file} not found.")
        return

    # Bilinear interpolation
    full_data = []
    for t in range(0, 181):
        for p in range(-180, 181):
            if (p, t) in data:
                gain = data[(p, t)]
            else:
                # Need to interpolate
                # Find surrounding points (p0, t0), (p1, t0), (p0, t1), (p1, t1)
                p0 = p - 1 if p % 2 != 0 else p
                p1 = p + 1 if p % 2 != 0 else p
                t0 = t - 1 if t % 2 != 0 else t
                t1 = t + 1 if t % 2 != 0 else t
                
                # Simple averaging for 2-degree grid
                weights = []
                # Check horizontal interpolation
                if p % 2 != 0 and t % 2 == 0:
                    gain = (data[(p-1, t)] + data[(p+1, t)]) / 2.0
                # Check vertical interpolation
                elif p % 2 == 0 and t % 2 != 0:
                    gain = (data[(p, t-1)] + data[(p, t+1)]) / 2.0
                # Check center interpolation
                else:
                    gain = (data[(p-1, t-1)] + data[(p+1, t-1)] + 
                            data[(p-1, t+1)] + data[(p+1, t+1)]) / 4.0
            
            full_data.append((p, t, gain))

    # Print to stdout as requested and also save to file
    print("Phi[deg],Theta[deg],dB(GainTotal)")
    for p, t, g in full_data:
        print(f"{p},{t},{g:.5f}")
        
    with open(output_file, 'w', newline='') as f:
        writer = csv.writer(f)
        writer.writerow(['Phi[deg]', 'Theta[deg]', 'dB(GainTotal)'])
        for row in full_data:
            writer.writerow([row[0], row[1], f"{row[2]:.5f}"])

if __name__ == "__main__":
    interpolate()
