import os
import pandas as pd
import numpy as np

algos = ["genetic", "greedy", "random", "tabu"]
files = ["A-n32-k5", "A-n37-k6", "A-n39-k5", "A-n45-k6", "A-n48-k7", "A-n54-k7", "A-n60-k9"]

for algo in algos:
    path = f"../results/results_10k/{algo}"
    filess = [f for f in os.listdir(path) if f.endswith("_costs.csv")]

    data = []

    for file_name in files:

        matching_files = [f for f in filess if file_name in f]

        all_costs = []
        for f in matching_files:
            df = pd.read_csv(os.path.join(path, f))

            col = "Cost" if "Cost" in df.columns else df.columns[0]
            all_costs.extend(df[col].dropna().tolist())

        if all_costs:
            best = np.min(all_costs)
            worst = np.max(all_costs)
            avg = np.mean(all_costs)
            std = np.std(all_costs)
        else:
            best = worst = avg = std = np.nan

        data.append([file_name, best, worst, avg, std])

    aggregated = pd.DataFrame(data, columns=["Instancja", "Best", "Worst", "Average", "Std"])
    aggregated.to_csv(os.path.join(path, "aggregated.csv"), index=False)
    print(f"Saved aggregated CSV for {algo}")
