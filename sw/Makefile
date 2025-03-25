build:
	mkdir build
	cmake . -B build -DPICO_BOARD=pico_w
	make -C build

flash:build/pico-mgr_board.uf2
	picotool load -f build/pico-mgr_board.uf2

clean:
	rm -rf build
