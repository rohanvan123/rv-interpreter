FROM gcc:latest

RUN apt-get update && apt-get install -y valgrind

WORKDIR /usr/src/app
COPY . .

RUN g++ -g -std=c++20 src/main.cpp -o bin/test

CMD ["valgrind", "--leak-check=full", "--show-leak-kinds=all", "./bin/test", "./test_code/complex_string.rv"]