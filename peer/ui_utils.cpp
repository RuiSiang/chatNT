#include "ui_utils.h"

using namespace std;

extern PeerControl *peerControl;
FORM *connectionForm, *messageForm;
MENU *userListMenu;
FIELD *connectionFields[5], *messageFields[2];
WINDOW *mainWindow, *connectionWindow, *connectionFormSubWin, *messageWindow, *messageFormSubWin, *userListMenuSubWin, *userListWindow, *helpWindow, *historyWindow, *historyPad;
bool showUserMenu = 1;
string buddyHashId;
int yMaxHistory, xMaxHistory;
int padColumn = 0, padLines, padPositionY, padPositionX, maxMessageChars, currentMessageChars = 0;

char *trim_whitespaces(char *str)
{
  char *end;
  if (*str == 0)
    return str;
  end = str + strlen(str) - 1;
  while (end > str && isspace((unsigned char)*end))
    end--;
  end[1] = '\0';
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
    {
      form_driver(messageForm, REQ_DEL_PREV);
      currentMessageChars = max(0, currentMessageChars - 1);
    }
    break;

  case KEY_DOWN:
    if (showUserMenu)
      menu_driver(userListMenu, REQ_DOWN_ITEM);
    else
    {
      padColumn++;
      prefresh(historyPad, padColumn, 0, padPositionY, padPositionX, padPositionY + yMaxHistory - 3, padPositionX + xMaxHistory - 4);
    }
    break;

  case KEY_UP:
    if (showUserMenu)
      menu_driver(userListMenu, REQ_UP_ITEM);
    else
    {
      padColumn = max(padColumn - 1, 0);
      prefresh(historyPad, padColumn, 0, padPositionY, padPositionX, padPositionY + yMaxHistory - 3, padPositionX + xMaxHistory - 4);
    }
    break;

  default:
    if (!showUserMenu)
    {
      if (currentMessageChars < maxMessageChars)
      {
        form_driver(messageForm, ch);
        currentMessageChars++;
      }
    }

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
  init_pair(1, COLOR_BLACK, COLOR_WHITE);
  init_pair(2, COLOR_WHITE, COLOR_BLUE);

  wbkgd(stdscr, COLOR_PAIR(1));

  int yMax, xMax;
  getmaxyx(stdscr, yMax, xMax);
  mainWindow = newwin(yMax, xMax - 1, 0, 0);
  assert(mainWindow != NULL);
  box(mainWindow, 0, 0);
  wbkgd(mainWindow, COLOR_PAIR(1));

  int yMaxMain, xMaxMain;
  getmaxyx(mainWindow, yMaxMain, xMaxMain);

  connectionWindow = derwin(mainWindow, 10, int(xMaxMain * 0.3 - 2), 1, int(xMax * 0.7));
  assert(connectionWindow != NULL);
  box(connectionWindow, 0, 0);
  wbkgd(connectionWindow, COLOR_PAIR(1));

  int yMaxConnection, xMaxConnection;
  getmaxyx(connectionWindow, yMaxConnection, xMaxConnection);
  connectionFormSubWin = derwin(connectionWindow, 5, int(xMaxConnection - 4), 2, 1);
  assert(connectionFormSubWin != NULL);

  messageWindow = derwin(mainWindow, int(yMaxMain * 0.3 - 1), int(xMaxMain * 0.7 - 3), int(yMaxMain * 0.7), 2);
  assert(messageWindow != NULL);
  box(messageWindow, 0, 0);
  wbkgd(messageWindow, COLOR_PAIR(1));

  int yMaxMessage, xMaxMessage;
  getmaxyx(messageWindow, yMaxMessage, xMaxMessage);
  messageFormSubWin = derwin(messageWindow, yMaxMessage - 2, int(xMaxMessage - 2), 1, 1);
  userListMenuSubWin = derwin(messageWindow, yMaxMessage - 2, int(xMaxMessage - 2), 1, 1);
  assert(userListMenuSubWin != NULL);

  helpWindow = derwin(mainWindow, int(yMaxMain - 12), int(xMaxMain * 0.3 - 2), 11, int(xMax * 0.7));
  assert(helpWindow != NULL);
  box(helpWindow, 0, 0);

  historyWindow = derwin(mainWindow, int(yMaxMain * 0.7 - 3), int(xMaxMain * 0.7 - 3), 3, 2);
  assert(historyWindow != NULL);
  box(historyWindow, 0, 0);
  padPositionY = 4;
  padPositionX = 4;
  getmaxyx(historyWindow, yMaxHistory, xMaxHistory);

  mvwprintw(mainWindow, 1, 2, "chatNT - Anonymous Chatroom");
  mvwprintw(helpWindow, 1, 2, "Help");
  mvwprintw(helpWindow, 3, 2, "TAB   - toggle message /");
  mvwprintw(helpWindow, 4, 2, "        user list");
  mvwprintw(helpWindow, 5, 2, "Enter - send message /");
  mvwprintw(helpWindow, 6, 2, "        select user");
  mvwprintw(helpWindow, 7, 2, "Crl+e - exit program");
  refresh();
}

void genConnectionForm()
{
  mvwprintw(connectionWindow, 1, 3, "Relay Server Info");
  connectionFields[0] = new_field(1, 10, 1, 2, 0, 0);
  connectionFields[1] = new_field(1, 20, 1, 7, 0, 0);
  connectionFields[2] = new_field(1, 10, 2, 2, 0, 0);
  connectionFields[3] = new_field(1, 20, 2, 7, 0, 0);
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

  set_field_back(connectionFields[0], COLOR_PAIR(1));
  set_field_back(connectionFields[1], A_UNDERLINE | COLOR_PAIR(1));
  set_field_back(connectionFields[2], COLOR_PAIR(1));
  set_field_back(connectionFields[3], A_UNDERLINE | COLOR_PAIR(1));
  set_field_type(connectionFields[1], TYPE_REGEXP, "\b((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)(\.|$)){4}\b");
  set_field_type(connectionFields[3], TYPE_INTEGER, 0, 1024, 65535);

  connectionForm = new_form(connectionFields);
  assert(connectionForm != NULL);
  set_form_sub(connectionForm, connectionFormSubWin);
  post_form(connectionForm);

  wrefresh(mainWindow);
  wrefresh(connectionWindow);
}

void genMessageForm()
{
  int yMaxMessage, xMaxMessage;
  getmaxyx(messageWindow, yMaxMessage, xMaxMessage);

  messageFields[0] = new_field(yMaxMessage - 2, int(xMaxMessage - 5), 0, 2, 0, 0);
  maxMessageChars = (yMaxMessage - 2) * (xMaxMessage - 5) - 1;
  messageFields[1] = NULL;
  assert(messageFields[0] != NULL);

  set_field_buffer(messageFields[0], 0, "");
  set_field_opts(messageFields[0], O_VISIBLE | O_PUBLIC | O_EDIT | O_ACTIVE);
  set_field_back(messageFields[0], COLOR_PAIR(1) | A_UNDERLINE);

  messageForm = new_form(messageFields);
  assert(messageForm != NULL);

  set_form_sub(messageForm, messageFormSubWin);

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

  set_menu_sub(userListMenu, userListMenuSubWin);
  set_menu_format(userListMenu, yMaxMessage - 2, 1);
  set_menu_fore(userListMenu, COLOR_PAIR(2));
  set_menu_back(userListMenu, COLOR_PAIR(1));
  set_menu_grey(userListMenu, COLOR_PAIR(1));
  post_menu(userListMenu);

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
    curs_set(1);
  }
  else
  {
    unpost_form(messageForm);
    refreshUserList();
    post_menu(userListMenu);
    curs_set(0);
  }
  showUserMenu = !showUserMenu;
  wrefresh(mainWindow);
  wrefresh(messageWindow);
}

void messageWindowEnter()
{
  if (showUserMenu)
  {
    buddyHashId = userListVec[item_index(current_item(userListMenu))].hashId;
    mvwprintw(mainWindow, 1, 2, string("Your hash id: " + peerControl->hashId).c_str());
    mvwprintw(mainWindow, 2, 2, string("Chatting with " + buddyHashId).c_str());
    toggleMessageWindow();
  }
  else
  {
    form_driver(messageForm, REQ_VALIDATION);
    bool sendResult = peerControl->formPacketandSend(buddyHashId, string(trim_whitespaces(field_buffer(messageFields[0], 0))));
    if (!sendResult)
    {
      info("Message sent successfully\n");
    }
    else
    {
      error("Message send failed\n");
    }
    form_driver(messageForm, REQ_CLR_FIELD);
  }
  refreshMessages();
}

void refreshMessages()
{
  delwin(historyPad);
  padLines = 0;
  string historyString = "";
  vector<Message> messages = peerControl->getMessages();
  for (int i = 0; i < messages.size(); i++)
  {
    if (messages[i].sender == peerControl->hashId)
    {
      historyString += "You: " + messages[i].message + "\n";
      padLines += (messages[i].message.length() + 5) / (xMaxHistory - 4) + 2;
    }
    else if (messages[i].sender == buddyHashId)
    {
      historyString += "Buddy: " + messages[i].message + "\n";
      padLines += (messages[i].message.length() + 7) / (xMaxHistory - 4) + 2;
    }
  }
  historyPad = newpad(padLines, xMaxHistory - 4);
  waddstr(historyPad, historyString.c_str());
  prefresh(historyPad, padColumn, 0, padPositionY, padPositionX, padPositionY + yMaxHistory - 3, padPositionX + xMaxHistory - 4);
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
  unpost_menu(userListMenu);
  free_menu(userListMenu);
}

void destroyUI()
{
  delwin(connectionWindow);
  delwin(messageWindow);
  delwin(mainWindow);
  endwin();
}