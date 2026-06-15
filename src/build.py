import subprocess
import shutil
import sys
import os

SCRIPT_DIR = os.path.dirname(os.path.abspath(__file__))
SRC_DIR    = SCRIPT_DIR
REPO_ROOT  = os.path.dirname(SRC_DIR)

def get_cpp_files() -> list[str]:
    exclude_dirs = {
        "Testing",
        "src/chess-library",
    }
    
    cpp_files = []
    for root, dirs, files in os.walk(REPO_ROOT):
        # Normalize path for exclusion
        rel_root = os.path.relpath(root, REPO_ROOT).replace("\\", "/")
        if any(rel_root.startswith(ex) for ex in exclude_dirs):
            continue
        for file in files:
            if file.endswith(".cpp"):
                cpp_files.append(os.path.relpath(os.path.join(root, file), REPO_ROOT))
    return cpp_files

def get_chess_library_include() -> str | None:
    lib_in_src = os.path.join(SRC_DIR, "chess-library")

    if os.path.isdir(lib_in_src):
        return "src/chess-library"

    return None

def get_build_cmd(server: bool = False) -> str:
    cpp_files = get_cpp_files()
    cpp_files_str = " ".join(cpp_files)
    include_path = get_chess_library_include()

    if include_path is None:
        raise FileNotFoundError("[Fyn] Could not locate chess-library in src/chess-library.")

    march_flag = "" if server else "-march=native"

    return f"g++ {cpp_files_str} -I {include_path} -I src -std=c++20 -O3 {march_flag} -pthread -o Fyn"

def run_subprocess(cmd : str) -> bool:
    try:
        subprocess.run(cmd, shell=True, check=True)
        return True
    except subprocess.CalledProcessError:
        print(f"[Fyn] Subprocess command failed: {cmd}")
        return False

def check_dependencies() -> bool:
    if not shutil.which("git"):
        print("[Fyn] Git is not installed or not in PATH.")
        return False

    if not shutil.which("g++"):
        print("[Fyn] g++ is not installed or not in PATH.")
        return False

    return True

def check_lib_in_dir() -> bool:
    return get_chess_library_include() is not None

def build_fyn(server : bool = False) -> None:
    print("🛠️  [Fyn] Chess library found. Compiling Fyn...")
    build_cmd = get_build_cmd(server=server)
    if run_subprocess(build_cmd):
        print("✅ [Fyn] Fyn Build complete.")
        print("🎮 [Fyn] To use Fyn type in terminal: ./Fyn")

def ask_to_download_chess_lib() -> bool:
    verification : str = input(f"[Fyn] Disservin's chess library was not found in src/chess-library. Download it now? [y/n]: ").strip().lower()

    if verification == 'y':
        return True
    
    print("[Fyn] Build cancelled. You can manually clone:")
    print("      https://github.com/Disservin/chess-library.git")
    return False

def download_chess_lib() -> None:
    run_subprocess("git clone https://github.com/Disservin/chess-library.git src/chess-library")

# Build script
if __name__ == "__main__":
    is_server_build : bool = "--server" in sys.argv

    os.chdir(REPO_ROOT)

    if not check_dependencies():
        sys.exit(1)

    if check_lib_in_dir():
        build_fyn(server=is_server_build)
    else:
        should_download : bool = True if is_server_build else ask_to_download_chess_lib()
        
        if should_download:
            download_chess_lib()
            build_fyn(server=is_server_build)

    