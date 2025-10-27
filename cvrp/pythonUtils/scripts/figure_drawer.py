import matplotlib
import os
matplotlib.use("TkAgg")

import pandas as pd
import matplotlib.pyplot as plt
from scipy.signal import savgol_filter

models = {"genetic": "Genetic", "tabu": "Tabu"}

for key, value in models.items():

    path = f"../results/results_10k/{key}/generations"

    directories = os.listdir(path)

    for dir in directories:
        data = pd.read_csv(f"{path}/{dir}/best.csv", header=None)
        data.columns = ["best", "worst", "avg"]
        plt.figure(figsize=(10, 6))
        plt.plot(savgol_filter(data["best"], window_length=9, polyorder=2), label="Best", color="green", linewidth=2)
        plt.plot(savgol_filter(data["worst"], window_length=9, polyorder=2), label="Worst", color="red", linewidth=2)
        plt.plot(savgol_filter(data["avg"], window_length=9, polyorder=2), label="Average", color="blue", linewidth=2)
        plt.title(f"{value} Algorithm Progress {dir}")
        plt.xlabel("Generation")
        plt.ylabel("Cost")
        plt.legend()
        plt.grid(True)
        plt.tight_layout()
        plt.savefig(f"{path}/{dir}/wykres.png")