GPP = g++
OUTFILE = "redis.so"

RED_DIR = "src/hiredis"
SDK_DIR = "src/sdk"

COMPILE_FLAGS = -fpermissive -fPIC -m32 -std=c++11 -c -O3 -w -D LINUX
LINK_FLAGS = -Wl,--no-undefined -O2 -m32 -fshort-wchar -shared

TEST_SERVER_DIR = ../samp037svr_R2-1


all: build

build:
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/*.c
	$(GPP) $(COMPILE_FLAGS) $(SDK_DIR)/*.cpp
	$(GPP) $(COMPILE_FLAGS) -I$(SDK_DIR) -I$(SDK_DIR)/amx src/*.cpp
	$(GPP) $(LINK_FLAGS) -o $(OUTFILE) *.o
	rm *.o

test: build
	echo "'test' assumes valid SA:MP server exists at:"
	echo $(TEST_SERVER_DIR)
	echo "and that it's set to load any of the filterscripts named in the 'test' directory."
	# copy the test AMX to filterscripts
	cp test/test_connect.amx $(TEST_SERVER_DIR)/filterscripts/test_connect.amx
	# copy the plugin binary to plugins directory
	cp $(OUTFILE) $(TEST_SERVER_DIR)/plugins/$(OUTFILE)
	# run the server
	$(TEST_SERVER_DIR)/samp03svr

clean:
	-rm *~ *.o *.so
