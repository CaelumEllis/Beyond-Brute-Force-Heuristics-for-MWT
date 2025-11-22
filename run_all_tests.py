import os
import subprocess
import argparse
import time
import re
import math

# CLI Arguments
parser = argparse.ArgumentParser(description="Benchmark triangulation algorithm.")
parser.add_argument("--exec", required=True, help="Path to compiled executable")
parser.add_argument("--datasets", default="final_test_datasets", help="Dataset folder")
parser.add_argument("--runs", "-r", type=int, default=1, help="How many repeats per dataset")
parser.add_argument("--output_dir", default="results/raw", help="Folder for CSV output")
args = parser.parse_args()

# Resolve Paths
ROOT = os.getcwd()
EXEC = os.path.abspath(os.path.join(ROOT, args.exec))
DATASET_DIR = os.path.abspath(os.path.join(ROOT, args.datasets))
OUTPUT_DIR = os.path.abspath(os.path.join(ROOT, args.output_dir))

ALGO_NAME = os.path.basename(EXEC)
RUNS = args.runs

OUTPUT_PATH = os.path.join(OUTPUT_DIR, f"{ALGO_NAME}.csv")

os.makedirs(OUTPUT_DIR, exist_ok=True)

# CSV header only if new
if not os.path.exists(OUTPUT_PATH):
    with open(OUTPUT_PATH, "w") as f:
        f.write("dataset,size,algorithm,mean,stddev,min,max,runtime_ms,runs\n")

# Collect dataset files
datasets = sorted([
    os.path.join(root, f)
    for root, _, files in os.walk(DATASET_DIR)
    for f in files if f.endswith(".txt")
])

if not datasets:
    print(f"\n No datasets found in: {DATASET_DIR}")
    exit(1)

print(f"\n=== Running algorithm: {ALGO_NAME} ===")
print(f"📂 Datasets: {DATASET_DIR}")
print(f"🔁 Runs per dataset: {RUNS}")
print(f"📄 Output file: {OUTPUT_PATH}\n")

# Main Loop
for dataset in datasets:
    name = os.path.basename(dataset)
    print(f"→ {name}")

    weights = []
    total_runtime = 0.0

    for i in range(RUNS):
        start = time.time()
        result = subprocess.run([EXEC, dataset], capture_output=True, text=True)
        trial_time = round((time.time() - start) * 1000, 3)
        total_runtime += trial_time

        # Parse output
        parsed = None
        for line in result.stdout.splitlines():
            if line.startswith("RESULT,"):
                parsed = line.replace("RESULT,", "").strip()

        if parsed is None:
            print(f" Run {i+1}/{RUNS} failed (no RESULT)")
            continue

        try:
            weight = float(parsed.split(",")[0])
            weights.append(weight)
        except:
            print(f" Run {i+1}/{RUNS} parse error: '{parsed}'")
            continue

    # Skip if zero successful runs
    if not weights:
        row = f"{name},-,{ALGO_NAME},NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,{RUNS}\n"
        with open(OUTPUT_PATH, "a") as f:
            f.write(row)
        continue

    # Compute statistics
    mn = min(weights)
    mx = max(weights)
    mean = sum(weights) / len(weights)

    if len(weights) > 1:
        variance = sum((w - mean) ** 2 for w in weights) / len(weights)
        stddev = math.sqrt(variance)
    else:
        stddev = 0.0

    # Detect size
    size_match = re.search(r"_(\d+)\.txt$", name)
    size = int(size_match.group(1)) if size_match else -1

    # Write row
    avg_runtime = total_runtime / len(weights) if weights else 0
    row = f"{name},{size},{ALGO_NAME},{mean},{stddev},{mn},{mx},{round(avg_runtime,3)},{len(weights)}\n"

    with open(OUTPUT_PATH, "a") as f:
        f.write(row)

print("\nBenchmark Complete")
print(f"Results stored in: {OUTPUT_PATH}\n")
