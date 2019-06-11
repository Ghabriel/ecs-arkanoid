all:
	g++ src/main.cpp src/systems/level-loading-system.cpp -o main -std=c++17 -lsfml-graphics -lsfml-window -lsfml-system
