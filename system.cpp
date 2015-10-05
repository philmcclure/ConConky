/*
Copyright 2012-2013 Alexander Gulchenko aka A.Stahl

ConConky is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

ConConky is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>
*/

#include "system.h"
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>


System::System()
{


}

System::~System()
{


}

void  System::_get_tty_list()
{
    struct utmp *utmpptr;
    _tty_list.clear();

    while((utmpptr = getutent()))
    {
        if (!utmpptr->ut_name[0])
            continue;
        #ifdef USER_PROCESS
        if (utmpptr->ut_type != USER_PROCESS)
            continue;
        #endif

        if (utmpptr->ut_line[0] == ':')
            continue;

        std::string tty_name(utmpptr->ut_line);
        _tty_list.push_back("/dev/" + tty_name);
    }
    endutent();
}


void  System::_get_tty_descriptors_list()
{
    for (auto i = _tty_list.begin(); i != _tty_list.end(); ++i)
    {
        int descr = open((*i).c_str() , O_RDWR | O_NOCTTY);
        if (descr > 0)
        {
            _tty.push_back(descr);
        }
    }
}

void  System::_get_tty_size(int tty_fd, int &height, int &width)
{
    struct winsize w;
    ioctl(tty_fd, TIOCGWINSZ, &w);
    height = w.ws_row;
    width = w.ws_col;
}

void  System::_output_line_to_tty(int tty_fd, int line_no, const char* line)
{
    int width, height;
    _get_tty_size(tty_fd, height, width);

	char    code_move_cursor[100];
    memset(code_move_cursor, 0, 100);
    struct termios tty, orig_tty;

	tcgetattr( tty_fd, &tty );        // получили состояние терминала
	orig_tty = tty;
    tty.c_lflag &= ~( ICANON | ECHO | ISIG );
    tty.c_cc[ VMIN ] = 1;
    tcsetattr( tty_fd, TCSADRAIN, &tty ); // изменили состояние терминала

	if (width - (int)strlen(line))
	{
		sprintf(code_move_cursor, "\e7""%c[%d;%dH""\e[K""%s""\e8", 27, line_no,  width - (int)strlen(line) + 2, line);
		write(tty_fd, code_move_cursor, strlen(code_move_cursor));
	}

	tcsetattr(tty_fd, TCSADRAIN, &orig_tty);
}

void System::_normalizeLines(int terminal_cols, std::vector<std::string> &lines)
{
	const size_t com_line_len = 20;
	size_t max_len = terminal_cols - com_line_len;

	size_t de_facto_max_len = 0;
	for (auto line = lines.begin(); line != lines.end(); ++line)
    {
		if ((*line).size() > de_facto_max_len)
		{
			de_facto_max_len = (*line).size();
		}
    }

	if (de_facto_max_len > max_len)
	{
		de_facto_max_len = max_len;
	}

	for (auto line = lines.begin(); line != lines.end(); ++line)
    {

		if ((*line).size() > de_facto_max_len)
		{
			// truncate if needed
			(*line) = (*line).substr(0, de_facto_max_len - 3) + std::string("...");
		}
		else
		if ((*line).size() < de_facto_max_len)
		{
			// add spaces in front
			int space_len = de_facto_max_len - (*line).size();
			std::string space_fill(space_len, ' ');
			(*line) = space_fill + (*line);
		}

		// Ok, now we can add border
		(*line) = std::string("│ ") + (*line);
	}

	// And at last we can add footer-border
	std::string footer = std::string("╰") + std::string(de_facto_max_len + 1, '-');
	lines.push_back(footer);

}


void  System::_reset()
{
    // clear _lines and close descripors
    for (auto tty = _tty.begin(); tty != _tty.end(); ++tty)
    {
        close(*tty);
    }

    _tty_list.clear();
    _tty.clear();
    _lines.clear();
}

void  System::output_lines()
{
    _get_tty_list();
    _get_tty_descriptors_list();
	int width, height;


    for (auto tty = _tty.begin(); tty != _tty.end(); ++tty)
    {
		 _get_tty_size(*tty, height, width);
		std::vector<std::string> temp = _lines;
		 _normalizeLines(width, temp);
		int line_no = 1;
        for (auto line = temp.begin(); line != temp.end(); ++line, ++line_no)
        {
            _output_line_to_tty(*tty, line_no, (*line).c_str());
        }
    }

    _reset();

}

void  System::push_line(std::string line)
{
    _lines.push_back(line);

}



