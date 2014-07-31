CXX=g++
INC=-I ./include
CXXFLAG=-g -Wall -DDEBUG

all:
	mkdir -p ./lib;
	$(CXX) $(INC) $(CXXFLAG) -c ./src/buffer.cpp -o ./lib/buffer.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/config.cpp -o ./lib/config.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/event_base.cpp -o ./lib/event_base.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/handler.cpp -o ./lib/handler.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/handler/handler_listener.cpp -o ./lib/handler_listener.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/handler/handler_server.cpp -o ./lib/handler_server.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/handler/handler_client.cpp -o ./lib/handler_client.o

	$(CXX) $(INC) $(CXXFLAG) -c ./src/handler/logic/handler_listener_echo.cpp -o ./lib/handler_listener_echo.o
	$(CXX) $(INC) $(CXXFLAG) -c ./src/handler/logic/handler_server_echo.cpp -o ./lib/handler_server_echo.o
	$(CXX) $(INC) $(CXXFLAG) ./src/main.cpp -o ./vas.exe ./lib/*.o

clean:
	rm -rf ./lib
	rm ./vas.exe
