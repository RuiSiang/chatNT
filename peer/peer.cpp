#include "ui_utils.h"

using namespace std;

PeerControl *peerControl;
extern FORM *connectionForm, *messageForm;
extern FIELD *connectionFields[5], *messageFields[3];
extern WINDOW *mainWindow, *connectionWindow, *messageWindow, *userListWindow;

int main()
{
  int ch;

  initUI();

  genConnectionForm();

  while ((ch = getch()) != 10)
    connectionDriver(ch);
  mvwprintw(connectionWindow, 8, 3, "                          ");
  mvwprintw(connectionWindow, 7, 3, "Loading...                ");
  wrefresh(connectionWindow);
  form_driver(connectionForm, REQ_VALIDATION);

  peerControl = new PeerControl(trim_whitespaces(field_buffer(connectionFields[1], 0)), stoi(string(trim_whitespaces(field_buffer(connectionFields[3], 0)))));
  mvwprintw(mainWindow, 1, 2, string("Your hash id: " + peerControl->hashId).c_str());
  mvwprintw(connectionWindow, 7, 3, "                          ");
  wrefresh(connectionWindow);

  curs_set(0);
  genMessageForm();
  genUserListMenu();

  while ((ch = getch()) != ctrl('e'))
  {
    messageDriver(ch);
  }

  destroyObjects();
  destroyUI();

  return 0;
}