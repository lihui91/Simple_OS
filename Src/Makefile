CC = g++
CXX_FLAGS = -std=c++11 -w -g -rdynamic
# gdbµ÷ÊÔÑ¡Ïî -g -rdynamic

BIN = secondfilesystem

$(BIN): main.o Buf.o  BufferManager.o DiskManager.o File.o FileManager.o FileSystem.o INode.o OpenFileManager.o User.o
	$(CC) -o $@ $^
main.o: main.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
Buf.o: Buf.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
BufferManager.o: BufferManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
DiskManager.o: DiskManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
File.o: File.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
FileManager.o: FileManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
FileSystem.o: FileSystem.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
INode.o: INode.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
OpenFileManager.o: OpenFileManager.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
User.o: User.cpp
	$(CC) $(CXX_FLAGS) -c -o $@ $<
	
	
.phony:
clean:
	rm -f *.o $(BIN)
	rm -f *.img