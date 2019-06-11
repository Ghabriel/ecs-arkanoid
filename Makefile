all:
	g++ src/main.cpp src/systems/**/*.cpp -o main -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system
