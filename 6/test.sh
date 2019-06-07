#!/bin/bash

rm -f student.hsh

echo "해시 테이블 (크기 == 10)"
./a.out -c 10

echo "검색"
echo "123 검색"
./a.out -s 123
echo "52 검색"
./a.out -s 52
echo "2859190 검색"
./a.out -s 2859190
echo "2014230032 검색"
./a.out -s 2014230032
echo "1492 검색"
./a.out -s 1492

echo "123 삭제"
./a.out -d 123
./a.out -s 123
echo "2859190 삭제"
./a.out -d 2859190
./a.out -s 2859190
echo "1492 삭제"
./a.out -d 1492
./a.out -s 1492

od -c student.hsh

echo "해시 테이블 (크기 == 5)"
./a.out -c 5

echo "검색"
echo "123 검색"
./a.out -s 123
echo "52 검색"
./a.out -s 52
echo "2859190 검색"
./a.out -s 2859190
echo "2014230032 검색"
./a.out -s 2014230032
echo "1492 검색"
./a.out -s 1492

echo "52 삭제"
./a.out -d 52
./a.out -s 52
echo "2014230032 삭제"
./a.out -d 2014230032
./a.out -s 2014230032

od -c student.hsh
