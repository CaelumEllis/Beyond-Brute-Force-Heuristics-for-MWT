import os
import subprocess
import argparse
import time
import re
import math
from datetime import datetime
import uuid

# CLI Arguments
parser = argparse.ArgumentParser(description="Parallel-safe benchmarking runner.")
parser.add_argument("--exec", required=True, help="Path to compiled executable")
parser.add_argument("--datasets", default="final_test_datasets", help="Folder of .txt datasets")
parser.add_argument("--runs", "-r", type=int, default=1, help="Repeats per dataset")
parser.add_argument("--tag", default="", help="Optional tag to distinguish files")
parser.add_argument("--output_dir", default="results/raw", help="Where to store result CSVs")

args = parser.parse_args()

# Resolve paths
ROOT = os.getcwd()
EXEC = os.path.abspath(os.path.join(ROOT, args.exec))
DATASET_DIR = os.path.abspath(os.path.join(ROOT, args.datasets))
OUTPUT_DIR = os.path.abspath(os.path.join(ROOT, args.output_dir))

RUNS = args.runs
ALGO_NAME = os.path.basename(EXEC)

# Build unique filename
timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
unique_id = uuid.uuid4().hex[:6]
TAG = f"_{args.tag}" if args.tag else ""

OUTPUT_PATH = os.path.join(
    OUTPUT_DIR, f"{ALGO_NAME}{TAG}_{timestamp}_{unique_id}.csv"
)

os.makedirs(OUTPUT_DIR, exist_ok=True)

# Collect dataset files
datasets = sorted([
    os.path.join(root, f)
    for root, _, files in os.walk(DATASET_DIR)
    for f in files if f.endswith(".txt")
])

if not datasets:
    print(f"No datasets found in {DATASET_DIR}")
    exit(1)

# Create CSV with header
with open(OUTPUT_PATH, "w") as f:
    f.write("dataset,size,algorithm,mean,stddev,min,max,runtime_ms,successful_runs,requested_runs\n")

print("\n Parallel Runner Started")
print(f"Algorithm: {ALGO_NAME}")
print(f"Runs per dataset: {RUNS}")
print(f"Output file: {OUTPUT_PATH}\n")

# Main loop
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

        parsed = None
        for line in result.stdout.splitlines():
            if line.startswith("RESULT,"):
                parsed = line.replace("RESULT,", "").strip()

        if parsed is None:
            print(f" Run {i+1}/{RUNS}: No RESULT line")
            continue

        try:
            weight = float(parsed.split(",")[0])
            weights.append(weight)
        except:
            print(f" Run {i+1}/{RUNS}: Parse failed: '{parsed}'")
            continue

    # Dataset Completed — Write Result
    size_match = re.search(r"_(\d+)\.txt$", name)
    size = int(size_match.group(1)) if size_match else -1

    if not weights:
        row = f"{name},{size},{ALGO_NAME},NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,0,{RUNS}\n"
    else:
        mean_val = sum(weights) / len(weights)
        mn = min(weights)
        mx = max(weights)

        if len(weights) > 1:
            variance = sum((w - mean_val)**2 for w in weights) / len(weights)
            stddev_val = math.sqrt(variance)
        else:
            stddev_val = 0.0

        avg_runtime = total_runtime / len(weights) if weights else 0
        row = f"{name},{size},{ALGO_NAME},{mean},{stddev},{mn},{mx},{round(avg_runtime,3)},{len(weights)}\n"

    with open(OUTPUT_PATH, "a") as f:
        f.write(row)

print("\n Finished.")
print(f" Results saved in: {OUTPUT_PATH}\n")
