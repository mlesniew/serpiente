CFLAGS=$(shell pkg-config --cflags allegro)
CXXFLAGS=-O3
LDFLAGS=$(shell pkg-config --libs allegro) -Wl,-rpath,.
EXECUTABLE=serpiente

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

serpiente: Serpiente.cpp
	$(CXX) $< $(LDFLAGS) $(CXXFLAGS) $(CFLAGS) -o $@

.PHONY: all clean
