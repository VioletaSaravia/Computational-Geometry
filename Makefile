web-build:
	.\tools\emsdk\upstream\emscripten\emcmake cmake -DPLATFORM=Web -DASSIMP_BUILD_ZLIB=ON -DGLFW_BUILD_WAYLAND=OFF -DGLFW_BUILD_X11=OFF -S . -B build/web
	cmake --build build/web

web-run:
	python -m http.server