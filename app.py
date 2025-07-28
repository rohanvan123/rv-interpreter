import subprocess
from flask import Flask, request
from flask_cors import CORS
import time
import os

app = Flask(__name__)
CORS(app)
app.config["CORS_HEADER"] = 'Content-Type'

DELIMITER = "================================="

@app.route('/')
def hello():
    return "API base"

def write_program_to_file(program, filename):
    with open(filename, 'w') as program_file:
        # Iterate over each line in code_commands and write it to the file
        for line in program:
            program_file.write(line.rstrip() + '\n')
    
    return

def interpret_program(program_file):
    # compile_res = subprocess.run("g++ -std=c++20 src/main.cpp -o bin/main -w", shell=True, text=True, capture_output=True)
    exe_res = subprocess.run(f"./bin/main {program_file}", shell=True, text=True, capture_output=True)
    return exe_res

def parse_program_output(raw_ouput):
    outputs = raw_ouput.split(DELIMITER)
    lexer_ouput = outputs[0]
    lines = lexer_ouput.split('\n')[:-1]
    tokens_per_line = [line.split(",") for line in lines]
    for i in range(len(tokens_per_line)):
        tokens_per_line[i] = [tok.strip() for tok in tokens_per_line[i]]

    parser_output = outputs[1]
    ast_sequence = parser_output.split('\n')[1:-1]

    evaluator_output = outputs[2]
    progam_output = evaluator_output.split('\n')[1:-1]

    body = {
        "tokens": tokens_per_line,
        "ast_sequence": ast_sequence,
        "progam_output": progam_output
    }
    return body


@app.route('/interpret', methods=["POST", "OPTIONS"])
def interpret():
    if request.method == "OPTIONS":
        # Respond to preflight request with 204 No Content
        return '', 204
    
    body = request.json
    code_commands = body["code"]
    file_path = "./test_code/test.rv"

    if os.path.isfile(file_path):
        # clear the contents
        open(file_path, 'w').close()

    write_program_to_file(code_commands, file_path)
    exec_result = interpret_program("test_code/test.rv")
    response_body = parse_program_output(exec_result.stdout)

    return response_body, 200
