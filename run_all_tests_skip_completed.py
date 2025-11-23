import os
import subprocess
import argparse
import time
import re
import math
from datetime import datetime
import uuid
import pandas as pd


# Helper: Extract dataset size from filename
def extract_dataset_size(filename: str) -> int:
    """
    Extracts numeric size from filenames such as:
    - convex_200.pnt
    - Ci_10K_D.pnt
    - EM_46625_F.pnt
    - uniform_5k_noise.txt

    Returns -1 if no numeric inference is possible.
    """

    # Detect 10K, 5k, 100K style names
    match_k = re.search(r"(\d+)\s*([kK])", filename)
    if match_k:
        return int(match_k.group(1)) * 1000

    # Otherwise detect final standalone integer (covers 46625, 500, etc.)
    match_num = re.findall(r"(\d+)", filename)
    if match_num:
        return int(match_num[-1])

    return -1


# CLI arguments
parser = argparse.ArgumentParser(description="Parallel-safe benchmarking runner.")
parser.add_argument("--exec", required=True, help="Path to compiled executable")
parser.add_argument("--datasets", default="final_test_datasets", help="Folder containing datasets")
parser.add_argument("--runs", "-r", type=int, default=1, help="Repeats per dataset")
parser.add_argument("--tag", default="", help="Optional label suffix for output file")
parser.add_argument("--output_dir", default="results/raw", help="Where to store generated CSV files")

args = parser.parse_args()


# Resolve environment paths
ROOT = os.getcwd()
EXEC = os.path.abspath(os.path.join(ROOT, args.exec))
DATASET_DIR = os.path.abspath(os.path.join(ROOT, args.datasets))
OUTPUT_DIR = os.path.abspath(os.path.join(ROOT, args.output_dir))

RUNS = args.runs
ALGO_NAME = os.path.basename(EXEC)

timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
unique_id = uuid.uuid4().hex[:6]
TAG = f"_{args.tag}" if args.tag else ""

OUTPUT_PATH = os.path.join(OUTPUT_DIR, f"{ALGO_NAME}{TAG}_{timestamp}_{unique_id}.csv")
os.makedirs(OUTPUT_DIR, exist_ok=True)


# Detect already completed datasets for this algorithm
completed = set()

for f in os.listdir(OUTPUT_DIR):
    if f.endswith(".csv"):
        try:
            df = pd.read_csv(os.path.join(OUTPUT_DIR, f), usecols=["dataset", "algorithm"])
            for _, row in df.iterrows():
                completed.add((row["dataset"], row["algorithm"]))
        except Exception:
            pass


# Collect dataset files (.txt and .pnt)
datasets = sorted([
    os.path.join(root, f)
    for root, _, files in os.walk(DATASET_DIR)
    for f in files if f.lower().strip().endswith((".txt", ".pnt"))
], key=lambda x: os.path.basename(x).lower())

if not datasets:
    print(f"No datasets found in {DATASET_DIR}")
    exit(1)


# Create CSV header
with open(OUTPUT_PATH, "w") as f:
    f.write("dataset,size,algorithm,mean,stddev,min,max,runtime_ms,successful_runs,requested_runs\n")


# Main Execution Loop
print("\nBenchmarking Started")
print(f"Algorithm: {ALGO_NAME}")
print(f"Runs per dataset: {RUNS}")
print(f"Output file: {OUTPUT_PATH}\n")


for dataset in datasets:
    name = os.path.basename(dataset)

    # Skip logic
    if (name, ALGO_NAME) in completed:
        print(f"✔ SKIPPED (already completed): {name}")
        continue

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
            print(f" Run {i+1}/{RUNS}: No RESULT line found")
            continue

        try:
            weight = float(parsed.split(",")[0])
            weights.append(weight)
        except ValueError:
            print(f" Run {i+1}/{RUNS}: Failed to parse RESULT: '{parsed}'")

    # Result aggregation and CSV write
    size = extract_dataset_size(name)

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

        avg_runtime = total_runtime / len(weights)
        row = f"{name},{size},{ALGO_NAME},{mean_val},{stddev_val},{mn},{mx},{round(avg_runtime,3)},{len(weights)},{RUNS}\n"

    with open(OUTPUT_PATH, "a") as f:
        f.write(row)


# Done
print("\nBenchmarking Complete.")
print(f"Results saved to: {OUTPUT_PATH}\n")
