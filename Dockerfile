FROM debian:stable AS jdb_build

RUN \
	apt update && \
	apt install -y libcurlpp-dev nlohmann-json3-dev cmake clang

COPY src src
COPY CMakeLists.txt CMakeLists.txt

RUN mkdir build && cd build && cmake -DCMAKE_BUILD_TYPE=Release .. && cmake --build . --config Release

RUN cd build && cpack CPackConfig.cmake

FROM scratch

COPY --from=jdb_build /build/journald-bot /
COPY --from=jdb_build /build/journald-bot-2.0.0-Linux.deb /
