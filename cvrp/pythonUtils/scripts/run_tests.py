import shutil
import subprocess
import os

cpp_path = "../../build/cmake-build-release"
python_path = os.getcwd()
print(python_path)
def remove_files():
    for root, dirs, files in os.walk("../results"):
        for file in files:
            file_path = os.path.join(root, file)
            print(file_path)
            try:
                os.remove(file_path)
            except OSError as e:
                print(f"Failed to remove {file_path}: {e}")

parameters = {"10k": "10000"}
remove_files()
os.chdir("../../build/cmake-build-release")
filenames = os.listdir("../../input")
print(f"\n\n{filenames}\n\n")

for key, value in parameters.items():
    for filename in filenames:
        subprocess.run(['.\\algorytmy_optymalizacji.exe', filename, value])

    dest_dir = f"../../pythonUtils/results/results_{key}"
    os.makedirs(dest_dir, exist_ok=True)

    for root, _, files in os.walk("../../output"):
        for file in files:
            src = os.path.join(root, file)
            rel_path = os.path.relpath(src, "../../output")
            dest = os.path.join(dest_dir, rel_path)
            os.makedirs(os.path.dirname(dest), exist_ok=True)
            shutil.copy2(src, dest)
