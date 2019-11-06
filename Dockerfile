FROM archlinux

RUN pacman -Syu --noconfirm && \
    pacman -S --noconfirm \
        base-devel \
        grpc \
        protobuf \
        boost \
        boost-libs \
        cmake
