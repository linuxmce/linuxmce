/*
     Copyright (C) 2004 Pluto, Inc., a Florida Corporation

     www.plutohome.com

     Phone: +1 (877) 758-8648
 

     This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License.
     This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
     of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

     See the GNU General Public License for more details.

*/
#define MAX 4096

#include <stdio.h>
#include <string.h>

const char * colors[] = {"black", "red", "green", "yellow", "blue", "magenta", "cyan", "white"};
const int d_fg = 7, d_bg = 0, d_bold = 0, d_underline = 0;

void open_span(int fg, int bg, int bold, int underline)
{
	printf("<span style=\"background:%s;color:%s;%s%s\">", colors[bg], colors[fg],
			(bold ? "font-weight:bold;" : ""), (underline ? "text-decoration:underline;" : ""));
}

void close_span()
{
	printf("</span>");
}

int parse_ansi(const char * ansi, int * fg, int * bg, int * bold, int * underline)
{
	unsigned int i, len;

	len = strlen(ansi);
	for (i = 2; i < len; i++)
	{
		switch (ansi[i])
		{
			case '3':
				i++;
				* fg = ansi[i] - '0';
				break;
			case '4':
				i++;
				if (ansi[i] >= '0' && ansi[i] <= '9')
				{
					* bg = ansi[i] - '0';
				}
				else
				{
					* underline = 1;
					i--; // in this case, we're one step too far
				}
				break;
			case '0':
				* fg = d_fg;
				* bg = d_bg;
				* bold = d_bold;
				* underline = d_underline;
				break;
			case '1':
				* bold = 1;
				break;
			case ';':
				continue;
			case 'm':
				close_span();
				open_span(* fg, * bg, * bold, * underline);
				return i;
		}
	}

	return 0;
}

int main()
{
	char line[MAX];
	int fg = d_fg, bg = d_bg, bold = d_bold, underline = d_underline;
	unsigned int i, len;

	while (fgets(line, MAX, stdin))
	{
		open_span(fg, bg, bold, underline);
		len = strlen(line);
		for (i = 0; i < len; i++)
		{
			if (line[i] != '\x1B')
			{
				if (line[i] != '\n' && line[i] != '\r')
					printf("%c", line[i]);
			}
			else
			{
				i += parse_ansi(&line[i], &fg, &bg, &bold, &underline);
			}
		}
		close_span();
		printf("\n");
		fflush(stdout);
	}
	
	return 0;
}
