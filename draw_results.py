import matplotlib.pyplot as plt

filename = ("./DT_CES_SA/DT_CES_SA_output_bruteForceDataset.csv")

points = []
edges = []

with open(filename) as f:
    mode = None
    for line in f:
        line = line.strip()

        if line.startswith("Points:"):
            mode = "points"
            continue

        if line.startswith("Edges:"):
            mode = "edges"
            continue

        if line.startswith("#") or not line:
            continue

        # Skip headers
        if line.startswith("index") or line.startswith("from") or "," not in line:
            continue

        if mode == "points":
            parts = line.split(",")
            if len(parts) == 3:  # ensure valid format
                points.append((float(parts[1]), float(parts[2])))

        elif mode == "edges":
            u, v = map(int, line.split(","))
            edges.append((u, v))


# --------- Plotting -----------

xs = [p[0] for p in points]
ys = [p[1] for p in points]

plt.figure(figsize=(8, 8))
plt.scatter(xs, ys, s=80, color='black')

for i, (x, y) in enumerate(points):
    plt.text(x + 0.05, y + 0.05, str(i), fontsize=10)

for (u, v) in edges:
    x1, y1 = points[u]
    x2, y2 = points[v]
    plt.plot([x1, x2], [y1, y2], color='blue', linewidth=1)

plt.title("DT_CES_SA MWT Result")
plt.gca().set_aspect('equal', adjustable='box')
plt.grid(True, linestyle="--", alpha=0.3)
plt.show()
