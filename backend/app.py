import subprocess
from flask import Flask, request
from flask_cors import CORS, cross_origin
import json
import os

app = Flask(__name__)
CORS(app)
app.config["CORS_HEADER"] = 'Content-Type'

@app.route('/')
def hello():
    return "API base"

def write_program_to_file(program, filename):
    with open(filename, 'w') as program_file:
        # Iterate over each line in code_commands and write it to the file
        for line in program:
            program_file.write(line.strip() + '\n')
    
    return

def interpret_program(program_file):
    os.chdir("..")
    # compile_res = subprocess.run("g++ -std=c++20 main.cpp -o bin/main -w", shell=True, text=True, capture_output=True)
    exe_res = subprocess.run(f"bin/main {program_file}", shell=True, text=True, capture_output=True)
    
    return exe_res

def parse_program_output(raw_ouput):
    chunks = raw_ouput.split("\n")[:-1]
    tokens = chunks[0].split(",")
    ast_sequence = [chunks[i] for i in range(1, len(chunks) - 1)]
    progam_output = chunks[-1]

    body = {
        "tokens": tokens,
        "ast_sequence": ast_sequence,
        "progam_output": progam_output
    }
    return body


@app.route('/interpret', methods=["POST"])
def interpret():
    body = request.json
    print(body)
    code_commands = body["code"]
    print(code_commands)
    file_path = "../test_code/test.rv"

    if os.path.isfile(file_path):
        # clear the contents
        open(file_path, 'w').close()

    write_program_to_file(code_commands, file_path)
    exec_result = interpret_program("test_code/test.rv")
    response_body = parse_program_output(exec_result.stdout)
    os.chdir("./backend")
    print(json.dumps(response_body, indent=2))
    

    

    return response_body, 200
