import os
import matplotlib.pyplot as plt

# Directory containing datasets
DATASET_DIR = "final_test_datasets"

# Reads points from either .txt or .pnt format
def load_points(path):
    points = []
    with open(path) as f:
        first_line = True
        for line in f:
            line = line.strip()
            if not line or line.startswith("#"):
                continue

            # Skip first line if it's just a count
            if first_line and line.replace("-", "").isdigit():
                first_line = False
                continue

            # Support both comma or space separated coordinates
            parts = line.replace(",", " ").split()
            if len(parts) >= 2:
                try:
                    x = float(parts[0])
                    y = float(parts[1])
                    points.append((x, y))
                except ValueError:
                    pass

    return points


print("\nGenerating scatter plots of dataset points...\n")

# Walk all subdirectories
for root, _, files in os.walk(DATASET_DIR):
    for file in files:
        if not file.lower().endswith((".txt", ".pnt")):
            continue

        dataset_path = os.path.join(root, file)
        print(f"Processing: {file}")

        points = load_points(dataset_path)

        if not points:
            print("  Skipped (no valid points found)")
            continue

        # Extract coordinates
        xs = [p[0] for p in points]
        ys = [p[1] for p in points]

        # Create plot
        plt.figure(figsize=(6, 6))
        plt.scatter(xs, ys, s=10, color="black")
        plt.gca().set_aspect("equal", adjustable="box")
        plt.title(file)
        plt.grid(True, linestyle="--", alpha=0.3)

        # Output file path
        out_name = os.path.splitext(file)[0] + ".png"
        out_path = os.path.join(root, out_name)

        # Save image next to dataset
        plt.savefig(out_path, dpi=300, bbox_inches="tight")
        plt.close()

print("\nDone.\n")
