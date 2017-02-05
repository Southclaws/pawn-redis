GPP = g++
OUTFILE = "redis.so"

RED_DIR = "src/hiredis"
SDK_DIR = "src/sdk"

COMPILE_FLAGS = -fpermissive -fPIC -m32 -std=c++11 -c -O3 -w -D LINUX


all: build

build:
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/*.c
	$(GPP) $(COMPILE_FLAGS) $(SDK_DIR)/*.cpp
	$(GPP) $(COMPILE_FLAGS) -I$(SDK_DIR) -I$(SDK_DIR)/amx src/*.cpp
	$(GPP) -O2 -m32 -fshort-wchar -shared -o $(OUTFILE) *.o
	rm *.o

clean:
	-rm *~ *.o *.so
