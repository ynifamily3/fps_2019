OBJECTS	= student.o
TARGET	= ./a.out
CC		= gcc
CFLAGS	= -W -Wall

$(TARGET) : $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

%.o : %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean :
	-rm $(OBJECTS) $(TARGET) student.dat student.idx 2>/dev/null

test :
	@-rm student.dat student.idx 2>/dev/null # 기존 파일을 제거하고
	@echo "test #1 - 데이터 한 개 추가"	
	@-$(TARGET) -a "20142455" "Jongkeun Choi" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	#@-hd student.idx
	#@-hd student.dat
	@echo "---------------------------------------------------------------------------------------------------------------------"
	@echo "test #2 - 데이터 한 개 삭제"
	@-$(TARGET) -d "20142455"
	#@-hd student.idx
	#@-hd student.dat
	@echo "---------------------------------------------------------------------------------------------------------------------"
	@echo "test #3 - 좀 더 짧은 데이터 추가"
	@-$(TARGET) -a "20142303" "shk.im" "Computer Science" "3" "hongcheon, Gangwon-do" "010-xxxx-yyyy" "niceb5y@gmail.com"
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #4 - 검색"
	@-$(TARGET) -s "20142303"
	@echo "test #5 - 데이터 3 개 추가"
	@-$(TARGET) -a "20150001" "Shinyoung-Tae" "computer law" "9" "heaven" "010-1111-zzzz" "god@tae.com"
	@-$(TARGET) -a "20150002" "Nisio-isin" "Author" "0" "hell" "017-1199-2288" "ishin@japan.jp"
	@-$(TARGET) -a "20150003" "honobu-yonezawa" "bing-gwa" "5" "yo-japan" "817-1111-zzxx" "yonezawa@honobu.com"
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #6 - 검색"
	@-$(TARGET) -s "20142303"
	@-$(TARGET) -s "20150001"
	@-$(TARGET) -s "20150002"
	@-$(TARGET) -s "20150003"
	@echo "test #7 - 중간 2개 삭제"
	@-$(TARGET) -d "20150001"
	#@-hd student.idx
	#@-hd student.dat
	@-$(TARGET) -d "20150002"
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #7 - 검색"
	@-$(TARGET) -s "20142303"
	@-$(TARGET) -s "20150001"
	@-$(TARGET) -s "20150002"
	@-$(TARGET) -s "20150003"
ifeq ("x","y") # 가짜 여러줄 주석용
	@echo "test #8 - 삽입(삭제된 위치 재활용)"
	$(TARGET) -a "20150004" "newface" "new" "0" "hav" "010-123" "short@email" # 삽입했는데 헤더가 -1이 되는 버그가 있음 삽입하면 헤더가 기존에 이 포지션을 가져야함
	#@-hd student.idx
	#@-hd student.dat
	$(TARGET) -a "20150005" "newface2" "new2" "2" "haz" "110-123" "short@em" 
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #9 - 검색"
	@-$(TARGET) -s "20142303"
	@-$(TARGET) -s "20150001"
	@-$(TARGET) -s "20150002"
	@-$(TARGET) -s "20150003"
	@-$(TARGET) -s "20150004"
	@-$(TARGET) -s "20150005"
	@echo "test #10 - 삭제"
	@-$(TARGET) -d "20142303"
	@-$(TARGET) -d "20150003"
	@-$(TARGET) -d "20150004"
	@-$(TARGET) -d "20150005"
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #11 - 4개의 작은 데이터 할당 (재사용성 테스트)"
	$(TARGET) -a "1" "2" "3" "4" "5" "6" "7" 
	$(TARGET) -a "2" "2" "3" "4" "5" "6" "7" 
	$(TARGET) -a "3" "2" "3" "4" "5" "6" "7" 
	$(TARGET) -a "4" "2" "3" "4" "5" "6" "7" 
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #12 - 검색"
	@-$(TARGET) -s "1"
	@-$(TARGET) -s "2"
	@-$(TARGET) -s "3"
	@-$(TARGET) -s "4"
	@echo "test #13 - 삭제"
	@-$(TARGET) -d "1"
	@-$(TARGET) -d "2"
	@-$(TARGET) -d "3"
	@-$(TARGET) -d "4"
	@echo "test #14 - 4개의 좀 더 큰 데이터 할당 (재사용성 테스트)"
	$(TARGET) -a "1" "20" "30" "4" "50" "60" "70" 
	$(TARGET) -a "2" "20" "30" "4" "50" "60" "70" 
	$(TARGET) -a "3" "20" "30" "4" "50" "60" "70" 
	$(TARGET) -a "4" "20" "30" "4" "50" "60" "70" 
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #15 - 검색"
	@-$(TARGET) -s "1"
	@-$(TARGET) -s "2"
	@-$(TARGET) -s "3"
	@-$(TARGET) -s "4"
	@echo "test #16 - 삭제"
	@-$(TARGET) -d "1"
	@-$(TARGET) -d "2"
	@-$(TARGET) -d "3"
	@-$(TARGET) -d "4"
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #18 - 검색 시도 (안나옴)"
	@-$(TARGET) -s "1"
	@-$(TARGET) -s "2"
	@-$(TARGET) -s "3"
	@-$(TARGET) -s "4"
	@echo "test #19 - 재생성"
	@echo "하나"
	@-$(TARGET) -a "20190001" "Jongkeun" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	@echo "둘"
	@-$(TARGET) -a "20190002" "Jongkeun Choi2" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	@echo "셋"
	@-$(TARGET) -a "20190003" "Jongkeun Choi3" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	@echo "넷"
	@-$(TARGET) -a "20190004" "Jongkeun Choi4" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	@echo "다섯"
	@-$(TARGET) -a "20190005" "Jongkeun Choi5" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	@echo "여섯"
	@-$(TARGET) -a "20190006" "Jongkeun Choi6" "Computer Science" "3" "Dongjak-gu, Seoul" "010-2878-3876" "ynifamily3@gmail.com"
	#@-hd student.idx
	#@-hd student.dat
	@echo "test #20 - 임의의 순서대로 삭제"
#	@-$(TARGET) -d "20190002"
#	@-$(TARGET) -d "20190004"
#	@-$(TARGET) -d "20190006"
#	@-$(TARGET) -d "20190005"
#	@-$(TARGET) -d "20190003"
#	@-$(TARGET) -d "20190001"
	@-$(TARGET) -d "20190001"
	@-$(TARGET) -d "20190002"
	@-$(TARGET) -d "20190003"
	@-$(TARGET) -d "20190004"
	@-$(TARGET) -d "20190005"
	@-$(TARGET) -d "20190006"
	@echo "test #21 - 남은 데이터 없음"
	@-$(TARGET) -s "20190002"
	@-$(TARGET) -s "20190004"
	@-$(TARGET) -s "20190006"
	@-$(TARGET) -s "20190005"
	@-$(TARGET) -s "20190001"
	@-$(TARGET) -s "20190003"
	@-hd student.idx
	@-hd student.dat
	@echo "test #22 - 삭제 공간 재활용성 테스트 (적절한 데이터 기록)"
	@echo "하나"
	@-$(TARGET) -a "2014" "paradise" "law" "1" "aaddr " "010-a-b" "a@"
	@echo "둘"
	@-$(TARGET) -a "2015" "apple" "smart" "2" "baddr" "010-c-d" "b@"
	@echo "셋"
	@-$(TARGET) -a "2016" "banana" "hang" "3" "caddr" "010-e-f" "c@"
	@echo "넷"
	@-$(TARGET) -a "2017" "carrot" "manage" "4" "daddr" "010-g-h" "d@"
	@echo "다섯"
	@-$(TARGET) -a "2018" "diamond" "sound" "5" "eaddr" "010-i-j" "e@"
	@echo "여섯"
	@-$(TARGET) -a "2019" "elephant" "hyunghak" "6" "faddr" "010-k-l" "f@"
	@-hd student.idx
	@-hd student.dat
	@echo "test #23 - 검색 테스트"
	@-$(TARGET) -s "2014"
	@-$(TARGET) -s "2015"
	@-$(TARGET) -s "2016"
	@-$(TARGET) -s "2017"
	@-$(TARGET) -s "2018"
	@-$(TARGET) -s "2019"
endif
