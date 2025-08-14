FROM gcc:latest

RUN apt-get update && apt-get install -y valgrind

WORKDIR /usr/src/app
COPY . .

RUN make

CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./bin/test", "./test_code/complex_list.rv", "--output-lexer", "output-parser"]