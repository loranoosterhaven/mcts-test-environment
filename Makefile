all:
	mkdir -p Release
	g++ *.cpp -O2 -o Release/mcts-test-environment.out