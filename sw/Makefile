all: build/
	make -C build

build/:
	mkdir build
	cmake . -B build 

flash: build/
	picotool load -f build/pico-mgr_board.uf2

clean:
	rm -rf build
