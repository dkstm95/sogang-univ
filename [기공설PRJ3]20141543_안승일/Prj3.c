/*
*****************************************************************************************
제목 : 지뢰찾기
기능 : 지뢰를 피하는 게임
파일 이름 : Prj3.c
수정 날짜 : 12 / 4
작성자 : 안승일
*****************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void setField(char [][10], int, int []);//지뢰가 설치된 밭을 만들어주는 함수이다.
void play_game(char [][10], int, int []);//게임을 실행하는 주요 함수이다.
void printField(char [][10], int);//공개된 좌표와 비공개된 좌표를 구분하여 지뢰밭을 출력하는 함수이다.
void openTile(char [][10], char [][10], int, int);//입력받은 좌표를 공개하는 함수이다.
void statistics(int [], int []);//통계를 내주는 함수이다.
int charToint(char);//좌표의 열을 입력 받을 때 문자를 숫자로 바꿔주는 함수이다.

int main(){
	char field[10][10]={0,};//지뢰밭을 설치할 배열이다.
	int num=0;//게임 시작, 통계, 종료의 번호를 저장할 변수이다.
	int level_num=0;//난이도의 번호를 저장할 변수이다.
	int total[3]={0,}, win[3]={0,};//통계를 낼 때 게임 횟수와 이긴 게임 횟수를 저장할 배열이다.
	int i, j;

	while(1){//3번 종료일 때까지 무한 반복을 한다.
		printf("-----------------------------------------------------------\n");
		printf("1. 게임 시작\n");
		printf("2. 통계\n");
		printf("3. 종료\n");
		scanf("%d", &num);//입력받은 숫자를 num에 저장한다.
		getchar();

		if(num == 3){
			printf("Good bye!!\n");
			break;
		}
		else if(num == 2) statistics(total, win);
		else if(num == 1){
			printf("난이도를 선택하세요.\n");
			printf("1. 상 <10*10, 30 mines>\n");
			printf("2. 중 <10*10, 20 mines>\n");
			printf("3. 하 <10*10, 10 mines>\n");
			scanf("%d", &level_num);//입력받은 숫자를 level_num에 저장한다.
			getchar();
			setField(field, level_num, total);//setField 함수를 호출하여 지뢰밭을 설치한 후, total 배열에 난이도에 따른 게임 횟수를 저장한다.
			play_game(field, level_num, win);//play_game 함수를 호출하여 본격적인 게임을 실행한다. win 배열에 난이도에 따른 이긴 게임 횟수를 저장한다.
		}

		for(i=0;i<10;i++){//한 게임이 끝난 후 field 배열을 다시 0으로 초기화한다.
			for(j=0;j<10;j++){
				field[i][j] = 0;
			}
		}
	}

	return 0;
}

void statistics(int total[], int win[]){
	float win_per[3] = {0,};//승률을 저장할 배열이다.
	int i;

	for(i=0;i<3;i++){//이긴 게임 횟수가 0이면 승률은 0으로 한다. 아닐 경우는 이긴 게임 횟수와 게임 횟수를 나누어 100을 곱한다.
		if(win[i] == 0) win_per[i] = 0;
		else win_per[i] = ((float)win[i] / (float)total[i]) * 100;
	}

	printf("난이도 상\n");
	printf("게임 횟수 : %d, 이긴 게임 횟수 : %d, 승률 : %.2f%%\n", total[0], win[0], win_per[0]);
	printf("난이도 중\n");
	printf("게임 횟수 : %d, 이긴 게임 횟수 : %d, 승률 : %.2f%%\n", total[1], win[1], win_per[1]);
	printf("난이도 하\n");
	printf("게임 횟수 : %d, 이긴 게임 횟수 : %d, 승률 : %.2f%%\n", total[2], win[2], win_per[2]);
}

int charToint(char col_temp){//col_temp 값에 따라 숫자를 리턴 해준다.
	if(col_temp == 'a') return 0;
	else if(col_temp == 'b') return 1;
	else if(col_temp == 'c') return 2;
	else if(col_temp == 'd') return 3;
	else if(col_temp == 'e') return 4;
	else if(col_temp == 'f') return 5;
	else if(col_temp == 'g') return 6;
	else if(col_temp == 'h') return 7;
	else if(col_temp == 'i') return 8;
	else if(col_temp == 'j') return 9;
}

void openTile(char temp[][10], char field[][10], int row, int col){
	int i, j;

	if(field[row][col] == '0'){//입력받은 좌표에 0이 들어 있는 경우 그 주변의 값들을 조사한다.
		for(i=row-1;i<=row+1;i++){
			for(j=col-1;j<=col+1;j++){
				if((i < 0) || (j < 0)) continue;
				else if((i > 9) || (j > 9)) continue;
				else if(field[i][j] == 'M') continue;
				else{
					if(field[i][j] == '0'){//주변의 값이 0인 경우, temp배열의 그 좌표에서의 값이 *이면 좌표를 열고 openTile 함수를 재귀적으로 부른다.
						if(temp[i][j] == '*'){
							temp[i][j] = field[i][j];
							openTile(temp, field, i, j);
						}
						else continue;//temp배열의 그 좌표에서의 값이 *가 아닌 경우 continue한다.
					}
					else temp[i][j] = field[i][j];//주변의 값이 0이 아닌 경우 좌표를 연다.
				}
			}
		}
	}
	else temp[row][col] = field[row][col];//입력받은 좌표에 0이 아닌 수가 있는 경우 좌표를 연다.
}

void setField(char field[][10], int level_num, int total[]){
	int random_row, random_col;//행과 열을 난수로 발생시켜 저장할 변수이다.
	int i, j, k, w;

	srand(time(NULL));

	if(level_num == 1){//넘겨받은 난이도에 따라 total 배열의 값을 1씩 증가시켜준다. 
		total[0]++;
		for(i=0;i<30;i++){//난이도에 따라 지뢰의 개수가 다르므로 for문의 조건식을 밑에서 계속 바꿔준다. 난수를 발생시켜 얻은 좌표값이 메인 함수에서 초기화한 NULL값이면 지뢰를 설치하고 아닐 경우 i를 1 감소시켜준다. 
			random_row = rand() % 10;
			random_col = rand() % 10;
			if(field[random_row][random_col] == '\0'){
				field[random_row][random_col] = 'M';
			}
			else{
				i--;
				continue;
			}
		}
	}

	else if(level_num == 2){
		total[1]++;
		for(i=0;i<20;i++){
			random_row = rand() % 10;
			random_col = rand() % 10;
			if(field[random_row][random_col] == '\0'){
				field[random_row][random_col] = 'M';
			}
			else{
				i--;
				continue;
			}
		}
	}

	else if(level_num == 3){
		total[2]++;
		for(i=0;i<10;i++){
			random_row = rand() % 10;
			random_col = rand() % 10;
			if(field[random_row][random_col] == '\0'){
				field[random_row][random_col] = 'M';
			}
			else{
				i--;
				continue;
			}
		}
	}

	for(i=0;i<10;i++){//지뢰가 설치되지 않은 좌표들을 정수처럼 쓰기 위해 '0'을 더해준다.
		for(j=0;j<10;j++){
			if(field[i][j] == 'M') continue;
			else field[i][j] += '0';
		}
	}

	for(i=0;i<10;i++){//지뢰가 설치되지 않은 좌표들 중 행과 열이 0~9인 좌표를 걸러내어 그 주변에 지뢰가 설치되어 있으면 그 좌표의 값을 1씩 증가시킨다.
		for(j=0;j<10;j++){
			if(field[i][j] == 'M') continue;
			for(k=i-1;k<=i+1;k++){
				for(w=j-1;w<=j+1;w++){
					if((k < 0) || (w < 0)) continue;
					else if((k > 9) || (w > 9)) continue;
					else{
						if((k == i) && (w == j)) continue;
						else if(field[k][w] == 'M') field[i][j] = field[i][j] + 1;
					}
				}
			}
		}
	}
}

void play_game(char field[][10], int level_num, int win[]){
	char temp[10][10] = {0,};//*로 감춰진 지뢰밭을 저장할 배열이다. 
	int i, j;
	int row=0, col=0;//입력받은 좌표를 저장할 변수이다.
	int mine_num=0, check=0;//난이도에 따른 지뢰 갯수를 저장할 변수와 게임 성공을 위한 변수이다.
	int temp_num=0, field_num=1;//완전 공개된 지뢰밭과 부분 공개된 지뢰밭을 구분하여 printField 함수에 넘겨주기 위한 변수이다.
	char col_temp=0;//입력받은 좌표 중 열은 문자이므로 캐릭터형으로 받을 변수이다.

	if(level_num == 1) mine_num = 30;//난이도에 따라 지뢰 갯수를 저장해준다.
	else if(level_num == 2) mine_num = 20;
	else if(level_num == 3) mine_num = 10;

	for(i=0;i<10;i++){//temp 배열을 *로 초기화해준다.
		for(j=0;j<10;j++){
			temp[i][j] = '*';
		}
	}

	printField(field, field_num);//게임 처음에 완전 공개된 지뢰밭를 출력한 후, *로 감춰진 좌표를 출력하기 위해 printField 함수를 호출한다.
	printField(temp, temp_num);

	while(1){//입력받은 좌표값이 지뢰인 경우나 게임이 성공한 경우 무한 반복을 탈출한다.
		for(i=0;i<10;i++){//temp 배열의 값이 *인 경우 check를 1씩 증가시킨다.
			for(j=0;j<10;j++){
				if(temp[i][j] == '*') check++;
			}
		}

		if(check == mine_num){//check와 지뢰 개수가 같은 경우 "성공!!"을 출력 후 완전 공개된 지뢰밭을 출력 후 난이도에 따라 win 배열의 값을 1 증가시키고 while문을 탈출한다.
			printf("성공!!\n");
			printField(field, field_num);
			if(level_num == 1) win[0]++;
			else if(level_num == 2) win[1]++;
			else if(level_num == 3) win[2]++;
			break;
		}
		else{
			printf("좌표 <ex : 1 a> : ");
			scanf("%d %c", &row, &col_temp);//입력받은 좌표를 row와 col_temp에 저장한다.
			getchar();
			col = charToint(col_temp);//charToint 함수를 호출하여 입력받은 좌표의 열의 문자에 따라 숫자로 바꾸어 리턴받은 것을 col에 저장한다.

			if(field[row][col] == 'M'){//입력받은 좌표의 값이 지뢰인 경우 "펑!!"과 완전 공개된 지뢰밭을 출력후 while 문을 탈출한다.
				printf("펑!!\n");
				printField(field, field_num);
				break;
			}
			else{//입력받은 좌표의 값이 숫자일 경우 openTile 함수를 호출하고 부분 공개된 지뢰밭을 출력한다.
				openTile(temp, field, row, col);
				printField(temp, temp_num);
			}
			check = 0;//다음 좌표 받을 때 성공 여부를 확인하기 위하여 check를 0으로 초기화한다.
		}
		check = 0;
	}
}

void printField(char arr[][10], int temporary){
	int i, j;
	int temp=0;

	printf("   a b c d e f g h i j\n");
	printf("  --------------------\n");
	if(temporary == 1){//넘겨받은 temporary 값이 1, 즉 완전 공개된 지뢰밭일 경우, 지뢰밭을 출력한다.
		for(i=0;i<10;i++){
			printf("%d| ", temp);
			for(j=0;j<10;j++){
				printf("%c ", arr[i][j]);
				if(j == 9) printf("\n");
			}
			temp++;
		}
	}
	else{//넘겨받은 temporary 값이 0, 즉 부분 공개된 지뢰밭일 경우, 지뢰밭을 출력한다.
		for(i=0;i<10;i++){
			printf("%d| ", temp);
			for(j=0;j<10;j++){
				printf("%c ", arr[i][j]);
				if(j == 9) printf("\n");
			}
			temp++;
		}
	}
}