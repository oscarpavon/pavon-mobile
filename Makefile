
WORKDIR := $(shell pwd)

all:
	mkdir -p $(WORKDIR)/bin
	./scripts/create_engine_file_h.sh
	make -C ./src/engine WORKDIR=$(WORKDIR)
	make -C ./src/editor WORKDIR=$(WORKDIR)
	make -C ./src/shaders


clean:
	rm -f ./src/engine/files.h
	make -C ./src/editor WORKDIR=$(WORKDIR) clean
	make -C ./src/engine WORKDIR=$(WORKDIR) clean
	make -C ./src/shaders clean
	rm -f ./bin/peditor
