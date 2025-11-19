import pandas as pd
import matplotlib.pyplot as plt
import os
from io import StringIO

INPUT_FILE = "analysis_summary.csv"
OUTPUT_DIR = "plots"

print(f"Loading: {INPUT_FILE}")

os.makedirs(OUTPUT_DIR, exist_ok=True)

# Read only the relevant table rows
valid_lines = []
capture = False

with open(INPUT_FILE) as f:
    for line in f:
        line = line.strip()

        # Detect when a size block begins
        if line.startswith("-- Size breakdown"):
            capture = True
            continue

        # Stop capturing when a blank line or next header appears
        if capture and (line.startswith("====") or line.startswith("--") or line == ""):
            capture = False

        # Record valid table rows
        if capture and "," in line:
            valid_lines.append(line)

# Build CSV from extracted lines
df = pd.read_csv(StringIO("\n".join(valid_lines)))

# Ensure correct types
df["size"] = pd.to_numeric(df["size"], errors="coerce")
df["mean_improvement_pct"] = pd.to_numeric(df["mean_improvement_pct"], errors="coerce")
df["std_improvement_pct"] = pd.to_numeric(df["std_improvement_pct"], errors="coerce")
df["avg_sa_time_ms"] = pd.to_numeric(df["avg_sa_time_ms"], errors="coerce")

# Sort for nice graph ordering
df = df.sort_values(by=["category", "size"])

print("Parsed data sample:\n", df.head(), "\n")


# Plot 1 – Improvement vs Size
plt.figure(figsize=(10,6))
for category, sub in df.groupby("category"):
    plt.errorbar(
        sub["size"], sub["mean_improvement_pct"],
        yerr=sub["std_improvement_pct"],
        marker="o", capsize=4, label=category
    )

plt.title("Mean Improvement (%) vs Dataset Size")
plt.xlabel("Dataset Size (points)")
plt.ylabel("Mean Improvement (%)")
plt.grid(True, linestyle="--", alpha=0.5)
plt.legend(title="Dataset Type")
plt.tight_layout()
plt.savefig(f"{OUTPUT_DIR}/improvement_vs_size.png", dpi=300)
print("Saved: improvement_vs_size.png")


# Plot 2 — Runtime vs Size
plt.figure(figsize=(10,6))
for category, sub in df.groupby("category"):
    plt.plot(sub["size"], sub["avg_sa_time_ms"], marker="o", label=category)

plt.title("Average Runtime vs Dataset Size")
plt.xlabel("Dataset Size (points)")
plt.ylabel("Runtime (ms)")
plt.grid(True, linestyle="--", alpha=0.5)
plt.legend(title="Dataset Type")
plt.tight_layout()
plt.savefig(f"{OUTPUT_DIR}/runtime_vs_size.png", dpi=300)
print("Saved: runtime_vs_size.png")


# Plot 3 — Runtime vs Improvement (Separate plot per dataset type)
tradeoff_dir = os.path.join(OUTPUT_DIR, "runtime_vs_improvement")
os.makedirs(tradeoff_dir, exist_ok=True)

for category, sub in df.groupby("category"):
    plt.figure(figsize=(8, 6))

    plt.scatter(sub["avg_sa_time_ms"], sub["mean_improvement_pct"], s=60)

    # annotate each point with dataset size
    for _, row in sub.iterrows():
        plt.annotate(
            str(row["size"]),
            (row["avg_sa_time_ms"], row["mean_improvement_pct"]),
            textcoords="offset points",
            xytext=(5, 5),
            fontsize=9
        )

    plt.title(f"Runtime vs Improvement — {category}")
    plt.xlabel("Runtime (ms)")
    plt.ylabel("Improvement (%)")
    plt.grid(True, linestyle="--", alpha=0.5)

    filename = f"{tradeoff_dir}/{category}_tradeoff.png"
    plt.tight_layout()
    plt.savefig(filename, dpi=300)
    print(f"Saved: {filename}")


# Plot 4 — Std Dev vs Size
plt.figure(figsize=(10,6))
for category, sub in df.groupby("category"):
    plt.plot(sub["size"], sub["std_improvement_pct"], marker="o", label=category)

plt.title("Stability of Improvement vs Size (Standard Deviation)")
plt.xlabel("Dataset Size (points)")
plt.ylabel("Std. Deviation (%)")
plt.grid(True, linestyle="--", alpha=0.5)
plt.legend(title="Dataset Type")
plt.tight_layout()
plt.savefig(f"{OUTPUT_DIR}/stddev_vs_size.png", dpi=300)
print("Saved: plots/stddev_vs_size.png")

print("\nAll plots generated successfully.")
