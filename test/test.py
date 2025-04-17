import subprocess
import os
import threading
import time
import signal
from queue import Queue
from concurrent.futures import ThreadPoolExecutor, as_completed

CHAT = "./ipk25chat-client"
SERVER = "python3 test/server.py"
TEST_DIR = "test/test_cases"
INVALID_INPUTS_FILE = "test/invalid_inputs.txt"

GREEN = "\033[92m"
RED = "\033[91m"
RESET = "\033[0m"

invalid = Queue()
tcp = Queue()
udp = Queue()

# === INVALID ARGUMENTS TESTING ===
def load_invalid_tests(file_name):
    with open(file_name, 'r') as file:
        return [line.strip() for line in file.readlines() if line.strip()]

def test_invalid_input(index, args):
    global invalid
    try:
        subprocess.run([CHAT] + args.split(), check=True, capture_output=True, text=True)
        invalid.put((index, f"{RED}[FAIL]{RESET} [INVALID] args: {args}"))
    except subprocess.CalledProcessError as e:
        msg = e.stderr.strip().splitlines()[0] if e.stderr else "Error"
        invalid.put((index, f"{GREEN}[PASS]{RESET} [INVALID] {msg}"))

def run_invalid_tests(start_index):
    index = start_index
    for args in load_invalid_tests(INVALID_INPUTS_FILE):
        test_invalid_input(index, args)
        index += 1

# === STRUCTURED TESTING ===
def parse_test_file(file_path):
    output = False
    with open(file_path, 'r') as f:
        lines = [line.strip() for line in f.readlines() if line.strip()]
    description = lines[0]
    exit_code = 0
    commands = []
    expected_output = []

    start_index = 1
    if lines[1].startswith("EXIT CODE:"):
        try:
            exit_code = int(lines[1].split(":")[1].strip())
            start_index = 2
        except ValueError:
            raise ValueError(f"Invalid exit code in test file: {file_path}")

    for line in lines[start_index:]:
        if output:
            expected_output.append(line)
        elif line.startswith("="):
            output = True
        else:
            commands.append(line)
    return description, commands, expected_output, exit_code

def run_test(index: int, description: str, commands: list, expected_output_lines: list, protocol: str, exit_code: int, results: Queue):
    try:
        process = subprocess.Popen(
            [CHAT] + ["-t", protocol, "-s", "localhost"],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True
        )
        time.sleep(0.1)

        for line in commands:
            if line == "KILL":
                process.send_signal(signal.SIGINT)
                break
            process.stdin.write(line + "\n")
            process.stdin.flush()
            time.sleep(0.1)
        stdout, _ = process.communicate()

        output_lines = stdout.strip().splitlines()

        if process.returncode != exit_code:
            results.put((index, f"{RED}[FAIL]{RESET} [{protocol.upper()}] {description} (exit code {process.returncode})"))
            return

        if len(output_lines) < len(expected_output_lines):
            results.put((index, f"{RED}[FAIL]{RESET} [{protocol.upper()}] Number of output lines differ"))
            return

        if all(out.startswith(exp) for exp, out in zip(expected_output_lines, output_lines)):
            results.put((index, f"{GREEN}[PASS]{RESET} [{protocol.upper()}] {description}"))
        else:
            mismatch = "\n".join([
                f"Expected: {exp}\nGot:      {out}"
                for exp, out in zip(expected_output_lines, output_lines)
                if not out.startswith(exp)
            ])
            results.put((index, f"{RED}[FAIL]{RESET} [{protocol.upper()}] {description}\n{mismatch}"))

    except Exception as e:
        results.put((index, f"{RED}[ERROR]{RESET} [{protocol.upper()}] {description}\n{str(e)}"))

# === MAIN ===
def main():
    global tcp, udp
    # Start server
    server = subprocess.Popen(SERVER.split(), stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)
    time.sleep(0.5)

    print("\nRunning...")
    test_index = 1
    tasks = []
    
    # 1. Invalid input tests
    invalid_thread = threading.Thread(target=run_invalid_tests, args=(test_index,), daemon=True)
    invalid_thread.start()

    with ThreadPoolExecutor(max_workers=10) as executor:
        # 2. Valid TCP/UDP tests from files
        test_files = sorted(f for f in os.listdir(TEST_DIR) if f.startswith("test_"))
        for file_name in test_files:
            path = os.path.join(TEST_DIR, file_name)
            description, commands, expected_output, exit_code = parse_test_file(path)
            tasks.append(executor.submit(run_test, test_index, description, commands, expected_output, "tcp", exit_code, tcp))
            tasks.append(executor.submit(run_test, test_index, description, commands, expected_output, "udp", exit_code, udp))
            test_index += 1

        for task in as_completed(tasks):
            pass

    # Stop server
    server.kill()

    # Summary
    invalid_collected = []
    tcp_collected = []
    udp_collected = []
    while not invalid.empty():
        invalid_collected.append(invalid.get())
    invalid_collected.sort()
    while not tcp.empty():
        tcp_collected.append(tcp.get())
    tcp_collected.sort()
    while not udp.empty():
        udp_collected.append(udp.get())
    udp_collected.sort()

    passed = 0
    invalid_passed = 0
    tcp_passed = 0
    udp_passed = 0
    total = len(invalid_collected) + len(tcp_collected) + len(udp_collected)

    print("-----------------------------------------")
    for i, result in invalid_collected:
        print(f"TEST {i:02}: {result}")
        if "[PASS]" in result:
            passed += 1
            invalid_passed += 1

    print("-----------------------------------------")
    for i, result in tcp_collected:
        print(f"TEST {i:02}: {result}")
        if "[PASS]" in result:
            passed += 1
            tcp_passed += 1

    print("-----------------------------------------")
    for i, result in udp_collected:
        print(f"TEST {i:02}: {result}")
        if "[PASS]" in result:
            passed += 1
            udp_passed += 1

    print("-----------------------------------------")
    print("Summary:")
    print(f"Invalid tests: {invalid_passed}/{len(invalid_collected)}")
    print(f"TCP tests: {tcp_passed}/{len(tcp_collected)}")
    print(f"UDP tests: {udp_passed}/{len(udp_collected)}")
    print(f"Total: {passed}/{total}")
    print(f"Total percents: {int((passed / total) * 100)}%")

if __name__ == "__main__":
    main()
