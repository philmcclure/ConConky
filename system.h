#if !defined(__SYSTEM_H__)
#define __SYSTEM_H__

#include <utmp.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>


#include <list>
#include <string>
#include <vector>


class System
{
public:
    System();
    ~System();

    void                output_lines();
    void                push_line(std::string line);

private:
    void                _get_tty_list();
    void                _get_tty_descriptors_list();
    void                _get_tty_size(int tty_fd, int &height, int &width);
    void                _output_line_to_tty(int tty_fd, int line_no, const char* line);
	void				_normalizeLines(int terminal_cols, std::vector<std::string> &lines);
    void                _reset();

    std::list<std::string>      _tty_list;
    std::list<int>              _tty;
    std::vector<std::string>    _lines;

};

void			show_error(char* s);
unsigned char	number_of_columns();
void			daemonize();



#endif
