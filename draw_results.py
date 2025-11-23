import os
import matplotlib.pyplot as plt

# =====================================================
#   parse and plot triangulation output format
# =====================================================

def plot_generated_triangulation(csv_path):

    # storage
    points = []
    edges = []

    # default mode
    mode = "points"

    with open(csv_path) as f:
        for line in f:
            line = line.strip()

            # skip blank lines
            if not line:
                continue

            # detect where edges begin
            if line.lower().startswith("edges") or line.lower().startswith("#edges"):
                mode = "edges"
                continue

            # skip header-like rows
            if "index" in line.lower() or "x,y" in line.lower():
                continue

            # skip comment lines
            if line.startswith("#"):
                continue

            # parse point rows
            if mode == "points":
                parts = line.split(",")

                # format: index,x,y
                if len(parts) == 3:
                    try:
                        points.append((float(parts[1]), float(parts[2])))
                    except ValueError:
                        continue

                # format: x,y
                elif len(parts) == 2:
                    try:
                        points.append((float(parts[0]), float(parts[1])))
                    except ValueError:
                        continue

            # parse edge rows
            elif mode == "edges":
                parts = line.split(",")
                if len(parts) == 2:
                    try:
                        edges.append((int(parts[0]), int(parts[1])))
                    except ValueError:
                        continue

    # safety check
    if not points:
        print(f"[WARN] no valid point data found in {csv_path}")
        return

    # =====================================================
    # plotting
    # =====================================================

    plt.figure(figsize=(10, 10))

    xs = [p[0] for p in points]
    ys = [p[1] for p in points]

    # draw triangulation edges
    for u, v in edges:
        x1, y1 = points[u]
        x2, y2 = points[v]
        plt.plot([x1, x2], [y1, y2], color="blue", linewidth=0.8)

    # draw points
    plt.scatter(xs, ys, s=20, color="black")

    # label points
    # for idx, (x, y) in enumerate(points):
    #     plt.text(x + 0.7, y + 0.7, str(idx), fontsize=7)

    plt.gca().set_aspect("equal", adjustable="box")
    plt.title(os.path.basename(csv_path))

    # output name
    out_path = csv_path.replace(".csv", ".png")

    plt.savefig(out_path, dpi=300)
    plt.close()

    print(f"[OK] triangulation figure saved -> {out_path}")


# =====================================================
# find and process all triangulation output files
# =====================================================

base_folder = "."

for root, _, files in os.walk(base_folder):
    for f in files:
        if f.endswith("_generated_triangulation.csv"):
            plot_generated_triangulation(os.path.join(root, f))
