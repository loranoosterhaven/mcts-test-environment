all:
	mkdir -p Release
	g++ *.cpp -O2 -std=c++0x -o Release/mcts-test-environment.out