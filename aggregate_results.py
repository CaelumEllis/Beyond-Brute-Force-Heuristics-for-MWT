import os
import pandas as pd
import re

RAW_DIR = "results/raw"
OUTPUT_DIR = "results/summary"
OUTPUT_FILE = os.path.join(OUTPUT_DIR, "matrix_summary.txt")

os.makedirs(OUTPUT_DIR, exist_ok=True)

# extract core algorithm name (strip timestamps)
def normalize_algo_name(name: str) -> str:
    name = os.path.splitext(name)[0]
    match = re.match(r"(.+?)_\d{8}_", name)
    return match.group(1) if match else name

# get all csv result files
csv_files = [f for f in os.listdir(RAW_DIR) if f.endswith(".csv")]
if not csv_files:
    print("No CSV files found.")
    exit()

records = []
for file in csv_files:
    df = pd.read_csv(os.path.join(RAW_DIR, file))
    df["algorithm"] = normalize_algo_name(file)
    df["mean"] = pd.to_numeric(df["mean"], errors="coerce")
    df["successful_runs"] = pd.to_numeric(df.get("successful_runs", 1), errors="coerce").fillna(1).astype(int)
    records.append(df)

combined = pd.concat(records, ignore_index=True)

# compute weighted averages per dataset+algorithm
weighted = (
    combined.groupby(["dataset", "size", "algorithm"])
    .apply(lambda g: (g["mean"] * g["successful_runs"]).sum() / g["successful_runs"].sum()
    if not g["mean"].isna().all() else float("nan"))
    .reset_index(name="weighted_mean")
)

# convert to matrix
pivot = weighted.pivot(index="dataset", columns="algorithm", values="weighted_mean")

# enforce DT_ONLY baseline first
cols = list(pivot.columns)
baseline_col = [c for c in cols if "DT_ONLY" in c.upper()]
others = sorted([c for c in cols if c not in baseline_col])
pivot = pivot[baseline_col + others]

# remove rows with missing values OR baseline == 0 (user request)
pivot = pivot[
    pivot.apply(lambda r: pd.notna(r).all() and float(r[baseline_col[0]]) != 0.0, axis=1)
]

# formatting rules
def format_val(row, col):
    algo_val = row[col]
    base_val = row[baseline_col[0]]

    if pd.isna(algo_val):
        return "NO OUTPUT"

    if float(base_val) == 0.0:
        return f"{algo_val:.4f} (baseline returned 0 - invalid)"

    pct = ((algo_val - base_val) / base_val) * 100
    return f"{algo_val:.4f} ({pct:+.2f}%)"

for col in others:
    pivot[col] = pivot.apply(lambda r: format_val(r, col), axis=1)

pivot[baseline_col[0]] = pivot[baseline_col[0]].apply(lambda v: f"{v:.4f}")

pivot.reset_index(inplace=True)
pivot.rename(columns={"dataset": "Dataset"}, inplace=True)

# compute category tags
pivot["category"] = pivot["Dataset"].apply(lambda x: x.split("_")[0])

output_rows = []
algos = pivot.columns.tolist()[2:]

for cat, group in pivot.groupby("category"):
    block = group.drop(columns=["category"])
    output_rows.append(block)

    if len(block) > 1:
        avg = {"Dataset": f"Avg - {cat}", baseline_col[0]: "---"}
        for col in algos:
            vals = []
            for v in block[col]:
                if "(" in v:
                    try:
                        vals.append(float(v.split("(")[1].split("%")[0]))
                    except:
                        pass
            avg[col] = f"{sum(vals)/len(vals):+.2f}%" if vals else "N/A"
        output_rows.append(pd.DataFrame([avg]))

# compute global average
global_row = {"Dataset": "GLOBAL AVG", baseline_col[0]: "---"}
for col in algos:
    vals = []
    for v in pivot[col]:
        if "(" in v:
            try:
                vals.append(float(v.split("(")[1].split("%")[0]))
            except:
                pass
    global_row[col] = f"{sum(vals)/len(vals):+.2f}%" if vals else "N/A"
output_rows.append(pd.DataFrame([global_row]))

final = pd.concat(output_rows, ignore_index=True)

# format text output
col_widths = {col: max(len(str(x)) for x in final[col].tolist() + [col]) for col in final.columns}

def fmt(row):
    return " | ".join(
        str(row[col]).ljust(col_widths[col]) if col == "Dataset" else str(row[col]).rjust(col_widths[col])
        for col in final.columns
    )

sep = "-" * (sum(col_widths.values()) + 3 * (len(col_widths) - 1))

with open(OUTPUT_FILE, "w") as f:
    header = {col: col for col in final.columns}
    f.write(fmt(header) + "\n")
    f.write(sep + "\n")

    for _, row in final.iterrows():
        if row["Dataset"].startswith("Avg") or row["Dataset"] == "GLOBAL AVG":
            f.write(sep + "\n")
        f.write(fmt(row) + "\n")

print(f"\nSummary file created → {OUTPUT_FILE}")
