FROM ubuntu:22.10

ENV DEBIAN_FRONTEND=noninteractive

RUN ln -sf /usr/share/zoneinfo/Asia/Tokyo /etc/localtime

RUN apt update && \
    apt upgrade -y && \
    apt install -y build-essential cmake git gcc g++ pkg-config libgtk2.0-dev \
                   python3 python3-dev python3-pip python3-setuptools libopencv-dev

RUN pip install --upgrade pip && \
    pip install numpy opencv-python-headless opencv-contrib-python
