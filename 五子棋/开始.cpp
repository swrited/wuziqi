#undef UNICODE
#undef _UNICODE
#include <stdio.h>
#include <WinSock2.h>
#include <WS2tcpip.h>
#pragma comment(lib,"ws2_32.lib")
#include <windows.h>
#include<mmsystem.h>
#include<graphics.h>
#pragma comment(lib,"Winmm.lib")

IMAGE bgp;
MOUSEMSG m;

int n_x = 0, n_y = 0;	//当前的坐标
int piece[17][17] = { 0 };
int is_land = -1;
int position = 1;
int over = 0;
int step = 0;
int flag = 0;

char RecvBuffer[256];
char buff[256];

struct chess_piece
{
	int color;
	int x;
	int y;
	int step;
}qi[225];

void draw_line()
{
	setlinecolor(BLACK);
	for (int x = 20; x < 600; x += 40)
		line(x, 20, x, 580);
	for (int y = 20; y < 600; y += 40)
		line(20, y, 580, y);
}

void draw_point()
{
	setfillcolor(BLACK);
	fillcircle(4 * 40 - 20, 4 * 40 - 20, 3);
	fillcircle(12 * 40 - 20, 4 * 40 - 20, 3);
	fillcircle(4 * 40 - 20, 12 * 40 - 20, 3);
	fillcircle(12 * 40 - 20, 12 * 40 - 20, 3);
	fillcircle(8 * 40 - 20, 8 * 40 - 20, 3);
}

void init_back()
{
	loadimage(&bgp, _T("background.jpg"));
	putimage(0, 0, 800, 630, &bgp, 0, 0);
	draw_line();
	draw_point();
}

void init_piece()
{
	for (int i = 0; i < 16; i++)
		for (int j = 0; j < 16; j++)
			piece[i][j] = 0;
}

void init_struct_piece()
{
	for (int i = 0; i < 225; i++)
	{
		qi[i].color = 0;
		qi[i].x = 0;
		qi[i].y = 0;
	}
}
int check_five_piece(int x, int y)
{
	int temp = 0;
	int i = 0, j = 0;
	for (i = x - 3; i <= x + 4; i++)
	{
		if (piece[i][y] == piece[i - 1][y] && piece[i][y] != 0)
		{
			temp++;
		}
		else
			temp = 0;
		if (temp == 4)
			return 1;
	}
	temp = 0;
	for (j = y - 3; j <= y + 4; j++)
	{
		if (piece[x][j] == piece[x][j - 1] && piece[x][j] != 0)
		{
			temp++;
		}
		else
			temp = 0;
		if (temp == 4)
			return 1;
	}
	temp = 0;
	for (i = x - 3, j = y - 3; i <= x + 4 && j <= y + 4; i++, j++)
	{
		if (piece[i][j] == piece[i - 1][j - 1] && piece[i][j] != 0)
		{
			temp++;
		}
		else
			temp = 0;
		if (temp == 4)
			return 1;
	}
	temp = 0;
	for (i = x - 3, j = y + 3; i <= x + 4 && j >= y - 4; i++, j--)
	{
		if (piece[i][j] == piece[i - 1][j + 1] && piece[i][j] != 0)
		{
			temp++;
		}
		else
			temp = 0;
		if (temp == 4)
			return 1;
	}
	return 0;
}

void play_again()
{
	position = 1;
	init_back();
	init_struct_piece();
	init_piece();
	over = 0;
	step = 0;
}



int draw_piece_single(int x, int y)
{
	if (position == 1)
		setfillcolor(BLACK);
	else if (position == -1)
		setfillcolor(WHITE);

	if (piece[x][y] == 0 && (x>0&&x < 16 &&y>0&& y < 16))
	{
		fillcircle(x * 40 - 20, y * 40 - 20, 17);
		
		if (position == 1)
		{
			piece[x][y] = 1;
			qi[step].color = 1;
			qi[step].x = x;
			qi[step].y = y;
		}
			
		else
		{
			piece[x][y] = -1;
			qi[step].color = -1;
			qi[step].x = x;
			qi[step].y = y;
			
		}
		if(flag!=2)
			position *= -1;
		return 1;
	}
	return 0;
}

int score(int x, int y)
{
	int player_chess = 0;
	int robot_chess = 0;
	int empty_chess = 1;
	int empty = 0;
	int chess= 0;
	int r = 0;
	int l = 0;
	int i = 0;
	int score=0;
	int chance = 0;

	for (int yi = -1; yi <= 0; yi++)
	{
		for (int xi = -1; xi <= 1; xi++)
		{
			if (yi == 0 && xi != 1)
				continue;
			for (int i = 1; i <= 5; i++)
			{
				r = x + xi * i;
				l = y + yi * i;
				if (piece[r][l] == -position)
					player_chess++;
				else if (r<1||r > 15 ||l<1|| l > 15)
					break;
				else if (piece[r][l] == 0)
				{
					empty_chess++;
					break;
				}
				else
					break;

			}
				
			switch (player_chess)
			{
			case 1:
				score += 10; 
				break;
			case 2:
				if (empty_chess == 1)
					score += 20;
				else if (empty_chess == 2)
				{
					score += 30;
					chance++;


				}
					
				break;
			case 3:
				if (empty_chess == 1)
					score += 40;
				else if (empty_chess == 2)
					score += 2000;
				chance++;
				break;
			case 4:
				score += 20000; 

				break;
			}
			chess += player_chess;
			empty += empty_chess;
			player_chess = 0;
			robot_chess = 0;
			empty_chess = 1;
			for (int i = 1; i <= 5; i++)
			{
				r = x - xi * i;
				l = y - yi * i;
				if (piece[r][l] == -position)
					player_chess++;
				else if (r < 1 || r > 15 || l < 1 || l > 15)
					break;
				else if (piece[r][l] == 0)
				{
					empty_chess++;
					break;
				}
				else
					break;

			}

			switch (player_chess)
			{
			case 1:
				score += 10;
				break;
			case 2:
				if (empty_chess == 1)
					score += 20;
				else if (empty_chess == 2)
				{
					score += 30;
					chance++;
				}
					
				break;
			case 3:
				if (empty_chess == 1)
					score += 40;
				else if (empty_chess == 2)
					score += 2000;
				chance++;
				break;
			case 4:
				score += 20000;

				break;
			}
			chess += player_chess;
			empty += empty_chess;
			empty = empty - 1;
			 player_chess = 0;
			 robot_chess = 0;
			 empty_chess = 1;

			 if (empty == 3 && chess == 2) score += 30;
			 else if (empty == 3 && chess == 3) score += 2000;
			 else if (empty == 2 && chess == 2) score += 20;
			 else if (empty == 2 && chess == 3) score += 40;
			 else if (empty != 0 && chess == 4) score += 20000;

			 if (chance >= 2) score += 2000;
			 chance = 0;
			 chess = 0;
			 empty = 0;
			 //攻击
			 for (int i = 1; i <= 5; i++)
			 {


				 r = x + xi * i;
				 l = y + yi * i;
				 if (piece[r][l] == position)
					 robot_chess ++;
				 else if (r < 1 || r > 15 || l < 1 || l > 15)
					 break;
				 else if (piece[r][l] == 0)
				 {
					 empty_chess++;
					 break;
				 }
				 else
					 break;

			 }

			 switch (robot_chess)
			 {
			 case 1:
				 score += 10;
				 break;
			 case 2:
				 if (empty_chess == 1)
					 score += 20;
				 else if (empty_chess == 2)
				 {
					score += 30;
					chance++;
				 }
				 break;
			 case 3:
				 if (empty_chess == 1)
					 score += 50;
				 else if (empty_chess == 2)
					 score += 15000;
				 chance++;
				 break;
			 case 4:
				 score += 100000;

				 break;
			 }
			 chess += player_chess;
			 empty += empty_chess;
			 player_chess = 0;
			 robot_chess = 0;
			 empty_chess = 1;
			 for (int i = 1; i <= 5; i++)
			 {


				 r = x - xi * i;
				 l = y - yi * i;
				 if (piece[r][l] == position)
					 robot_chess++;
				 else if (r < 1 || r > 15 || l < 1 || l > 15)
					 break;
				 else if (piece[r][l] == 0)
				 {
					 empty_chess++;
					 break;
				 }
				 else
					 break;

			 }

			 switch (robot_chess)
			 {
			 case 1:
				 score += 10;
				 break;
			 case 2:
				 if (empty_chess == 1)
					 score += 20;
				 else if (empty_chess == 2)
				 {
					 score += 30;
					 chance++;
				 }
					
				 break;
			 case 3:
				 if (empty_chess == 1)
					 score += 50;
				 else if (empty_chess == 2)
					 score += 15000;
				 chance++;
				 break;
			 case 4:
				 score += 100000;

				 break;
			 }
			 chess += player_chess;
			 empty += empty_chess;
			 empty = empty - 1;
			 player_chess = 0;
			 robot_chess = 0;
			 empty_chess = 1;
			 if (empty == 3 && chess == 2) score += 30;
			 else if (empty == 3 && chess == 3) score += 15000;
			 else if (empty == 2 && chess == 2) score += 20;
			 else if (empty == 2 && chess == 3) score += 1500;
			 else if (empty != 0 && chess == 4) score += 100000;

			 if (chance >= 2) score += 15000;
			 chance = 0;
			 chess = 0;
			 empty = 0;
			 
		}

	}
		
	return score;
}

int draw_piece_robot()
{
	int sc = 0;
	int te_x = 0, te_y = 0;
	for (int i = 1; i < 16; i++)
		for (int j = 1; j < 16; j++)
		{
			if (piece[i][j] == 0)
			{
				if (score(i, j) > sc)
				{
					sc = score(i, j);
					te_x = i;
					te_y = j;
				}	
			}
		}
	Sleep(100);
	if (step == 0) te_x = 7, te_y = 7;
	if (position == -1)
	{
		setfillcolor(WHITE);
		piece[te_x][te_y] = -1;
		qi[step].color = -1;
		position = 1;
	}
	else if (position == 1)
	{
		setfillcolor(BLACK);
		piece[te_x][te_y] = 1;
		qi[step].color = 1;
		position = -1;
	}
		
	fillcircle(te_x * 40 - 20, te_y * 40 - 20, 17);
	
	qi[step].x = te_x;
	qi[step].y = te_y;
	n_x = te_x;
	n_y = te_y;
	return 1;
}

int draw_front(int i)
{
	init_back();
	i = i - 1;
	for (int j = 0; j < i; j++)
	{
		if (qi[j].color == 1)
			setfillcolor(BLACK);
		else if (qi[j].color == -1)
			setfillcolor(WHITE);
		else return 0;
		fillcircle(qi[j].x * 40 - 20, qi[j].y * 40 - 20, 17);

	}
	piece[qi[i].x][qi[i].y] = 0;
	qi[i].color = 0;
	qi[i].x = 0;
	qi[i].y = 0;
	position = -position;
	
	return 1;

}

void draw_front_piece(int x, int y)
{
	if (position == 1)
		setfillcolor(WHITE);
	else if (position == -1)
		setfillcolor(BLACK);

	if (piece[x][y] == 0 && (x > 0 && x < 16 && y>0 && y < 16))
	{
		fillcircle(x * 40 - 20, y * 40 - 20, 17);

		if (position == 1)
		{
			piece[x][y] = -1;
			qi[step].color = -1;
			qi[step].x = x;
			qi[step].y = y;

		}

		else
		{
			piece[x][y] = 1;
			qi[step].color = 1;
			qi[step].x = x;
			qi[step].y = y;
		}

	}

}

int memory(int i)
{
	
	init_back();
	for (int j = 0; j < i; j++)
	{
		if (qi[j].color == 1)
			setfillcolor(BLACK);
		else if (qi[j].color == -1)
			setfillcolor(WHITE);
		else return 0;
		fillcircle(qi[j].x * 40 - 20, qi[j].y * 40 - 20, 17);
		
		Sleep(500);

	}
	flushmessage(EX_MOUSE);
	if (over == 1)
	{
		settextcolor(RED);
		settextstyle(80, 0, _T("楷体"));
		outtextxy(150, 250, _T("游戏结束"));
		if (position == 1) outtextxy(150, 350, _T("白子获胜"));
		else if (position == -1) outtextxy(150, 350, _T("黑子获胜"));
	}

	return 1;


}

void menu()
{
	loadimage(&bgp, _T("back.jpg"));
	putimage(0, 0, 800, 630, &bgp, 0, 0);
	settextcolor(RED);
	settextstyle(50, 0, _T("楷体"));
	setbkmode(TRANSPARENT);
	outtextxy(100, 150, _T("单人模式"));
	outtextxy(100, 250, _T("多人模式"));
	outtextxy(100, 350, _T("人机模式"));
	settextcolor(RED);
	settextstyle(20, 0, _T("楷体"));
	
}

void tools()
{
	settextcolor(RED);
	settextstyle(30, 15, _T("黑体"));
	setbkmode(TRANSPARENT);
	outtextxy(630, 100, _T("提示"));
	outtextxy(630, 200, _T("悔棋"));
	outtextxy(630, 300, _T("重新开始"));
	outtextxy(630, 500, _T("回放"));
	outtextxy(630, 400, _T("返回菜单"));


}

void click()
{
	if (m.uMsg == WM_LBUTTONDOWN && over == 0)
	{
		n_x = m.x / 40 + 1;
		n_y = m.y / 40 + 1;

		if (draw_piece_single(n_x, n_y))//下棋
		{
			step++;
		}
		else if (m.x > 620 && m.x < 800 && m.y < 200 && m.y>100)//提示
		{
			draw_piece_robot();
			step++;
		}
		else if (m.x > 620 && m.x < 800 && m.y < 300 && m.y>200)//悔棋
		{
			if (step > 0)
			{
				draw_front(step);
				step--;
				if (flag == 3)
				{
					draw_front(step);
					step--;
				}
			}
		}
		else if (m.x > 620 && m.x < 800 && m.y < 400 && m.y>300)//重新开始
		{
			play_again();
		}
		else if (m.x > 620 && m.x < 800 && m.y < 500 && m.y>400)//返回菜单
		{
			init_struct_piece();
			init_piece();
			menu();
			flag = 0;
		}
		else if (m.x > 620 && m.x < 800 && m.y < 600 && m.y>500)//回放
		{
			memory(step);
		}
	}


}
void over_click()
{

	settextcolor(RED);
	settextstyle(80, 0, _T("楷体"));
	outtextxy(150, 250, _T("游戏结束"));
	if (position == 1) outtextxy(150, 350, _T("白子获胜"));
	else if (position == -1) outtextxy(150, 350, _T("黑子获胜"));

	if (m.uMsg == WM_LBUTTONDOWN)
	{
		if (m.x > 620 && m.x < 800 && m.y < 400 && m.y>300)
		{
			play_again();
		}
		else if (m.x > 620 && m.x < 800 && m.y < 500 && m.y>400)
		{
			init_struct_piece();
			init_piece();
			menu();
			flag = 0;
		}
		else if (m.x > 620 && m.x < 800 && m.y < 600 && m.y>500)
		{
			memory(step);
		}
	}

}
void chess_single()
{
	position = 1;
	step = 0;
	over = 0;
	initgraph(800, 630);
	init_back();
	while (1)
	{
		tools();
		m = GetMouseMsg();
		click();

		over = check_five_piece(n_x,n_y);
		
		if (over == 1)
		{	
			over_click();
		}
		if (flag == 0)
			break;
	}
}
int chess_double()
{

	int temp_x;
	int temp_y;
	over = 0;
	//1.确定协议版本
	WSADATA wsaData;
	WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (LOBYTE(wsaData.wVersion) != 2 ||
		HIBYTE(wsaData.wVersion) != 2)
	{
		printf("确定协议失败！\n");
		return -1;
	}
	printf("确定协议成功！\n");

	//2.创建socket
	SOCKET clientSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (SOCKET_ERROR == clientSocket)
	{
		printf("创建SOCKET失败：%d\n", GetLastError());
		//9.清理协议版本信息
		WSACleanup();
		return -1;
	}
	printf("创建SOCKET成功！\n");

	//3.获取服务器协议地址簇
	SOCKADDR_IN addr = { 0 };
	addr.sin_family = AF_INET;
	addr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(10212);

	//4.连接服务器
	int r = connect(clientSocket, (sockaddr*)&addr, sizeof addr);
	if (-1 == r)
	{
		printf("连接服务器失败：%d\n", GetLastError());
		outtextxy(400, 250, _T("未打开服务器"));
		//6.关闭socket
		closesocket(clientSocket);
		//7.清理协议版本信息
		WSACleanup();
		return -1;
	}
	printf("连接服务器成功！\n");
	r = recv(clientSocket, RecvBuffer, 255, NULL);
	if (r == 0)
		return -1;
	RecvBuffer[r] = '\0';
	if (strcmp(RecvBuffer, "-1") == 0)//相同 为0
		position = -1;
	else if (strcmp(RecvBuffer, "1") == 0)
		position = 1;
	r = recv(clientSocket, RecvBuffer, 255, NULL);
	if (r == 0)
		return -1;
	RecvBuffer[r] = '\0';
	
	int over = 0;
	initgraph(600, 630);
	init_back();
	
	settextcolor(RED);
	settextstyle(20, 10, _T("黑体"));
	setbkmode(TRANSPARENT);
	outtextxy(0, 600, RecvBuffer);
	if (position == 1)//先手
	{
		r = recv(clientSocket, RecvBuffer, 255, NULL);
		if (r == 0)
			return -1;
		RecvBuffer[r] = '\0';
		fillrectangle(0, 600, 800, 650);
		outtextxy(0, 600, RecvBuffer);
		is_land = 0;
	}
		
	while (1)
	{
		if (is_land == -1)
		{

			r = recv(clientSocket, RecvBuffer, 255, NULL);
			if (r == 0)
				return -1;
			RecvBuffer[r] = '\0';
			//outtextxy(0, 0, RecvBuffer);
			temp_x = RecvBuffer[0];
			temp_y = RecvBuffer[1];
			draw_front_piece(temp_x, temp_y);
			fillrectangle(0, 600, 800, 650);
			
			outtextxy(0, 600, _T("己方下"));
			over = check_five_piece(temp_x, temp_y);
			is_land = 0;
			if (over == 1)
			{
				settextstyle(80, 0, _T("楷体"));
				outtextxy(150, 250, _T("游戏结束"));

				outtextxy(150, 450, _T("返回菜单"));
				is_land = 1;
			}
			
			flushmessage(EX_MOUSE);
		}

		while (is_land == 0)
		{
			
			m = GetMouseMsg();

			if (m.uMsg == WM_LBUTTONDOWN && over == 0 )
			{
				n_x = m.x / 40 + 1;
				n_y = m.y / 40 + 1;

				if (draw_piece_single(n_x, n_y))//下棋
				{
					is_land = -1;
					buff[0] = n_x;
					buff[1] = n_y;
					send(clientSocket, buff, strlen(buff), NULL);
					fillrectangle(0, 600, 800, 650);
					outtextxy(0, 600, _T("对方下"));
					flushmessage(EX_MOUSE);
				}
				
				over = check_five_piece(n_x, n_y);
				if (over == 1)
				{
					settextstyle(80, 0, _T("楷体"));
					outtextxy(150, 250, _T("游戏结束"));

					outtextxy(150, 450, _T("返回菜单"));
					is_land = 1;

				}
			}
		}
		if (is_land == 1)
		{
			m = GetMouseMsg();
			if (m.uMsg == WM_LBUTTONDOWN)
			{ 

				if (m.x > 150 && m.x < 450 && m.y >450 && m.y < 600)
				{
					init_struct_piece();
					init_piece();
					initgraph(800, 630);
					menu();
					over = 0;
					flag = 0;

				}
			}
			if (flag == 0)
			break;
		}
		

	}
	//5.关闭socket
	closesocket(clientSocket);
	//6.清理协议版本信息
	WSACleanup();
	//while (1);

	return 0;
}

void chess_robot()
{

	position = 1;
	step = 0;
	over = 0;
	initgraph(800, 630);
	init_back();
	int x, y;
	while (1)
	{
		tools();
		m = GetMouseMsg();

		if (position == -1 && over == 0)
		{
			draw_piece_robot();
			step++;
		}
		
		click();
		over = check_five_piece(n_x, n_y);

		if (over == 1)
		{
			over_click();
		}
		
		if (flag == 0)
			break;
	}
}

int main()
{
	int mode;
	initgraph(800, 630);
	menu();

	while (1)
	{
		mciSendString("open 2.mp3 alias bkmusic", NULL, 0, NULL);
		mciSendString("play bkmusic repeat", NULL, 0, NULL);

		mode = 0;
		m = GetMouseMsg();

		if (m.uMsg == WM_LBUTTONDOWN )
		{
			if ((m.x > 100 && m.x < 400) && (m.y > 150 && m.y < 250))
				mode = 1;
			else if ((m.x > 100 && m.x < 400) && (m.y > 250 && m.y < 350))
				mode = 2;
			else if ((m.x > 100 && m.x < 400) && (m.y > 350 && m.y < 450))
				mode = 3;
		}

		switch (mode)
		{
		case 1: flag = 1; chess_single(); break;
		case 2: flag = 2; chess_double(); break;
		case 3: flag = 3; chess_robot(); break;
		}

	}

}