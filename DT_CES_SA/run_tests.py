import os
import subprocess
import argparse
import time
from datetime import datetime

# Command-line args
parser = argparse.ArgumentParser(description="Automate full dataset benchmarking.")
parser.add_argument("-r", "--runs", type=int, default=5, help="Runs per dataset")
parser.add_argument("-o", "--output", default="benchmark_results.csv", help="Output CSV filename")
parser.add_argument("-e", "--exec", default="./DT_CES_SA", help="Path to compiled executable")
parser.add_argument("-d", "--datasets", default="datasets", help="Dataset root folder")
args = parser.parse_args()

RUNS = args.runs
OUTPUT_FILE = args.output
EXEC = args.exec
DATASET_ROOT = args.datasets

# CSV Header
header = (
    "category,file,points,edges,dt_ms,candidate_ms,sa_ms,"
    "initial_weight,final_weight,improvement_percent,accepted_flips\n"
)

# Create file if missing
if not os.path.exists(OUTPUT_FILE):
    print(f"Creating new result file: {OUTPUT_FILE}")
    with open(OUTPUT_FILE, "w") as f:
        f.write(header)

# Collect datasets
dataset_entries = []

for root, dirs, files in os.walk(DATASET_ROOT):
    for f in files:
        if f.endswith(".txt"):
            full = os.path.join(root, f)
            cat = os.path.basename(os.path.dirname(full))
            dataset_entries.append((cat, full))

if not dataset_entries:
    print("No datasets found.")
    exit(1)

print("\nDataset folders detected:")
for cat, _ in sorted(dataset_entries):
    print(f"  - {cat}")

print(f"\nEach dataset will run {RUNS} times")
print(f"Output will be stored in: {OUTPUT_FILE}\n")

# Execution loop
total = len(dataset_entries) * RUNS
count = 0

for category, dataset in sorted(dataset_entries):
    for run in range(RUNS):
        count += 1
        print(f"[{count}/{total}] Running {dataset} ({category}) — run {run+1}/{RUNS}")

        start = time.time()
        result = subprocess.run(
            [EXEC, dataset, "1"],
            capture_output=True,
            text=True,
            timeout=120
        )
        elapsed = round(time.time() - start, 2)

        result_line = None
        for line in result.stdout.splitlines():
            if line.startswith("RESULT,"):
                result_line = line[len("RESULT,"):]
                break

        if result_line is None:
            print("No RESULT line found. Output was:")
            print(result.stdout)
            continue

        with open(OUTPUT_FILE, "a") as f:
            f.write(f"{category},{result_line}\n")

        print(f"Completed in {elapsed}s")

print("\nBenchmarking complete.")
print(f"Results saved to: {OUTPUT_FILE}")
print(f"Finished at: {datetime.now().strftime('%Y-%m-%d %H:%M:%S')}")
