import os
models = ["genetic", "greedy", "random", "tabu"]

for model in models:
    for root, dirs, files in os.walk(f"../../output/{model}"):
        for file in files:
            file_path = os.path.join(root, file)
            print(file_path)
            try:
                os.remove(file_path)
            except OSError as e:
                print(f"Failed to remove {file_path}: {e}")