GPP = g++
OUTFILE = redis.so

RED_DIR = src/hiredis
SDK_DIR = src/sdk

COMPILE_FLAGS = -fpermissive -fPIC -m32 -std=c++11 -c -O3 -w -D LINUX
LINK_FLAGS = -Wl,--no-undefined -O2 -m32 -fshort-wchar -shared

TEST_SERVER_DIR = ../samp037svr_R2-1

.DEFAULT_GOAL = test


build/amxplugin.o: $(SDK_DIR)/amxplugin.cpp
	$(GPP) $(COMPILE_FLAGS) $(SDK_DIR)/amxplugin.cpp -o build/amxplugin.o
build/amxplugin2.o: $(SDK_DIR)/amxplugin2.cpp
	$(GPP) $(COMPILE_FLAGS) $(SDK_DIR)/amxplugin2.cpp -o build/amxplugin2.o

build/async.o: $(RED_DIR)/async.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/async.c -o build/async.o
build/dict.o: $(RED_DIR)/dict.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/dict.c -o build/dict.o
build/hiredis.o: $(RED_DIR)/hiredis.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/hiredis.c -o build/hiredis.o
build/net.o: $(RED_DIR)/net.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/net.c -o build/net.o
build/read.o: $(RED_DIR)/read.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/read.c -o build/read.o
build/sds.o: $(RED_DIR)/sds.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/sds.c -o build/sds.o
build/test.o: $(RED_DIR)/test.c
	$(GPP) $(COMPILE_FLAGS) $(RED_DIR)/test.c -o build/test.o

build/impl.o: src/impl.cpp
	$(GPP) $(COMPILE_FLAGS) -I$(SDK_DIR) -I$(SDK_DIR)/amx src/impl.cpp -o build/impl.o
build/main.o: src/main.cpp
	$(GPP) $(COMPILE_FLAGS) -I$(SDK_DIR) -I$(SDK_DIR)/amx src/main.cpp -o build/main.o
build/natives.o: src/natives.cpp
	$(GPP) $(COMPILE_FLAGS) -I$(SDK_DIR) -I$(SDK_DIR)/amx src/natives.cpp -o build/natives.o

$(OUTFILE): build/amxplugin.o build/amxplugin2.o build/async.o build/dict.o build/hiredis.o build/net.o build/read.o build/sds.o build/test.o build/impl.o build/main.o build/natives.o
	$(GPP) $(LINK_FLAGS) -o $(OUTFILE) build/*.o

all: $(OUTFILE)

test: $(OUTFILE)
	# 'test' assumes valid SA:MP server exists at: $(TEST_SERVER_DIR)
	# and it's configured to load the `samp-redis-tests` gamemode script.
	cp test/samp-redis-tests.amx $(TEST_SERVER_DIR)/gamemodes/samp-redis-tests.amx
	cp $(OUTFILE) $(TEST_SERVER_DIR)/plugins/$(OUTFILE)
	# now run the server

clean:
	-rm build/*.o *.so
