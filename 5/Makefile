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
	@-hd student.idx
	@-hd student.dat
	@echo "---------------------------------------------------------------------------------------------------------------------"
	@echo "test #2 - 데이터 한 개 삭제"
	@-$(TARGET) -d "20142455"
	@-hd student.idx
	@-hd student.dat
	@echo "---------------------------------------------------------------------------------------------------------------------"
	@echo "test #3 - 좀 더 짧은 데이터 추가"
	@-$(TARGET) -a "20142303" "shk.im" "Computer Science" "3" "hongcheon, Gangwon-do" "010-xxxx-yyyy" "niceb5y@gmail.com"
	@-hd student.idx
	@-hd student.dat
	@echo "test #4 - 검색"
	@-$(TARGET) -s "20142303"
	@echo "test #5 - 데이터 3 개 추가"
	@-$(TARGET) -a "20150001" "Shinyoung-Tae" "computer law" "9" "heaven" "010-1111-zzzz" "god@tae.com"
	@-$(TARGET) -a "20150002" "Nisio-isin" "Author" "0" "hell" "017-1199-2288" "ishin@japan.jp"
	@-$(TARGET) -a "20150003" "honobu-yonezawa" "bing-gwa" "5" "yo-japan" "817-1111-zzxx" "yonezawa@honobu.com"
	@-hd student.idx
	@-hd student.dat
	@echo "test #6 - 검색"
	@-$(TARGET) -s "20142303"
	@-$(TARGET) -s "20150001"
	@-$(TARGET) -s "20150002"
	@-$(TARGET) -s "20150003"
	@echo "test #7 - 중간 2개 삭제"
	@-$(TARGET) -d "20150001"
	@-hd student.idx
	@-hd student.dat
	@-$(TARGET) -d "20150002"
	@-hd student.idx
	@-hd student.dat
	@echo "test #7 - 검색"
	@-$(TARGET) -s "20142303"
	@-$(TARGET) -s "20150001"
	@-$(TARGET) -s "20150002"
	@-$(TARGET) -s "20150003"
	@echo "test #8 - 삽입(삭제된 위치 재활용)"
	$(TARGET) -a "20150004" "newface" "new" "0" "hav" "010-123" "short@email" # 삽입했는데 헤더가 -1이 되는 버그가 있음 삽입하면 헤더가 기존에 이 포지션을 가져야함
	@-hd student.idx
	@-hd student.dat
	$(TARGET) -a "20150005" "newface2" "new2" "2" "haz" "110-123" "short@em" 
	@-hd student.idx
	@-hd student.dat
	@echo "test #9 - 검색"
	@-$(TARGET) -s "20142303"
	@-$(TARGET) -s "20150001"
	@-$(TARGET) -s "20150002"
	@-$(TARGET) -s "20150003"
	@-$(TARGET) -s "20150004"
	@-$(TARGET) -s "20150005"
