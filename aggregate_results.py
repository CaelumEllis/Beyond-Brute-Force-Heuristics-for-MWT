import os
import pandas as pd

RAW_DIR = "results/raw"
OUTPUT_DIR = "results/summary"
OUTPUT_FILE = os.path.join(OUTPUT_DIR, "matrix_summary.txt")

os.makedirs(OUTPUT_DIR, exist_ok=True)

# Load CSVs
csv_files = [f for f in os.listdir(RAW_DIR) if f.endswith(".csv")]
if not csv_files:
    print(" No CSV files found.")
    exit()

dfs = []
for file in csv_files:
    algo = file.replace(".csv", "")
    df = pd.read_csv(os.path.join(RAW_DIR, file))

    # Ensure mean values are numeric (convert errors → NaN)
    df["mean"] = pd.to_numeric(df["mean"], errors="coerce")

    if df.empty:
        continue

    df["algorithm"] = algo
    dfs.append(df)

combined = pd.concat(dfs, ignore_index=True)
combined = combined[["dataset", "size", "algorithm", "mean"]]

# Identify brute force baseline
brute = combined[combined["algorithm"].str.contains("brute", case=False)]
baseline = {}

if brute.empty:
    print("⚠ No brute force results found — skipping percent comparison.")
else:
    baseline = brute.set_index("dataset")["mean"].to_dict()

def calc_diff(row):
    # handles missing brute force
    if not baseline:
        return None
    base = baseline.get(row["dataset"])
    if base is None or pd.isna(row["mean"]):
        return None
    return ((row["mean"] - base) / base) * 100

combined["percent_diff"] = combined.apply(calc_diff, axis=1)

# Pivot to matrix form
pivot = combined.pivot(index="dataset", columns="algorithm", values="mean")
cols_sorted = sorted(pivot.columns, key=lambda x: "0" if "brute" in x.lower() else x)
pivot = pivot[cols_sorted]

formatted = pivot.astype(object)

# Format output values
for dataset in pivot.index:
    for algo in pivot.columns:
        val = pivot.loc[dataset, algo]
        if pd.isna(val):
            formatted.loc[dataset, algo] = "NO OUTPUT"
            continue

        if "brute" in algo.lower():
            formatted.loc[dataset, algo] = f"{val:.4f}"
        else:
            pct = calc_diff(pd.Series({"dataset": dataset, "mean": val}))
            if pct is None:
                formatted.loc[dataset, algo] = f"{val:.4f} (N/A)"
            else:
                formatted.loc[dataset, algo] = f"{val:.4f} ({pct:+.2f}%)"

formatted = formatted.reset_index().rename(columns={"dataset": "Dataset"})

# Determine dataset category (convex, fractal, gaussian...)
combined["category"] = combined["dataset"].apply(lambda x: x.split("_")[0])

output_rows = []

for category in sorted(combined["category"].unique()):
    group_rows = formatted[formatted["Dataset"].str.startswith(category)].copy()
    output_rows.append(group_rows)

    # compute category avg
    avg_row = {"Dataset": f"Avg - {category}"}
    for algo in pivot.columns:
        if "brute" in algo.lower():
            avg_row[algo] = "---"
            continue

        vals = combined[(combined["category"] == category) & (combined["algorithm"] == algo)]["percent_diff"].dropna()
        avg_row[algo] = f"{vals.mean():+.2f}%" if (not vals.empty and baseline) else "N/A"

    output_rows.append(pd.DataFrame([avg_row]))

# Global summary
global_row = {"Dataset": "GLOBAL AVG"}
for algo in pivot.columns:
    if "brute" in algo.lower():
        global_row[algo] = "---"
    else:
        vals = combined[combined["algorithm"] == algo]["percent_diff"].dropna()
        global_row[algo] = f"{vals.mean():+.2f}%" if (not vals.empty and baseline) else "N/A"

output_rows.append(pd.DataFrame([global_row]))
final_table = pd.concat(output_rows, ignore_index=True)

# alignment formatting
col_widths = {col: max(len(str(x)) for x in final_table[col].tolist() + [col]) for col in final_table.columns}

def format_row(row):
    return " | ".join(
        str(row[col]).rjust(col_widths[col]) if col != "Dataset" else str(row[col]).ljust(col_widths[col])
        for col in final_table.columns
    )

sep = "-" * (sum(col_widths.values()) + 3 * (len(col_widths) - 1))

with open(OUTPUT_FILE, "w") as f:
    # header
    header = {col: col for col in final_table.columns}
    f.write(format_row(header) + "\n")
    f.write(sep + "\n")

    for _, row in final_table.iterrows():
        if row["Dataset"].startswith("Avg") or row["Dataset"] == "GLOBAL AVG":
            f.write(sep + "\n")
        f.write(format_row(row) + "\n")

print("\nSummary file created.")
print(f"Saved at: {OUTPUT_FILE}")
