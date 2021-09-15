CFLAGS=$(shell pkg-config --cflags allegro)
CXXFLAGS=-O3
LDFLAGS=$(shell pkg-config --libs allegro) -Wl,-rpath,.
EXECUTABLE=serpiente

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

$(EXECUTABLE): $(wildcard *.cpp)
	$(CXX) $(CXXFLAGS) $(CFLAGS) $^ $(LDFLAGS) -o $@

.PHONY: all clean
