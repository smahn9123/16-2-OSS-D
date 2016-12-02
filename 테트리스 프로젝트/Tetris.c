#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <conio.h>
#include <time.h>

#define BOARD_WIDTH 12	// 게임판 너비
#define BOARD_HEIGHT 20	// 게임판 높이

// 게임판 위치 보정값
#define BOARD_X	1
#define BOARD_Y 1

// 다음블록위치 좌표 
#define NEXT_X 18
#define NEXT_Y 16

// 블록 진행 속도
#define DELAY 1

// 게임판
int board[BOARD_HEIGHT + 2][BOARD_WIDTH + 2];

// 블록 정보
typedef struct {
	int type;
	int rotation;
	int curX;
	int curY;
}block;

// 입력키
enum ControlKeys
{
	UP = 72,
	DOWN = 80,
	LEFT = 75,
	RIGHT = 77,
	SPACE = 32
};

// 블록
int blocks[7][4][4][4] = {
	// ■■■■ - 1
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,
	0, 0, 0, 0,

	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,

	0, 0, 0, 0,
	0, 0, 0, 0,
	1, 1, 1, 1,
	0, 0, 0, 0,

	// ■■
	// ■■ - 2
	1, 1, 0, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 0, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 0, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 0, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	//   ■
	// ■■■ - 3
	0, 1, 0, 0,
	1, 1, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	0, 1, 0, 0,
	1, 1, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	0, 0, 0, 0,
	1, 1, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	0, 1, 0, 0,
	0, 1, 1, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	//     ■
	// ■■■ - 4
	0, 0, 1, 0,
	1, 1, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 0, 0,
	0, 1, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	1, 1, 1, 0,
	1, 0, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 0, 0, 0,
	1, 0, 0, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,

	// ■
	// ■■■ - 5
	1, 0, 0, 0,
	1, 1, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	0, 1, 0, 0,
	0, 1, 0, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,

	1, 1, 1, 0,
	0, 0, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 0, 0,
	1, 0, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 0,

	//   ■■
	// ■■   - 6
	0, 1, 1, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 0, 0, 0,
	1, 1, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	0, 1, 1, 0,
	1, 1, 0, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	1, 0, 0, 0,
	1, 1, 0, 0,
	0, 1, 0, 0,
	0, 0, 0, 0,

	// ■■
	//   ■■ - 7
	1, 1, 0, 0,
	0, 1, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	0, 1, 0, 0,
	1, 1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 0,

	1, 1, 0, 0,
	0, 1, 1, 0,
	0, 0, 0, 0,
	0, 0, 0, 0,

	0, 1, 0, 0,
	1, 1, 0, 0,
	1, 0, 0, 0,
	0, 0, 0, 0,
};

// 커서 이동 함수
void gotoxy(int x, int y)
{
	COORD Cur = { x, y };
	SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), Cur);
}

// 커서 위치 확인 함수
COORD getCursorPosition(void)
{
	CONSOLE_SCREEN_BUFFER_INFO cursorInfo;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursorInfo);
	COORD Cur = { cursorInfo.dwCursorPosition.X, cursorInfo.dwCursorPosition.Y };
	return Cur;
}

// 커서 제거 함수
void cursorHide() {
	CONSOLE_CURSOR_INFO CurInfo;
	CurInfo.bVisible = 0;
	CurInfo.dwSize = 1;
	SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &CurInfo);
};
void title(void) { //게임시작화면
	int x = 30; //타이틀화면이 표시되는 x좌표 
	int y = 1; //타이틀화면이 표시되는 y좌표 
	int cnt; //타이틀 프레임을 세는 변수  

	gotoxy(x, y + 0); printf("□□□□□□□□□□□□□□□"); Sleep(100);
	gotoxy(x, y + 1); printf("□□□□□□□□□□□□□□□"); Sleep(100);
	gotoxy(x, y + 2); printf("□□□□              □□□□"); Sleep(100);
	gotoxy(x, y + 3); printf("□□□□□□□□□□□□□□□"); Sleep(100);
	gotoxy(x, y + 4); printf("□□□□□□□□□□□□□□□"); Sleep(100);

	gotoxy(x + 9, y + 2); printf("T E T R I S"); Sleep(100);

	gotoxy(x, y + 8); printf("  △   : Shift");
	gotoxy(x, y + 9); printf("◁  ▷ : Left / Right");
	gotoxy(x, y + 10); printf("  ▽   : Soft Drop");
	gotoxy(x, y + 11); printf(" SPACE : Hard Drop");

	gotoxy(x, y + NEXT_X - 4); printf("    □□□□□□□");
	gotoxy(x, y + NEXT_X - 3); printf("    □          □");
	gotoxy(x, y + NEXT_X - 2); printf("    □          □");
	gotoxy(x, y + NEXT_X - 1); printf("    □          □");
	gotoxy(x, y + NEXT_X); printf("    □          □");
	gotoxy(x, y + NEXT_X + 1); printf("    □          □");
	gotoxy(x, y + NEXT_X + 2); printf("    □          □");
	gotoxy(x, y + NEXT_X + 3); printf("    □□□□□□□");


	while (kbhit()) getch(); //버퍼에 기록된 키값을 버림 

}

// 게임판 출력
void initBoard() {
	// 왼쪽, 오른쪽 벽
	for (int i = 0; i < BOARD_HEIGHT + 2; ++i) {
		board[i][0] = 1;
		board[i][BOARD_WIDTH + 1] = 1;
		gotoxy(2 * BOARD_X, BOARD_Y + i);
		printf("■");
		gotoxy(2 * (BOARD_X + BOARD_WIDTH + 1), BOARD_Y + i);
		printf("■");
	}
	// 위, 아래 벽
	for (int i = 0; i < BOARD_WIDTH + 2; ++i) {
		board[0][i] = 1;
		board[BOARD_HEIGHT + 1][i] = 1;
		gotoxy(2 * (BOARD_X + i), BOARD_Y);
		printf("■");
		gotoxy(2 * (BOARD_X + i), BOARD_Y + BOARD_HEIGHT + 1);
		printf("■");
	}
}

// 충돌 체크
int collisionCheck(block t) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (blocks[t.type][t.rotation][i][j] + board[t.curY + i][t.curX + j] == 2)
				return 1;
		}
	}
	return 0;
}

// 블록 그리기
void drawBlock(block t) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (blocks[t.type][t.rotation][i][j]) {
				gotoxy(2 * (BOARD_X + t.curX + j), BOARD_Y + t.curY + i);
				printf("■");
			}
		}
	}
}

// 블록 지우기 (다음 위치 이동 위해 현재 출력된 을 지우기)
void removeBlock(block t) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			if (blocks[t.type][t.rotation][i][j] && t.curY + i) {
				gotoxy(2 * (BOARD_X + t.curX + j), BOARD_Y + t.curY + i);
				printf("  ");
			}
		}
	}
}

// 채워진 줄 확인
void lineCheck() {
	int remap = 0;
	for (int i = BOARD_HEIGHT; i > 0; --i) {

		int clear = 1; // 해당 line이 full임을 나타내는 flag 변수
		for (int j = 1; j <= BOARD_WIDTH; ++j) {
			if (!board[i][j]) {
				clear = 0;
				break;
			}
		}

		if (clear) {
			remap = 1; // board를 재구성할 필요를 나타내는 flag 변수
			for (int j = 1; j <= BOARD_WIDTH; ++j) {
				for (int k = i; k > 1; --k) {
					board[k][j] = board[k - 1][j]; // 해당 line을 기준으로 위의 모든 line을 하나씩 끌어내림
				}
			}
			++i;
		}
	}
	/* 위의 결과를 콘솔창 출력에 반영 */
	if (remap) {
		for (int i = BOARD_HEIGHT; i > 0; --i) {
			gotoxy(2 * (BOARD_X + 1), BOARD_Y + i);
			for (int j = 1; j <= BOARD_WIDTH; ++j) {
				if (board[i][j])
					printf("■");
				else
					printf("  ");
			}
		}
	}
}

// 블록 안착시키기
void mergeBlock(block t) {
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			board[t.curY + i][t.curX + j] |= blocks[t.type][t.rotation][i][j];
		}
	}
	lineCheck();
}

// 게임 시작 (return 2 : 게임 오버)
void start() {

	srand((unsigned)time(NULL));

	block t;	// 이동중인 블록
	block next;
	block temp;
	block temp2;

	next.type = rand() % 7;
	next.rotation = 0;
	next.curX = NEXT_X;
	next.curY = NEXT_Y;
	while (1) {
		// 새 블록 생성
		t.type = next.type;
		t.rotation = 0;
		t.curX = BOARD_WIDTH / 2;
		t.curY = 0;
		drawBlock(t);

		// 다음블럭 생성
		removeBlock(next);
		next.type = rand() % 7;
		drawBlock(next);

		// 타이머 생성
		time_t timer;
		time_t curTime;
		time(&timer);

		while (1) {
			int merged = 0;

			time(&curTime);	// 시간 체크

			// 시간이 다 됐을 시
			if (curTime - timer >= DELAY) {
				timer = curTime;

				// 아래로 한 칸 이동
				temp = t;
				temp.curY++;

				// 부딪힌 경우
				if (collisionCheck(temp)) {
					if (t.curY <= 0) // -> 현재 block이 첫줄(0) 아래로 내려가는 상황에서 충돌 발생하면 Game Over
						return 2;
					mergeBlock(t);
					break;
				}
				removeBlock(t);
				t = temp;
				drawBlock(t);
			}

			// 키 입력 시
			if (_kbhit()) {
				int key = _getch();

				switch (key) {
				case LEFT:
					temp = t;
					temp.curX--;
					if (collisionCheck(temp))
						break;
					removeBlock(t);
					t = temp;
					drawBlock(t);
					break;
				case RIGHT:
					temp = t;
					temp.curX++;
					if (collisionCheck(temp))
						break;
					removeBlock(t);
					t = temp;
					drawBlock(t);
					break;
				case UP:
					temp = t;
					temp.rotation = (temp.rotation + 1) % 4;
					if (collisionCheck(temp))
						break;
					removeBlock(t);
					t = temp;
					drawBlock(t);
					break;
				case DOWN:
					temp = t;
					temp.curY++;
					if (collisionCheck(temp)) {
						if (t.curY <= 0)
							return 2;
						mergeBlock(t);
						merged = 1;
						break;
					}
					time(&timer);	// 타이머 초기화
					removeBlock(t);
					t = temp;
					drawBlock(t);
					break;
				case SPACE:
					temp = t;
					while (1){
						temp2 = temp;
						temp.curY++;
						if (collisionCheck(temp)) {
							/* Bug */
							//	if (t.curY <= 0)
							//		return 2;
							/*******/
							mergeBlock(temp2);
							merged = 1;
							break;
						}
					}
					time(&timer);	// Å¸ÀÌ¸Ó ÃÊ±âÈ­
					removeBlock(t);
					t = temp2;
					drawBlock(t);
					break;
				}

				// 블록 안착시 새 블록 생성
				if (merged)
					break;
			}
		}
	}
}

int main(void) {
	// 타이틀 출력
	title();

	// 커서 제거
	cursorHide();

	// 게임판 출력
	initBoard();

	// 게임 시작
	start();

	return 0;
}
