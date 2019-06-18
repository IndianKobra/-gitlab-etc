//g++ textred.cpp -lncursesw -o textred
#include <ncurses.h>
#include <list>
#include <locale.h>
#include <vector>
#include <string>
using namespace std;
#include "FileArray.cpp"
#ifndef CTRL
#define CTRL(c) ((c) & 037)
#endif

int row,col, cur_x=0, cur_y=0, screen_y, LettersColor, ConsoleLettersColor, BackgroundColor, SaveButton = CTRL('s'), CompileButton;
bool ConsoleMode=0, Exit;
string FileName, CompileCommand = "g++ -o proga ", message = "";
list<char> Text, ConsoleCommnd;
list<char>::iterator cur = Text.end(), Console_cur = ConsoleCommnd.end();
FileArray<int> Settings("config", 1);
int Str2Int(string S)
{
	int Ans = 0;
	for(int i = 0; i < S.size(); i++) if(S[i] >= '0' && S[i] <= '9') Ans = Ans*10+S[i]-'0';
	return Ans;
}
bool Compile()
{
	if(system((CompileCommand+FileName).c_str())) message = "Executed "+CompileCommand+FileName;
}
void WriteText()
{
	int printed = 0, StringsFasted = 0, CharWasted = 0;
	init_pair(1, LettersColor, BackgroundColor);
	init_pair(2, BackgroundColor, LettersColor);   
	init_pair(3, ConsoleLettersColor, BackgroundColor);
	init_pair(4, BackgroundColor, ConsoleLettersColor);
	bkgd( COLOR_PAIR(1));
	wclear(stdscr);
	for(list<char>::iterator i = Text.begin(); i != Text.end(); i++)
	{
		if(*i!='\r'&&(*i&192) !=128) CharWasted++;
		if(*i=='\n' || CharWasted>=col)
		{
			StringsFasted++;
			CharWasted=0;
		}
		if(i==cur)break;
	}
	list<char>::iterator j = Text.begin();
	if(screen_y<StringsFasted - row + 2) screen_y = StringsFasted-row + 2;
	if(screen_y>StringsFasted -2) screen_y = StringsFasted-2;
	for(int k = 0; k < screen_y; k++)
	{
		CharWasted = 0;
		while(j != Text.end() && (*j != '\n' && CharWasted != col))
		{
			j++;
			if(*j!='\r'&&(*j&192) !=128)CharWasted++;
		} 
		if((*j)=='\n')j++;
	}
	CharWasted = 0;
	StringsFasted=0;
	for(list<char>::iterator i = j; i != Text.end(); i++)
	{
		attron(COLOR_PAIR(1+(i==cur)));
		if(*i!='\r'&&(*i&192) !=128)CharWasted++;
		if(*i=='\n'&&i==cur && CharWasted!=col)printw("%c", ' ');
		if(*i!='\r')printw("%c", *i);/*addch(*i);*/ 
		attroff(COLOR_PAIR(1+(i==cur)));
		if(*i=='\n' || CharWasted==col)
		{
			StringsFasted++;
			CharWasted=0;
		}
		if(StringsFasted>row) break;
	}
	if(cur == (Text.end()))
	{
		attron(COLOR_PAIR(2));
		printw("%c", ' ');
		attroff(COLOR_PAIR(2));
	}
	j = cur;
	cur_x = (*j&192) !=128;
	while(j !=  Text.begin() && (*--j) != ('\n') )if((*j&192) !=128&&(*j != '\r')) cur_x++;
	move(row-1, 0);
	if(!ConsoleMode) printw("Line %i, column %i %s\n", cur_y, cur_x, message.c_str());
	else
	{
		attron(COLOR_PAIR(3));
		printw("%s", "~:") ;
		attroff(COLOR_PAIR(3));
		for(list<char>::iterator i = ConsoleCommnd.begin(); i != ConsoleCommnd.end(); i++)
		{
			attron(COLOR_PAIR(3+(i==Console_cur)));
			printw("%c", *i);/*addch(*i);*/ 
			attroff(COLOR_PAIR(3+(i==Console_cur)));
		}
		if(Console_cur == (ConsoleCommnd.end()))
		{
			attron(COLOR_PAIR(4));
			printw("%c", ' ');
			attroff(COLOR_PAIR(4));
		}
		printw("%c", '\n');
	}
	message = "";
}
void OpenFile(string NewFileName)
{
	/*iconv_t conv = iconv_open("UTF-8","CP1251");
iconv(conv, &src_ptr, &src_len, &dst_ptr, &dst_len);
iconv_close(conv);*/
	FILE *file = fopen(NewFileName.c_str(), "rb");
	if(!file) message = "file \"" + NewFileName + "\" not found";
	if(!file) return ;
	Text.clear();
	cur = Text.begin();
	char c;
	while(fread(&c, 1, 1, file)) Text.insert ((cur), c);
	fclose(file);
	cur = Text.begin();
	cur_y=0;
	int charlen = ((*cur&192) == 192)+((*cur&224) == 224)+((*cur&240) == 240);//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
	for(int i = 0; i < charlen; i++) cur++;
	FileName = NewFileName;
}

void SaveFile()
{
	FILE *file = fopen(FileName.c_str(), "wb+");
	if(!file) message = "error with file"  + FileName;
	if(FileName == "") message = "filename is not declarated";
	if(!file) return;
	for(list<char>::iterator i = Text.begin(); i != Text.end(); i++)
	{
		char c = (*i);
		fwrite(&c, 1, 1, file);
	}
	fclose(file);
	message = "Saved file " + FileName;
}
void SaveSettings()
{
	Settings.resize(4+FileName.size());
	Settings[0] = LettersColor;
	Settings[1] = ConsoleLettersColor;
	Settings[2] = BackgroundColor;
	Settings[3] = FileName.size();
	if(FileName.size()>0) for(int i = 0; i < FileName.size(); i++) Settings[4+i] = FileName[i];
}
bool LoadSettings()
{
	if(Settings.size()<4)
	{
		LettersColor = COLOR_RED;
		ConsoleLettersColor = COLOR_GREEN;
		BackgroundColor = COLOR_BLACK;
		FileName = "";
	}
	else
	{
		LettersColor = Settings[0];
		ConsoleLettersColor = Settings[1];
		BackgroundColor = Settings[2];
		FileName = "";
		for(int i=0; i < Settings[3]; i++) FileName+=Settings[4+i]; 
		if(FileName!="") OpenFile(FileName);
	}
}
void ShowHelp()
{
	wclear(stdscr);
	attron(COLOR_PAIR(1));
	printw("help - show with help\nSave - save text, mat be added name of file. Exaple: Save 1.txt; Save\nOpen - open file with inputed name. Example: Open 1.txt\nNew - start work with new(unsaved) file\nSetBackgroundColor - change color of background\nSetLetterColor - change color of text\nCompile - compile opened file\nCNTRL+S - SAVE\nCNTRL+B - Compile\nPRESS ANY KEY 2 CONTINUE");
	attroff(COLOR_PAIR(1));
	getch();
}
void ExecuteCommnd()
{
	string S = "";
	for(list<char>::iterator i = ConsoleCommnd.begin(); i != ConsoleCommnd.end(); i++)
	{
		S+=*i;
	}
	vector<string> Tokens;
	for(int i = 0; i < S.size(); i++)
	{
		int b = i;//begin
		if(S[b]!= ' ')   while(i+1 < S.size() && S[i+1]!=' ') i++;
		if(S[b]!= ' ') Tokens.push_back(S.substr(b, i-b+1));
	}
	if(Tokens.size()==0) return;
	if(Tokens[0] == "Save" && (Tokens.size()>1 || FileName != ""))
	{
		if(Tokens.size()>1) FileName = Tokens[1];
		SaveFile();
	}
	else if(Tokens[0] == "Open" && Tokens.size()>1)
	{
		OpenFile(Tokens[1]);
	}
	else if(Tokens[0] == "New")
	{
		FileName = "";
		Text.clear();
		cur = Text.end();
		cur_y=0;
	}
	else if(Tokens[0] ==  "SetBackgroundColor" && Tokens.size()>1)
	{
		BackgroundColor = Str2Int(Tokens[1]);
	}
	else if(Tokens[0] ==  "SetLetterColor" && Tokens.size()>1)
	{
		LettersColor = Str2Int(Tokens[1]);
	}
	else if(Tokens[0] == "Compile" && Tokens.size()>1)
	{
		Compile();
	}
	else if(Tokens[0] ==  "Exit")
	{
		Exit = true;
	}
	else if(Tokens[0] ==  "help")
	{
		ShowHelp();
	}
	else message = "Unknowen command " + Tokens[0];
	SaveSettings();
}
int main()
{
	wchar_t c = '\0';
	setlocale(LC_ALL, "");
	initscr();  
	raw();
	keypad(stdscr, TRUE);
	noecho();
	curs_set(FALSE);
	set_escdelay(50);
	start_color();
	init_color(COLOR_BLACK, 0, 0, 0);
	getmaxyx(stdscr,row,col);
	LoadSettings();
	while(!Exit)
	{
		list<char>::iterator cur1;
		WriteText();
		refresh();
		c = getch();
		if(ConsoleMode)
		{
			switch(c)
			{
				case KEY_LEFT:
					if(cur!= Text.end())while((*cur&192) ==128) cur--;
					if(Console_cur==ConsoleCommnd.begin())
					{
						int charlen = ((*Console_cur&192) == 192)+((*Console_cur&224) == 224)+((*Console_cur&240) == 240);//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
						for(int i = 0; i < charlen; i++) Console_cur++;
					}
					else Console_cur--;
				break;
				case KEY_RIGHT:
					if(Console_cur != ConsoleCommnd.end())
					{
						Console_cur++;
						int charlen = ((*Console_cur&192) == 192)+((*Console_cur&224) == 224)+((*Console_cur&240) == 240);//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
						for(int i = 0; i < charlen; i++) Console_cur++;
					}
				break;
				case KEY_RESIZE:
					getmaxyx(stdscr,row,col);
				break;
				case '\n':
					ExecuteCommnd();
					ConsoleCommnd.clear();
					Console_cur = ConsoleCommnd.begin();
					ConsoleMode = FALSE;
				break;
				case KEY_BACKSPACE:
					cur1 = Console_cur;
					while((*cur1&192) ==128) cur1--;
					if(cur1 != ConsoleCommnd.begin())
					{
						cur1--;
						while((*cur1&192) ==128) ConsoleCommnd.erase(cur1--);
						ConsoleCommnd.erase(cur1);
					}
				break;
				case KEY_F(3):
					ConsoleMode=FALSE;
					ConsoleCommnd.clear();
					Console_cur=ConsoleCommnd.begin();
				break;
				case 27://esc |alt
					nodelay(stdscr,TRUE);
					c= getch();
					if(c==-1||c==27)//esc
					{
						ConsoleMode=FALSE;
						ConsoleCommnd.clear();
						Console_cur=ConsoleCommnd.begin();  
					}
					else //alt+c
					{
						ConsoleCommnd.insert (Console_cur, c);
					}
					nodelay(stdscr,FALSE);	
				 break;
				default:
					cur1 = Console_cur;
					while((*cur1&192) ==128) cur1--;
					ConsoleCommnd.insert ((cur1), c);
			}
		}
		else
		{
			switch(c)
			{
				case KEY_LEFT:
					if(cur!= Text.end())while((*cur&192) ==128) cur--;
					if(cur==Text.begin())
					{
						int charlen = ((*cur&192) == 192)+((*cur&224) == 224)+((*cur&240) == 240) + (*cur == '\r');//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
						for(int i = 0; i < charlen; i++) cur++;
					}
					else cur--;
					if(*cur == '\n') cur_y --;
					if(*cur == '\r') cur--;
				break;
				case KEY_RIGHT:
					if(cur != Text.end())
					{
						if(*cur == '\n') cur_y ++;
						cur++;
						int charlen = ((*cur&192) == 192)+((*cur&224) == 224)+((*cur&240) == 240) + (*cur == '\r');//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
						if(cur != Text.end())for(int i = 0; i < charlen; i++) cur++;
					}
				break;
				case CTRL('s'):
					SaveFile();
				break;
				case CTRL('b'):
					Compile();
				break;
				case KEY_UP:
					cur1 = cur;
					if(cur1!= Text.begin())cur1--;
					while(cur1 != Text.begin() &&*cur1 != '\n') cur1--;
					if(cur1!= Text.begin())
					{
						cur1--;
						while(cur1 != Text.begin() &&*cur1 != '\n') cur1--;
						if(*cur1 == '\n') cur1++;
						int charlen = ((*cur1&192) == 192)+((*cur1&224) == 224)+((*cur1&240) == 240) + (*cur1 == '\r');//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
						for(int i = 0; i < charlen; i++) cur1++;//мы в начале предыдущей строки
						for(int j = 1; j < cur_x; j++)
						{
							if(*cur1 == '\n') break;
							cur1++;
							int charlen = ((*cur1&192) == 192)+((*cur1&224) == 224)+((*cur1&240) == 240) + (*cur1 == '\r');//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
							for(int i = 0; i < charlen; i++) cur1++;
						}
						cur = cur1;
						cur_y--;
					}
				break;
				case KEY_DOWN:
					cur1 = cur;
					while(cur1 != Text.end() &&*cur1 != '\n') cur1++;
					if(cur1!= Text.end())
					{/*мы в конце этой строки, а нам на следующую*/
						for(int j = 0; j < cur_x; j++)
						{
							cur1++;
							int charlen = ((*cur1&192) == 192)+((*cur1&224) == 224)+((*cur1&240) == 240) + (*cur1 == '\r');//charlen=0 if 1 byte =1 if 2bytes =3if4bytes
							for(int i = 0; i < charlen; i++) cur1++;
							if(*cur1 == '\n' || cur1 == Text.end()) break;
						}
						cur = cur1;
						cur_y++;
					}
				break;
				case 27://esc |alt
					nodelay(stdscr,TRUE);
					c= getch();
					if(c==-1||c==27)//esc
					{
						ConsoleMode = TRUE;
					}
					else //alt+c
					{
						Text.insert ((cur), c);
					}
					nodelay(stdscr,FALSE);	
				break;
				case KEY_RESIZE:
					getmaxyx(stdscr,row,col);
				break;
				case KEY_BACKSPACE:
					cur1 = cur;
					if(cur1 != Text.end()) while((*cur1&192) ==128) cur1--;
					if(cur1 != Text.begin())
					{
						cur1--;
						if(*cur1 =='\n') cur_y--;
						while((*cur1&192) ==128 || (*cur == '\r')) Text.erase(cur1--);
						Text.erase(cur1);
					}
				break;
				case KEY_F(3):
					ConsoleMode = TRUE;
				break;
				default:
					if(c == '\n') cur_y++;
					cur1 = cur;
					if(cur1 != Text.end()) while((*cur1&192) ==128) cur1--;
					Text.insert ((cur1), c);
					/*bool cntrl = FALSE;
					for(int i = 0; i < 26; i++) if(c == CTRL('a'+i) || c == CTRL('A'+i)) cntrl= true; 
					if(!cntrl)Text.insert ((cur), c);*/
			}
		}
	}
	endwin();
	return 0;
}