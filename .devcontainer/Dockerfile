FROM debian:bookworm

ARG DEVKITPRO=/opt/devkitpro
ENV DEVKITPRO=$DEVKITPRO

# install basic deps and cleanup in one layer
RUN apt update && apt install -y \
    make build-essential wget curl git \
    && apt autoremove -y && apt clean && rm -rf /var/lib/apt/lists/*

# install devkitpro pacman
RUN wget https://apt.devkitpro.org/install-devkitpro-pacman \
    && chmod +x ./install-devkitpro-pacman \
    && sed -i 's/apt-get install/apt-get -y install/g' ./install-devkitpro-pacman \
    && ./install-devkitpro-pacman \
    && rm ./install-devkitpro-pacman

# Fix the missing /etc/mtab and then sync dkp pacman and install packages
RUN ln -s /proc/mounts /etc/mtab && \
    dkp-pacman -Sy && \
    dkp-pacman -S --noconfirm gba-dev

# set devkitpro variables
ARG DKP_TOOLS_PATH=$DEVKITPRO/tools/bin
ENV DKP_TOOLS_PATH=$DKP_TOOLS_PATH

# install butano

RUN apt update && apt install -y \
    python3 python-is-python3 \
    && apt autoremove -y && apt clean && rm -rf /var/lib/apt/lists/*

RUN git clone https://github.com/GValiente/butano.git /opt/butano

ENV LIBBUTANO=/opt/butano

