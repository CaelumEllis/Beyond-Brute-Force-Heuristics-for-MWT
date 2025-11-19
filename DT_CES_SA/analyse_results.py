import pandas as pd
import re

INPUT_FILE = "benchmark_results.csv"
OUTPUT_FILE = "analysis_summary.csv"

print(f"Loading data from: {INPUT_FILE}")
df = pd.read_csv(INPUT_FILE)

# Extract dataset size from filename
df["size"] = df["file"].apply(lambda x: int(re.search(r"_(\d+)\.txt", x).group(1)))

# Ensure numeric fields are typed correctly
numeric_fields = [
    "points", "edges", "dt_ms", "candidate_ms", "sa_ms",
    "initial_weight", "final_weight", "improvement_percent", "accepted_flips"
]

for field in numeric_fields:
    df[field] = pd.to_numeric(df[field], errors="coerce")

# Summary by category and size
size_summary = df.groupby(["category", "size"]).agg(
    runs=("improvement_percent", "count"),
    mean_improvement_pct=("improvement_percent", "mean"),
    std_improvement_pct=("improvement_percent", "std"),
    best_run_pct=("improvement_percent", "max"),
    median_pct=("improvement_percent", "median"),
    avg_sa_time_ms=("sa_ms", "mean")
).reset_index()

size_summary = size_summary.round(3)
size_summary["avg_sa_time_ms"] = size_summary["avg_sa_time_ms"].round(0).astype(int)

# Summary by category
category_summary = df.groupby("category").agg(
    runs=("improvement_percent", "count"),
    mean_improvement_pct=("improvement_percent", "mean"),
    std_improvement_pct=("improvement_percent", "std"),
    best_run_pct=("improvement_percent", "max"),
    median_pct=("improvement_percent", "median"),
    avg_sa_time_ms=("sa_ms", "mean")
).reset_index()

category_summary = category_summary.round(3)
category_summary["avg_sa_time_ms"] = category_summary["avg_sa_time_ms"].round(0).astype(int)

# Global Summary
global_summary = pd.DataFrame([{
    "category": "ALL_DATASETS",
    "runs": len(df),
    "mean_improvement_pct": round(df["improvement_percent"].mean(), 3),
    "std_improvement_pct": round(df["improvement_percent"].std(), 3),
    "best_run_pct": round(df["improvement_percent"].max(), 3),
    "median_pct": round(df["improvement_percent"].median(), 3),
    "avg_sa_time_ms": int(df["sa_ms"].mean())
}])

# Write formatted output
print(f"Writing summary to: {OUTPUT_FILE}")

with open(OUTPUT_FILE, "w") as f:

    f.write("===== GLOBAL PERFORMANCE SUMMARY =====\n")
    global_summary.to_csv(f, index=False)
    f.write("\n\n")

    for category in category_summary["category"]:
        f.write(f"===== CATEGORY SUMMARY: {category} =====\n")
        category_category = category_summary[category_summary["category"] == category]
        category_category.to_csv(f, index=False)
        f.write("\n")

        f.write(f"-- Size breakdown for {category} --\n")
        size_subset = size_summary[size_summary["category"] == category]
        size_subset.to_csv(f, index=False)
        f.write("\n\n")

print("Analysis complete.")
