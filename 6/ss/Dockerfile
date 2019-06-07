FROM       ubuntu:16.04

RUN sed -i 's@archive.ubuntu.com@mirror.kakao.com@g' /etc/apt/sources.list

RUN        apt-get -y update

RUN apt-get -y install build-essential

COPY . /usr/src/fileio

WORKDIR /usr/src/fileio

RUN     make

CMD     ["./test.sh"]
