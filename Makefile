CFLAGS = -g -O2 -Wall `pkg-config --cflags MLV`
LDFLAGS = -lMLV


main: main.o Point.o Snake.o chamferDistance.o
	g++ -o $@ $^ $(CFLAGS) $(LDFLAGS)

main.o: main.cpp Point.hpp Snake.hpp
	g++ -c $< $(CFLAGS) $(LDFLAGS)
Point.o: Point.cpp Point.hpp
	g++ -c $< $(CFLAGS) $(LDFLAGS)
Snake.o: Snake.cpp Snake.hpp Point.hpp
	g++ -c $< $(CFLAGS) $(LDFLAGS)
chamferDistance.o: chamferDistance.cpp chamferDistance.hpp
	g++ -c $< $(CFLAGS) $(LDFLAGS)
clean:
	-rm *.o
	-rm main