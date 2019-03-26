CFLAGS=-O2 $(shell pkg-config --cflags allegro)
LDFLAGS=$(shell pkg-config --libs allegro)
EXECUTABLE=serpiente

all: $(EXECUTABLE)

clean:
	rm -f $(EXECUTABLE)

serpiente: Serpiente.cpp
	$(CXX) $< $(LDFLAGS) $(CFLAGS) -o $@

.PHONY: all clean
