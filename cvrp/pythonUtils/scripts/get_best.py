import pandas as pd
import os
import numpy as np

models = ["tabu", "genetic"]

for model in models:

    path = f"../results/results_10k/{model}/generations"

    directories = os.listdir(path)

    for dir in directories:
        lowest = 100000
        name = ""
        for i in range(10):
            data = pd.read_csv(f"{path}/{dir}/gen{i}.csv", header=None)
            data.columns = ["best", "worst", "avg"]
            if np.asarray(data["best"])[-1] < lowest:
                lowest = np.asarray(data["best"])[-1]
                name = f"{path}/{dir}/gen{i}.csv"
        os.rename(name, os.path.join(path, f"{dir}/best.csv"))