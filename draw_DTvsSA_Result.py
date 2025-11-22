import matplotlib.pyplot as plt

filename = "./DT_CES_SA/compare_triangulations_output_bruteForceDataset.csv"

points = []
dt_edges = []
sa_edges = []

with open(filename) as f:
    mode = None
    for line in f:
        line = line.strip()

        # Switch modes
        if line.startswith("Points:"):
            mode = "points"
            continue
        if line.startswith("#DT_EDGES"):
            mode = "dt"
            continue
        if line.startswith("#SA_EDGES"):
            mode = "sa"
            continue

        # Skip blank or header lines
        if not line or line in ["x,y", "index,x,y", "u,v", "from,to"]:
            continue

        # ---- Parse data ----
        if mode == "points":
            try:
                x, y = map(float, line.split(","))
                points.append((x, y))
            except ValueError:
                continue   # Ignore malformed rows

        elif mode == "dt":
            try:
                u, v = map(int, line.split(","))
                dt_edges.append((u, v))
            except ValueError:
                continue

        elif mode == "sa":
            try:
                u, v = map(int, line.split(","))
                sa_edges.append((u, v))
            except ValueError:
                continue


# --------- Plotting ---------

xs = [p[0] for p in points]
ys = [p[1] for p in points]

fig, axes = plt.subplots(1, 2, figsize=(14, 7), sharex=True, sharey=True)

plots = [
    ("Delaunay Triangulation", dt_edges, "red"),
    ("Simulated Annealing Result", sa_edges, "blue")
]

for ax, (title, edges, color) in zip(axes, plots):
    ax.scatter(xs, ys, s=80, color='black')

    # Labels
    for i, (x, y) in enumerate(points):
        ax.text(x + 0.05, y + 0.05, str(i), fontsize=10)

    # Draw edges
    for (u, v) in edges:
        x1, y1 = points[u]
        x2, y2 = points[v]
        ax.plot([x1, x2], [y1, y2], color=color, linewidth=1.5)

    ax.set_title(title)
    ax.set_aspect("equal", adjustable="box")
    ax.grid(True, linestyle="--", alpha=0.3)

plt.tight_layout()
plt.show()
