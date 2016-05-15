BIN_DIR := ../../bin/

OBJECTS := \
	basketrec.o

basketrec: $(OBJECTS)
	g++ -O3 $(OBJECTS) -o $(BIN_DIR)basketrec

%.o: %.cpp
	g++ -O3 -Wall -c $< -o $@

clean:	clean_lib
	rm -f $(BIN_DIR)basketrec

clean_lib:
	rm -f $(OBJECTS)

