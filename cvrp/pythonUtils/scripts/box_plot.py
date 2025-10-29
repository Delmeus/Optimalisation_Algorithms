import pandas as pd
import matplotlib.pyplot as plt

#
data = pd.DataFrame({
    "Algorithm": ["Genetic", "Tabu", "Greedy", "Random"],
    "Average": [1420.7, 1597.3, 1844.6, 3003.8],
    "StdDev": [37.84, 38.66, 68.05, 66.24],
    "Min": [1364, 1532, 1693, 2887],
    "Max": [1481, 1684, 2012, 3091],
    "Optimal": [1354, 1354, 1354, 1354],
})


fig, ax = plt.subplots(figsize=(8, 8))

for i, row in data.iterrows():
    x = i + 1
    avg = row["Average"]
    std = row["StdDev"]
    min_val = row["Min"]
    max_val = row["Max"]
    optimal = row["Optimal"]

    # Draw the box = avg ± std
    ax.add_patch(plt.Rectangle(
        (x - 0.3, avg - std),
        0.6,
        2 * std,
        facecolor='skyblue',
        edgecolor='black',
        alpha=0.6
    ))

    ax.plot([x, x], [min_val, avg - std], color='black', lw=1.5)
    ax.plot([x, x], [avg + std, max_val], color='black', lw=1.5)

    ax.plot([x - 0.15, x + 0.15], [min_val, min_val], color='black', lw=1.5)
    ax.plot([x - 0.15, x + 0.15], [max_val, max_val], color='black', lw=1.5)

    ax.plot([x - 0.3, x + 0.3], [avg, avg], color='blue', lw=2)

    ax.plot([x - 0.4, x + 0.4], [optimal, optimal], color='red', lw=2, linestyle='--')

# Formatting
ax.set_xticks(range(1, len(data) + 1))
ax.set_xticklabels(data["Algorithm"], ha='right')
ax.set_ylabel("Cost")
ax.set_title("Box Plot for A-n60-k9")
ax.grid(True, linestyle='--', alpha=0.5)
plt.tight_layout()
plt.show()
plt.savefig("box_plot.png")
