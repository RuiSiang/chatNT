#include "logger.h"
#include "peer_control.h"

#include <ncursesw/ncurses.h>
#include <ncursesw/form.h>
#include <ncursesw/panel.h>
#include <ncursesw/menu.h>
#include <cassert>
#include <cstring>
#include <cstdlib>
#include <cctype>

#define ctrl(x) ((x) & 0x1f)

//misc functions
char *trim_whitespaces(char *);
void connectionDriver(int ch);
void messageDriver(int ch);

//core functions
void initUI();
void genConnectionForm();
void genMessageForm();
void genUserListMenu();
void refreshUserList();
void toggleMessageWindow();
void messageWindowEnter();
void refreshMessages();
void destroyObjects();
void destroyUI();
