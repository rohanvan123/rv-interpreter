import unittest
import subprocess

class TestCppProgram(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        # Compile once before all tests
        result = subprocess.run("make", shell=True, text=True, capture_output=True)
        if result.returncode != 0:
            raise RuntimeError("Compilation failed:\n" + result.stderr)

    def run_test_case(self, input_file, expected_file, test_name):
        print(f"🔍 Running test case: {test_name}")
        result = subprocess.run(["./bin/test", input_file, "--output-lexer", "--output-parser"], capture_output=True, text=True)
        self.assertEqual(result.returncode, 0, "Program crashed or exited with error")

        with open(expected_file) as f:
            expected = f.read().strip()

        actual = result.stdout.strip()
        self.assertEqual(actual, expected)
        print(f"✅ {test_name} passed!")

    def test_case_1(self):
        test_name = "simple_add"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_2(self):
        test_name = "simple_seq"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_3(self):
        test_name = "simple_ooo"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_4(self):
        test_name = "simple_if"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_5(self):
        test_name = "simple_reassign"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_6(self):
        test_name = "complex_if"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_7(self):
        test_name = "simple_while"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_8(self):
        test_name = "complex_while"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_9(self):
        test_name = "simple_string"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)

    def test_case_10(self):
        test_name = "complex_string"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)

    def test_case_11(self):
        test_name = "simple_array"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_12(self):
        test_name = "simple_size"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_13(self):
        test_name = "simple_function"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)
    
    def test_case_14(self):
        test_name = "complex_function"
        self.run_test_case(f"test_code/{test_name}.rv", f"test_outputs/expected_{test_name}.txt", test_name)

if __name__ == '__main__':
    unittest.main()
