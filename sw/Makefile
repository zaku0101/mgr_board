build:
	mkdir build
	cmake . -B build 
	make -C build

flash:build/pico-mgr_board.uf2
	picotool load -f build/pico-mgr_board.uf2

clean:
	rm -rf build
