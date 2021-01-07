#include "ui_utils.h"

using namespace std;

extern PeerControl *peerControl;
FORM *connectionForm, *messageForm;
MENU *userListMenu;
FIELD *connectionFields[5], *messageFields[2];
WINDOW *mainWindow, *connectionWindow, *messageWindow, *userListWindow;
bool showUserMenu = 1;
string friendHashId;

char *trim_whitespaces(char *str)
{
  char *end;
  while (isspace(*str))
    str++;
  if (*str == 0)
    return str;
  end = str + strnlen(str, 128) - 1;

  while (end > str && isspace(*end))
    end--;
  *(end + 1) = '\0';
  return str;
}

void connectionDriver(int ch)
{
  switch (ch)
  {
  case '\t':
  case KEY_STAB:
    form_driver(connectionForm, REQ_NEXT_FIELD);
    form_driver(connectionForm, REQ_END_FIELD);
    break;

  case 127:
  case '\b':
  case KEY_BACKSPACE:
    form_driver(connectionForm, REQ_DEL_PREV);
    break;

  default:
    form_driver(connectionForm, ch);
    break;
  }

  wrefresh(connectionWindow);
}

void messageDriver(int ch)
{
  switch (ch)
  {
  case '\t':
  case KEY_STAB:
    toggleMessageWindow();
    break;
  case 10:
    messageWindowEnter();
    break;
  case 127:
  case '\b':
  case KEY_BACKSPACE:
    if (!showUserMenu)
      form_driver(messageForm, REQ_DEL_PREV);
    break;

  case KEY_DOWN:
    if (showUserMenu)
      menu_driver(userListMenu, REQ_DOWN_ITEM);
    break;

  case KEY_UP:
    if (showUserMenu)
      menu_driver(userListMenu, REQ_UP_ITEM);
    break;

  default:
    if (!showUserMenu)
      form_driver(messageForm, ch);
    break;
  }

  wrefresh(messageWindow);
}

void initUI()
{
  initscr();
  noecho();
  start_color();
  cbreak();
  keypad(stdscr, TRUE);

  init_pair(1, COLOR_BLUE, COLOR_WHITE);
  init_pair(2, COLOR_BLUE, COLOR_RED);

  int yMax, xMax;
  getmaxyx(stdscr, yMax, xMax);
  mainWindow = newwin(yMax, xMax - 1, 0, 0);
  assert(mainWindow != NULL);
  box(mainWindow, 0, 0);

  int yMaxMain, xMaxMain;
  getmaxyx(mainWindow, yMaxMain, xMaxMain);
  connectionWindow = derwin(mainWindow, 10, int(xMaxMain * 0.3 - 4), 1, int(xMax * 0.7));
  assert(connectionWindow != NULL);
  box(connectionWindow, 0, 0);
  messageWindow = derwin(mainWindow, int(yMaxMain * 0.3 - 1), int(xMaxMain * 0.7 - 5), int(yMaxMain * 0.7), 4);
  assert(messageWindow != NULL);
  box(messageWindow, 0, 0);
  //wbkgd(messageWindow, COLOR_PAIR(1));

  mvwprintw(mainWindow, 1, 2, "chatNT - Anonymous Chatroom");
}

void genConnectionForm()
{
  connectionFields[0] = new_field(1, 10, 0, 2, 0, 0);
  connectionFields[1] = new_field(1, 20, 0, 7, 0, 0);
  connectionFields[2] = new_field(1, 10, 3, 2, 0, 0);
  connectionFields[3] = new_field(1, 20, 3, 7, 0, 0);
  mvwprintw(connectionWindow, 7, 3, "Press Tab to switch field");
  mvwprintw(connectionWindow, 8, 3, "Press Enter to submit");
  connectionFields[4] = NULL;
  assert(connectionFields[0] != NULL && connectionFields[1] != NULL && connectionFields[2] != NULL && connectionFields[3] != NULL);

  set_field_buffer(connectionFields[0], 0, "IP");
  set_field_buffer(connectionFields[1], 0, "");
  set_field_buffer(connectionFields[2], 0, "Port");
  set_field_buffer(connectionFields[3], 0, "");

  set_field_opts(connectionFields[0], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
  set_field_opts(connectionFields[1], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_opts(connectionFields[2], O_VISIBLE | O_PUBLIC | O_AUTOSKIP);
  set_field_opts(connectionFields[3], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);

  set_field_back(connectionFields[1], A_UNDERLINE);
  set_field_back(connectionFields[3], A_UNDERLINE);

  connectionForm = new_form(connectionFields);
  assert(connectionForm != NULL);
  int yMaxConnection, xMaxConnection;
  getmaxyx(connectionWindow, yMaxConnection, xMaxConnection);
  set_form_sub(connectionForm, derwin(connectionWindow, 6, int(xMaxConnection - 4), 1, 1));
  post_form(connectionForm);

  refresh();
  wrefresh(mainWindow);
  wrefresh(connectionWindow);
}

void genMessageForm()
{
  int yMaxMessage, xMaxMessage;
  getmaxyx(messageWindow, yMaxMessage, xMaxMessage);

  messageFields[0] = new_field(yMaxMessage - 2, int(xMaxMessage - 5), 0, 0, 0, 0);
  messageFields[1] = NULL;
  assert(messageFields[0] != NULL);

  set_field_buffer(messageFields[0], 0, "");
  set_field_opts(messageFields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(messageFields[0], A_UNDERLINE);

  messageForm = new_form(messageFields);
  assert(messageForm != NULL);

  set_form_sub(messageForm, derwin(messageWindow, yMaxMessage - 2, int(xMaxMessage - 2), 1, 1));

  refresh();
  wrefresh(mainWindow);
  wrefresh(messageWindow);
}

vector<User> userListVec;
ITEM **userList;

void genUserListMenu()
{
  userListVec.clear();
  userListMenu = new_menu(NULL);
  refreshUserList();

  int yMaxMessage, xMaxMessage;
  getmaxyx(messageWindow, yMaxMessage, xMaxMessage);

  assert(userListMenu != NULL);

  set_menu_sub(userListMenu, derwin(messageWindow, yMaxMessage - 2, int(xMaxMessage - 2), 1, 1));
  set_menu_format(userListMenu, yMaxMessage - 2, 1);
  post_menu(userListMenu);

  refresh();
  wrefresh(mainWindow);
  wrefresh(messageWindow);
}

void refreshUserList()
{
  peerControl->updateList();
  userListVec = peerControl->getList();
  userList = (ITEM **)calloc(userListVec.size() + 1, sizeof(ITEM *));
  for (int i = 0; i < userListVec.size(); i++)
    userList[i] = new_item(userListVec[i].hashId.c_str(), NULL);
  userList[userListVec.size()] = new_item(NULL, NULL);
  set_menu_items(userListMenu, userList);
}

void toggleMessageWindow()
{
  if (showUserMenu)
  {
    post_form(messageForm);
    unpost_menu(userListMenu);
  }
  else
  {
    unpost_form(messageForm);
    refreshUserList();
    post_menu(userListMenu);
  }
  showUserMenu = !showUserMenu;
  refresh();
  wrefresh(mainWindow);
  wrefresh(messageWindow);
}

void messageWindowEnter()
{
  if (showUserMenu)
  {
    friendHashId = userListVec[item_index(current_item(userListMenu))].hashId;
    mvwprintw(mainWindow, 1, 2, string("chatNT wth peer " + friendHashId).c_str());
    toggleMessageWindow();
  }
  else
  {
    //todo:sendMessage
  }
}

void destroyObjects()
{
  unpost_form(connectionForm);
  free_form(connectionForm);
  free_field(connectionFields[0]);
  free_field(connectionFields[1]);
  free_field(connectionFields[2]);
  free_field(connectionFields[3]);
  unpost_form(messageForm);
  free_form(messageForm);
  free_field(messageFields[0]);
  //todo free menu
}

void destroyUI()
{
  delwin(connectionWindow);
  delwin(messageWindow);
  delwin(mainWindow);
  endwin();
}