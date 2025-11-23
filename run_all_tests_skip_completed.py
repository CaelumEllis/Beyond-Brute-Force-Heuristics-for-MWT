import os
import subprocess
import argparse
import time
import re
import math
from datetime import datetime
import uuid


# extract dataset size from filename
def extract_dataset_size(filename: str) -> int:
    match_k = re.search(r"(\d+)\s*([kK])", filename)
    if match_k:
        return int(match_k.group(1)) * 1000
    match_num = re.findall(r"(\d+)", filename)
    if match_num:
        return int(match_num[-1])
    return -1


parser = argparse.ArgumentParser(description="Parallel-safe benchmarking runner.")
parser.add_argument("--exec", required=True, help="Path to compiled executable")
parser.add_argument("--datasets", default="final_test_datasets", help="Folder containing datasets")
parser.add_argument("--runs", "-r", type=int, default=1, help="Repeats per dataset")
parser.add_argument("--tag", default="", help="Optional label suffix for output file")
parser.add_argument("--output_dir", default="results/raw", help="Where to store generated CSV files")

args = parser.parse_args()


# resolve paths
ROOT = os.getcwd()
EXEC = os.path.abspath(os.path.join(ROOT, args.exec))
DATASET_DIR = os.path.abspath(os.path.join(ROOT, args.datasets))
OUTPUT_DIR = os.path.abspath(os.path.join(ROOT, args.output_dir))

REQUESTED_RUNS = args.runs
ALGO_NAME = os.path.basename(EXEC)

timestamp = datetime.now().strftime("%Y%m%d_%H%M%S")
unique_id = uuid.uuid4().hex[:6]
TAG = f"_{args.tag}" if args.tag else ""

OUTPUT_PATH = os.path.join(OUTPUT_DIR, f"{ALGO_NAME}{TAG}_{timestamp}_{unique_id}.csv")
os.makedirs(OUTPUT_DIR, exist_ok=True)


# track completed run counts
completed = {}  # (dataset, algo) -> total_successful_runs

history_source = {}  # (dataset, algo) -> file where it was logged


for file in os.listdir(OUTPUT_DIR):
    if file.endswith(".csv"):
        fpath = os.path.join(OUTPUT_DIR, file)
        try:
            with open(fpath, "r") as f:
                next(f)
                for line in f:
                    parts = line.strip().split(",")
                    if len(parts) < 10:
                        continue

                    dataset_name = parts[0]
                    algo_name = parts[2]
                    mean_val = parts[3]
                    success_runs = parts[8]

                    if success_runs.isdigit():
                        success_runs = int(success_runs)
                    else:
                        success_runs = 0

                    if mean_val != "NO_OUTPUT" and success_runs > 0:
                        key = (dataset_name, algo_name)
                        completed[key] = completed.get(key, 0) + success_runs
                        history_source[key] = file

        except Exception:
            pass


# collect dataset files
datasets = sorted([
    os.path.join(root, f)
    for root, _, files in os.walk(DATASET_DIR)
    for f in files if f.lower().strip().endswith((".txt", ".pnt"))
], key=lambda x: os.path.basename(x).lower())


if not datasets:
    print(f"No datasets found in {DATASET_DIR}")
    exit(1)


# write header for new run file
with open(OUTPUT_PATH, "w") as f:
    f.write("dataset,size,algorithm,mean,stddev,min,max,runtime_ms,successful_runs,requested_runs\n")


print("\nBenchmarking Started")
print(f"Algorithm: {ALGO_NAME}")
print(f"Runs requested per dataset: {REQUESTED_RUNS}")
print(f"Output file: {OUTPUT_PATH}\n")


for dataset in datasets:
    name = os.path.basename(dataset)

    already_done = completed.get((name, ALGO_NAME), 0)

    if already_done >= REQUESTED_RUNS:
        print(f"Skipping {name} (already completed {already_done}/{REQUESTED_RUNS} in: {history_source.get((name, ALGO_NAME),'unknown')})")
        continue

    remaining_runs = REQUESTED_RUNS - already_done
    print(f"→ {name} ({already_done}/{REQUESTED_RUNS} done, running {remaining_runs} more)")

    weights = []
    total_runtime = 0.0

    for i in range(remaining_runs):
        start = time.time()
        result = subprocess.run([EXEC, dataset], capture_output=True, text=True)
        trial_time = round((time.time() - start) * 1000, 3)
        total_runtime += trial_time

        parsed = None
        for line in result.stdout.splitlines():
            if line.startswith("RESULT,"):
                parsed = line.replace("RESULT,", "").strip()

        if parsed is None:
            print(f" Run {i+1}/{remaining_runs}: No RESULT line found")
            continue

        try:
            weight = float(parsed.split(",")[0])
            weights.append(weight)
        except ValueError:
            print(f" Run {i+1}/{remaining_runs}: Failed to parse RESULT: '{parsed}'")

    size = extract_dataset_size(name)

    if not weights:
        row = f"{name},{size},{ALGO_NAME},NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,NO_OUTPUT,0,{remaining_runs}\n"
    else:
        mean_val = sum(weights) / len(weights)
        mn = min(weights)
        mx = max(weights)
        if len(weights) > 1:
            variance = sum((w - mean_val) ** 2 for w in weights) / len(weights)
            stddev_val = math.sqrt(variance)
        else:
            stddev_val = 0.0

        avg_runtime = total_runtime / len(weights)
        row = f"{name},{size},{ALGO_NAME},{mean_val},{stddev_val},{mn},{mx},{round(avg_runtime,3)},{len(weights)},{remaining_runs}\n"

    with open(OUTPUT_PATH, "a") as f:
        f.write(row)


print("\nBenchmarking Complete.")
print(f"Results saved to: {OUTPUT_PATH}\n")
