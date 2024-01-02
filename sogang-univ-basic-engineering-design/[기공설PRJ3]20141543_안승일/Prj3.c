/*
*****************************************************************************************
���� : ����ã��
��� : ���ڸ� ���ϴ� ����
���� �̸� : Prj3.c
���� ��¥ : 12 / 4
�ۼ��� : �Ƚ���
*****************************************************************************************
*/
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void setField(char [][10], int, int []);//���ڰ� ��ġ�� ���� ������ִ� �Լ��̴�.
void play_game(char [][10], int, int []);//������ �����ϴ� �ֿ� �Լ��̴�.
void printField(char [][10], int);//������ ��ǥ�� ������� ��ǥ�� �����Ͽ� ���ڹ��� ����ϴ� �Լ��̴�.
void openTile(char [][10], char [][10], int, int);//�Է¹��� ��ǥ�� �����ϴ� �Լ��̴�.
void statistics(int [], int []);//��踦 ���ִ� �Լ��̴�.
int charToint(char);//��ǥ�� ���� �Է� ���� �� ���ڸ� ���ڷ� �ٲ��ִ� �Լ��̴�.

int main(){
	char field[10][10]={0,};//���ڹ��� ��ġ�� �迭�̴�.
	int num=0;//���� ����, ���, ������ ��ȣ�� ������ �����̴�.
	int level_num=0;//���̵��� ��ȣ�� ������ �����̴�.
	int total[3]={0,}, win[3]={0,};//��踦 �� �� ���� Ƚ���� �̱� ���� Ƚ���� ������ �迭�̴�.
	int i, j;

	while(1){//3�� ������ ������ ���� �ݺ��� �Ѵ�.
		printf("-----------------------------------------------------------\n");
		printf("1. ���� ����\n");
		printf("2. ���\n");
		printf("3. ����\n");
		scanf("%d", &num);//�Է¹��� ���ڸ� num�� �����Ѵ�.
		getchar();

		if(num == 3){
			printf("Good bye!!\n");
			break;
		}
		else if(num == 2) statistics(total, win);
		else if(num == 1){
			printf("���̵��� �����ϼ���.\n");
			printf("1. �� <10*10, 30 mines>\n");
			printf("2. �� <10*10, 20 mines>\n");
			printf("3. �� <10*10, 10 mines>\n");
			scanf("%d", &level_num);//�Է¹��� ���ڸ� level_num�� �����Ѵ�.
			getchar();
			setField(field, level_num, total);//setField �Լ��� ȣ���Ͽ� ���ڹ��� ��ġ�� ��, total �迭�� ���̵��� ���� ���� Ƚ���� �����Ѵ�.
			play_game(field, level_num, win);//play_game �Լ��� ȣ���Ͽ� �������� ������ �����Ѵ�. win �迭�� ���̵��� ���� �̱� ���� Ƚ���� �����Ѵ�.
		}

		for(i=0;i<10;i++){//�� ������ ���� �� field �迭�� �ٽ� 0���� �ʱ�ȭ�Ѵ�.
			for(j=0;j<10;j++){
				field[i][j] = 0;
			}
		}
	}

	return 0;
}

void statistics(int total[], int win[]){
	float win_per[3] = {0,};//�·��� ������ �迭�̴�.
	int i;

	for(i=0;i<3;i++){//�̱� ���� Ƚ���� 0�̸� �·��� 0���� �Ѵ�. �ƴ� ���� �̱� ���� Ƚ���� ���� Ƚ���� ������ 100�� ���Ѵ�.
		if(win[i] == 0) win_per[i] = 0;
		else win_per[i] = ((float)win[i] / (float)total[i]) * 100;
	}

	printf("���̵� ��\n");
	printf("���� Ƚ�� : %d, �̱� ���� Ƚ�� : %d, �·� : %.2f%%\n", total[0], win[0], win_per[0]);
	printf("���̵� ��\n");
	printf("���� Ƚ�� : %d, �̱� ���� Ƚ�� : %d, �·� : %.2f%%\n", total[1], win[1], win_per[1]);
	printf("���̵� ��\n");
	printf("���� Ƚ�� : %d, �̱� ���� Ƚ�� : %d, �·� : %.2f%%\n", total[2], win[2], win_per[2]);
}

int charToint(char col_temp){//col_temp ���� ���� ���ڸ� ���� ���ش�.
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

	if(field[row][col] == '0'){//�Է¹��� ��ǥ�� 0�� ��� �ִ� ��� �� �ֺ��� ������ �����Ѵ�.
		for(i=row-1;i<=row+1;i++){
			for(j=col-1;j<=col+1;j++){
				if((i < 0) || (j < 0)) continue;
				else if((i > 9) || (j > 9)) continue;
				else if(field[i][j] == 'M') continue;
				else{
					if(field[i][j] == '0'){//�ֺ��� ���� 0�� ���, temp�迭�� �� ��ǥ������ ���� *�̸� ��ǥ�� ���� openTile �Լ��� ��������� �θ���.
						if(temp[i][j] == '*'){
							temp[i][j] = field[i][j];
							openTile(temp, field, i, j);
						}
						else continue;//temp�迭�� �� ��ǥ������ ���� *�� �ƴ� ��� continue�Ѵ�.
					}
					else temp[i][j] = field[i][j];//�ֺ��� ���� 0�� �ƴ� ��� ��ǥ�� ����.
				}
			}
		}
	}
	else temp[row][col] = field[row][col];//�Է¹��� ��ǥ�� 0�� �ƴ� ���� �ִ� ��� ��ǥ�� ����.
}

void setField(char field[][10], int level_num, int total[]){
	int random_row, random_col;//��� ���� ������ �߻����� ������ �����̴�.
	int i, j, k, w;

	srand(time(NULL));

	if(level_num == 1){//�Ѱܹ��� ���̵��� ���� total �迭�� ���� 1�� ���������ش�. 
		total[0]++;
		for(i=0;i<30;i++){//���̵��� ���� ������ ������ �ٸ��Ƿ� for���� ���ǽ��� �ؿ��� ��� �ٲ��ش�. ������ �߻����� ���� ��ǥ���� ���� �Լ����� �ʱ�ȭ�� NULL���̸� ���ڸ� ��ġ�ϰ� �ƴ� ��� i�� 1 ���ҽ����ش�. 
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

	for(i=0;i<10;i++){//���ڰ� ��ġ���� ���� ��ǥ���� ����ó�� ���� ���� '0'�� �����ش�.
		for(j=0;j<10;j++){
			if(field[i][j] == 'M') continue;
			else field[i][j] += '0';
		}
	}

	for(i=0;i<10;i++){//���ڰ� ��ġ���� ���� ��ǥ�� �� ��� ���� 0~9�� ��ǥ�� �ɷ����� �� �ֺ��� ���ڰ� ��ġ�Ǿ� ������ �� ��ǥ�� ���� 1�� ������Ų��.
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
	char temp[10][10] = {0,};//*�� ������ ���ڹ��� ������ �迭�̴�. 
	int i, j;
	int row=0, col=0;//�Է¹��� ��ǥ�� ������ �����̴�.
	int mine_num=0, check=0;//���̵��� ���� ���� ������ ������ ������ ���� ������ ���� �����̴�.
	int temp_num=0, field_num=1;//���� ������ ���ڹ�� �κ� ������ ���ڹ��� �����Ͽ� printField �Լ��� �Ѱ��ֱ� ���� �����̴�.
	char col_temp=0;//�Է¹��� ��ǥ �� ���� �����̹Ƿ� ĳ���������� ���� �����̴�.

	if(level_num == 1) mine_num = 30;//���̵��� ���� ���� ������ �������ش�.
	else if(level_num == 2) mine_num = 20;
	else if(level_num == 3) mine_num = 10;

	for(i=0;i<10;i++){//temp �迭�� *�� �ʱ�ȭ���ش�.
		for(j=0;j<10;j++){
			temp[i][j] = '*';
		}
	}

	printField(field, field_num);//���� ó���� ���� ������ ���ڹ縦 ����� ��, *�� ������ ��ǥ�� ����ϱ� ���� printField �Լ��� ȣ���Ѵ�.
	printField(temp, temp_num);

	while(1){//�Է¹��� ��ǥ���� ������ ��쳪 ������ ������ ��� ���� �ݺ��� Ż���Ѵ�.
		for(i=0;i<10;i++){//temp �迭�� ���� *�� ��� check�� 1�� ������Ų��.
			for(j=0;j<10;j++){
				if(temp[i][j] == '*') check++;
			}
		}

		if(check == mine_num){//check�� ���� ������ ���� ��� "����!!"�� ��� �� ���� ������ ���ڹ��� ��� �� ���̵��� ���� win �迭�� ���� 1 ������Ű�� while���� Ż���Ѵ�.
			printf("����!!\n");
			printField(field, field_num);
			if(level_num == 1) win[0]++;
			else if(level_num == 2) win[1]++;
			else if(level_num == 3) win[2]++;
			break;
		}
		else{
			printf("��ǥ <ex : 1 a> : ");
			scanf("%d %c", &row, &col_temp);//�Է¹��� ��ǥ�� row�� col_temp�� �����Ѵ�.
			getchar();
			col = charToint(col_temp);//charToint �Լ��� ȣ���Ͽ� �Է¹��� ��ǥ�� ���� ���ڿ� ���� ���ڷ� �ٲپ� ���Ϲ��� ���� col�� �����Ѵ�.

			if(field[row][col] == 'M'){//�Է¹��� ��ǥ�� ���� ������ ��� "��!!"�� ���� ������ ���ڹ��� ����� while ���� Ż���Ѵ�.
				printf("��!!\n");
				printField(field, field_num);
				break;
			}
			else{//�Է¹��� ��ǥ�� ���� ������ ��� openTile �Լ��� ȣ���ϰ� �κ� ������ ���ڹ��� ����Ѵ�.
				openTile(temp, field, row, col);
				printField(temp, temp_num);
			}
			check = 0;//���� ��ǥ ���� �� ���� ���θ� Ȯ���ϱ� ���Ͽ� check�� 0���� �ʱ�ȭ�Ѵ�.
		}
		check = 0;
	}
}

void printField(char arr[][10], int temporary){
	int i, j;
	int temp=0;

	printf("   a b c d e f g h i j\n");
	printf("  --------------------\n");
	if(temporary == 1){//�Ѱܹ��� temporary ���� 1, �� ���� ������ ���ڹ��� ���, ���ڹ��� ����Ѵ�.
		for(i=0;i<10;i++){
			printf("%d| ", temp);
			for(j=0;j<10;j++){
				printf("%c ", arr[i][j]);
				if(j == 9) printf("\n");
			}
			temp++;
		}
	}
	else{//�Ѱܹ��� temporary ���� 0, �� �κ� ������ ���ڹ��� ���, ���ڹ��� ����Ѵ�.
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