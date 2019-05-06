// 주의 사항
// 1. 이 파일에 새로운 상수 변수를 추가 하지 마시오.
// 2. 이 파일에 정의 되어 있는 상수 변수의 이름을 변경하지 마시오.
// 3. 이 파일에서 수정할 수 있는 것은 PAGES_PER_BLOCK와 BLOCKS_PER_DEVICE의
//    상수값 뿐이며, 다른 것은 어떤 것도 수정하지 마시오.
// 1, 2, 3을 지키지 않으면 채점 시 컴파일이 제대로 안됩니다.

#ifndef	_BLOCK_MAPPING_H_
#define	_BLOCK_MAPPING_H_

#define TRUE				1
#define	FALSE				0

#define	SECTOR_SIZE			512			
#define	SPARE_SIZE			16			
#define	PAGE_SIZE			(SECTOR_SIZE+SPARE_SIZE)
#define SECTORS_PER_PAGE	1
#define	PAGES_PER_BLOCK		2  // 상수값 수정 가능
#define	BLOCK_SIZE			(PAGE_SIZE*PAGES_PER_BLOCK)
#define	BLOCKS_PER_DEVICE	3 // 상수값 수정 가능
#define DATABLKS_PER_DEVICE	(BLOCKS_PER_DEVICE - 1)	// 한 개의 free block을 유지

#endif
