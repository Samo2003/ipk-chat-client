import subprocess
import re

CHAT = "./ipk-25-chat"
RED = "\033[91m"
GREEN = "\033[92m"
RESET = "\033[0m"

def load_tests(file_name):
    with open(file_name, 'r') as file:
        return [line.strip() for line in file.readlines() if line.strip()]
    
INVALID_TESTS = load_tests("test/invalid_inputs.txt")

total_tests = 0
successful_invalid_tests = 0
invalid_tests = 0

def test_invalid_input(args: str):
    global successful_invalid_tests, invalid_tests, total_tests
    total_tests += 1
    invalid_tests += 1

    try:
        subprocess.run([CHAT] + args.split(), check=True, capture_output=True, text=True)
    except subprocess.CalledProcessError as e:
        print(f"TEST {invalid_tests}: \t{GREEN}[PASS]{RESET} \t{e.stderr}", end="")
        successful_invalid_tests += 1
        return

    print(f"TEST {invalid_tests}: \t{RED}[FAIL]{RESET} \targs: {args}")
    
print("Testing invalid inputs:")
for invalid in INVALID_TESTS:
    test_invalid_input(invalid)

print("-----------------------------------------")
print("Summary:")
print(f"Invalid inputs: {successful_invalid_tests}/{invalid_tests}")
percentage = successful_invalid_tests/total_tests * 100
print(f"Total percents: {int(percentage)}%")