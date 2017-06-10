all:
	mkdir -p Release
	g++ *.cpp -O2 -o -std=c++0x Release/mcts-test-environment.out