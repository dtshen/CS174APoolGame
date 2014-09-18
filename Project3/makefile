TARGET   = glutharness

CC       = g++
CFLAGS   = -c -g -DDEBUG -DLINUX -w
LDFLAGS  = -lGL -lGLU -lglut -lGLEW
SRC      = $(wildcard ./*.cpp ./*.cxx)
OBJ      = $(SRC:.cpp=.o)

all: $(TARGET)
	
$(TARGET): $(OBJ) 
	$(CC) $(OBJ) -o $(TARGET) $(LDFLAGS)

#example1.o: Angel.h InitShader.o
#InitShader.o: Angel.h

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f ./*.o
	rm -f $(TARGET)
