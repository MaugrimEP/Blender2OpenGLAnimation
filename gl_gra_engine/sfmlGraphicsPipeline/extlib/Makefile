all: build_freetype build_tinyobjloader

build_freetype:
	cd freetype-2.5.5/ ; mkdir build ; cd build/ && cmake ../ && make -j 8

build_tinyobjloader:
	cd tinyobjloader/ ; mkdir build ; cd build/ && cmake ../ && make -j 8

clean_all: clean_freetype clean_tinyobjloader

clean_freetype:
	cd freetype-2.5.5/ && rm -r build/

clean_tinyobjloader:
	cd tinyobjloader && rm -rf build/
