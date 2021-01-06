#include "peer_control.h"
#include "logger.h"
#include "ui_utils.h"

#include <ncursesw/ncurses.h>
#include <cstring>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <vector>

#define ctrl(x) (x & 0x1F)

using namespace std;

int scrollableWindows = 0; // keep track of the number of message history text bodies, with different receivers

ScrollableList scrollableList;

// function: if a messageHistory object has not been created for this user ID, create one.

int main()
{
  initscr();
  noecho();
  int yMax, xMax;
  getmaxyx(stdscr, yMax, xMax);
  int maxMessageRowsToDisplay = yMax * 2 / 3;
  int maxMessageColsToDisplay = xMax - 35 - 2;

  ConnectionInfo connectionInfo;

  WINDOW *commentWin = newwin(21, xMax - 90, 8, 85);
  WINDOW *commentDisplayWin = newwin(19, xMax - 93, 9, 87);
  box(commentWin, 0, 0);

  mvwprintw(commentWin, 0, 2, "Instructions");

  mvwprintw(commentDisplayWin, 1, 0, "CHANGE RECEIVER:\nctrl+g\n\nSCROLL RECEIVER:\nctrl+w / ctrl+s\n\nPULL NEW RECEIVERS:\nctrl+p\n\nVIEW CHAT HISTORY:\nkey_up / key_down\n\nREFRESH MESSAGE HISTORY:\nctrl+r\n\nEXIT PROGRAM:\nctrl+e");
  refresh();
  wrefresh(commentWin);
  wrefresh(commentDisplayWin);

  WINDOW *serverInfoWin = newwin(7, xMax - 90, 1, 85);
  box(serverInfoWin, 0, 0);

  mvwprintw(serverInfoWin, 0, 2, "Server Info: ");
  mvwprintw(serverInfoWin, 1, 2, "IP: ");
  wrefresh(serverInfoWin);

  keypad(serverInfoWin, true);

  int charCnt = 0;
  int cnt = 0;

  // IP input
  while (true)
  {
    int c = wgetch(serverInfoWin);

    if (c == 127) // delete text
    {
      connectionInfo.ipString[charCnt - 1] = '\0';
      charCnt--;
      wclear(serverInfoWin);
      box(serverInfoWin, 0, 0);
      mvwprintw(serverInfoWin, 0, 2, "Server Info: ");
      mvwprintw(serverInfoWin, 1, 2, "IP: ");
      mvwprintw(serverInfoWin, 1, 6, connectionInfo.ipString);
      wrefresh(serverInfoWin);
      continue;
    }

    else if (c == 10) //hit enter
    {
      wclear(serverInfoWin);
      box(serverInfoWin, 0, 0); // creates border for serverInfoWin
      mvwprintw(serverInfoWin, 0, 2, "Server Info: ");
      mvwprintw(serverInfoWin, 1, 2, "IP: ");
      mvwprintw(serverInfoWin, 1, 6, connectionInfo.ipString);
      mvwprintw(serverInfoWin, 3, 2, "PORT: ");
      cnt = 1;
      charCnt = 0;
      wrefresh(serverInfoWin);
      break;
    }

    else
    {
      connectionInfo.ipString[charCnt] = (char)c;
      charCnt++;
    }

    if (cnt == 0)
    {
      mvwprintw(serverInfoWin, 1, 6, connectionInfo.ipString);
      wrefresh(serverInfoWin);
    }
    else if (cnt == 1)
    {
      break;
    }
  }
  // port input
  while (true)
  {
    int c = wgetch(serverInfoWin); // wgetch() refreshed the window before taking in input

    cnt = 0;

    if (c == 127) // delete text
    {
      connectionInfo.portString[charCnt - 1] = '\0';
      charCnt--;

      wclear(serverInfoWin);
      box(serverInfoWin, 0, 0); // creates border for serverInfoWin
      mvwprintw(serverInfoWin, 0, 2, "Server Info: ");
      mvwprintw(serverInfoWin, 1, 2, "IP: ");
      mvwprintw(serverInfoWin, 1, 6, connectionInfo.ipString);
      mvwprintw(serverInfoWin, 3, 2, "PORT: ");
      mvwprintw(serverInfoWin, 3, 8, connectionInfo.portString);
      continue;
    }

    else if (c == 10) //hit enter
    {
      // break;
      wclear(serverInfoWin);
      box(serverInfoWin, 0, 0); // creates border for serverInfoWin
      mvwprintw(serverInfoWin, 0, 2, "Server Info: ");
      mvwprintw(serverInfoWin, 1, 2, "IP: ");
      mvwprintw(serverInfoWin, 1, 6, connectionInfo.ipString);
      mvwprintw(serverInfoWin, 3, 2, "PORT: ");
      mvwprintw(serverInfoWin, 3, 8, connectionInfo.portString);

      cnt = 1;
      wrefresh(serverInfoWin);
      break;
    }

    else
    {
      connectionInfo.portString[charCnt] = (char)c;
      charCnt++;
    }

    if (cnt == 0)
    {
      mvwprintw(serverInfoWin, 3, 8, connectionInfo.portString);
      wrefresh(serverInfoWin);
    }
    else if (cnt == 1)
    {
      break;
    }
  }

  //init peer control class
  PeerControl *peerControl = new PeerControl(connectionInfo.ipString, stoi(connectionInfo.portString));

  // this box that contains the message display window
  WINDOW *messageBox = newwin(maxMessageRowsToDisplay, maxMessageColsToDisplay, 1, 1);
  box(messageBox, 0, 0);
  mvwprintw(messageBox, 0, 2, "All messages:");
  wrefresh(messageBox);

  // the display window within the border box
  WINDOW *messageDisplay = newwin(maxMessageRowsToDisplay - 2, maxMessageColsToDisplay - 3, 2, 3); // the display window can accommodate (maxMessageRowsToDisplay - 2) rows and (maxMessageColsToDisplay - 3) cols

  // the box that contains the window for displaying the drop down menu for peers
  WINDOW *optionClientHash = newwin(8, 75, 21, 9);
  box(optionClientHash, 0, 0);
  wrefresh(optionClientHash);

  // the window that displays the drop down menu
  WINDOW *optionDisplayClientHash = newwin(6, 73, 22, 10);
  wrefresh(optionDisplayClientHash);

  // the window that displays the selected peer
  WINDOW *showSelectedClientHash = newwin(3, 75, 21, 9);
  box(showSelectedClientHash, 0, 0);
  wrefresh(optionClientHash);

  // the window that displays "To: "; has no border box
  WINDOW *textClientHash = newwin(1, 8, 22, 1);
  wprintw(textClientHash, "To:");
  wrefresh(textClientHash);

  char inputHash[100] = {0};
  int charCntHash = 0;

  char inputMessage[1000] = {0};
  int charCntMessage = 0;

  // the variable that keeps track of which row in being displayed at the last visible row of the display window
  int currentLastRow = 0;
  peerControl->updateList();
  vector<User> userList = peerControl->getList();
  int choice;
  int highlight = 0;
  // receiver scrolling
  curs_set(0);
  while (true)
  {
    for (int i = 0; i < userList.size(); i++)
    {
      if (i == highlight)
      {
        wattron(optionClientHash, A_REVERSE);
        mvwprintw(optionClientHash, i + 1, 2, userList[i].hashId.c_str());
        wattroff(optionClientHash, A_REVERSE);
      }
    }
    choice = wgetch(optionClientHash);
    switch (choice)
    {
    case ctrl('w'):
      highlight--;
      if (highlight == -1)
        highlight = 0;
      break;
    case ctrl('s'):
      highlight++;
      if (highlight == userList.size())
        highlight = userList.size() - 1;
      break;

    default:
      break;
    }
    if (choice == 10)
      break;
  }
  wclear(optionClientHash);
  wclear(optionDisplayClientHash);
  mvwprintw(showSelectedClientHash, 1, 2, userList[highlight].hashId.c_str());

  wrefresh(optionClientHash);
  wrefresh(optionDisplayClientHash);
  wrefresh(showSelectedClientHash);

  WINDOW *fieldMessage = newwin(5, 75, 24, 9);
  box(fieldMessage, 0, 0);
  wrefresh(fieldMessage);
  curs_set(1);
  wmove(fieldMessage, 1, 2);

  WINDOW *textMessage = newwin(1, 8, 25, 1);

  wprintw(textMessage, "Message:");
  wrefresh(textMessage);

  keypad(fieldMessage, true);

  Scrollable mainMessageHistory(maxMessageRowsToDisplay - 2, maxMessageColsToDisplay - 3, userList[highlight].hashId.c_str());
  scrollableList.addReceiver(mainMessageHistory);

  Scrollable *currentMessageReceiver = &mainMessageHistory;

  // message input
  while (true)
  {
    int c = wgetch(fieldMessage);

    if (c == 127)
    {
      if (charCntMessage == 0)
      {
        continue;
      }
      else
      {
        inputMessage[charCntMessage - 1] = '\0';
        charCntMessage--;
        wclear(fieldMessage);
        box(fieldMessage, 0, 0);
        wrefresh(fieldMessage);
        mvwprintw(fieldMessage, 1, 2, inputMessage);
        continue;
      }
    }

    else if (c == 10)
    {
      char newInputHash[100] = {0};

      for (int i = 0; i < strlen(currentMessageReceiver->receiver); i++)
      {
        newInputHash[i] = (currentMessageReceiver->receiver)[i];
      }

      currentMessageReceiver->charCntAllText = strlen(currentMessageReceiver->textBody);

      currentMessageReceiver->addText(newInputHash, inputMessage);
      currentLastRow = currentMessageReceiver->lastRowIdx;
      int rowsOfTextAll = getRowCount(currentMessageReceiver->textBody, maxMessageRowsToDisplay - 2, maxMessageColsToDisplay - 3);
      if (rowsOfTextAll <= maxMessageRowsToDisplay - 2)
      {
        wclear(messageDisplay);
        currentMessageReceiver->printAllText(messageDisplay);
      }
      else
      {
        wclear(messageDisplay);
        currentMessageReceiver->printSelectedText(messageDisplay, currentMessageReceiver->lastRowIdx);
      }

      wclear(fieldMessage);
      box(fieldMessage, 0, 0);
      memset(inputMessage, '\0', 1000);
      charCntMessage = 0;
      wmove(fieldMessage, 1, 2);

      continue;
    }

    else if (c == ctrl('e'))
    {
      break;
    }

    else if (c == KEY_UP)
    {
      if (currentLastRow <= maxMessageRowsToDisplay - 2)
      {
        wclear(messageDisplay);
        currentMessageReceiver->printSelectedText(messageDisplay, currentLastRow);
        continue;
      }
      else
      {
        currentLastRow--;
        wclear(messageDisplay);
        currentMessageReceiver->printSelectedText(messageDisplay, currentLastRow);
        continue;
      }
    }

    else if (c == KEY_DOWN)
    {
      if (currentLastRow < currentMessageReceiver->lastRowIdx)
      {
        currentLastRow++;
      }
      wclear(messageDisplay);
      currentMessageReceiver->printSelectedText(messageDisplay, currentLastRow);
      continue;
    }

    else if (c == ctrl('g'))
    {
      WINDOW *optionClientHash = newwin(8, 75, 21, 9);
      box(optionClientHash, 0, 0);
      wrefresh(optionClientHash);

      WINDOW *optionDisplayClientHash = newwin(6, 73, 22, 10);
      wrefresh(optionDisplayClientHash);

      WINDOW *showSelectedClientHash = newwin(3, 75, 21, 9);
      box(showSelectedClientHash, 0, 0);
      wrefresh(optionClientHash);

      wclear(textMessage);
      wrefresh(textMessage);
      curs_set(0);

      while (true)
      {
        for (int i = 0; i < userList.size(); i++)
        {
          if (i == highlight)
            wattron(optionClientHash, A_REVERSE);
          mvwprintw(optionClientHash, i + 1, 2, userList[i].hashId.c_str());
          wattroff(optionClientHash, A_REVERSE);
        }
        choice = wgetch(optionClientHash);
        switch (choice)
        {
        case ctrl('w'):
          highlight--;
          if (highlight == -1)
            highlight = 0;
          break;
        case ctrl('s'):
          highlight++;
          if (highlight == userList.size())
            highlight = userList.size() - 1;
          break;

        default:
          break;
        }
        if (choice == 10)
          break;
      }

      wclear(optionClientHash);
      wclear(optionDisplayClientHash);

      mvwprintw(showSelectedClientHash, 1, 2, userList[highlight].hashId.c_str());
      if (scrollableList.isNewReceiver(userList[highlight].hashId.c_str()))
      {
        Scrollable addedMessageHistory(maxMessageRowsToDisplay - 2, maxMessageColsToDisplay - 3, userList[highlight].hashId.c_str());
        scrollableList.addReceiver(addedMessageHistory);
        currentMessageReceiver = &(scrollableList.scrollableList[scrollableList.scrollableList.size() - 1]);
      }

      else
      {
        int indexOFReceiver = scrollableList.findIdxInScrollableArray(userList[highlight].hashId.c_str());
        currentMessageReceiver = &(scrollableList.scrollableList[indexOFReceiver]);
        cout << currentMessageReceiver->textBody;
      }

      WINDOW *textMessage = newwin(1, 8, 25, 1);

      wprintw(textMessage, "Message:");
      wrefresh(textMessage);

      wrefresh(textMessage);
      wrefresh(optionClientHash);
      wrefresh(optionDisplayClientHash);
      wrefresh(showSelectedClientHash);
      curs_set(1);
      wmove(fieldMessage, 1, 2);
      box(fieldMessage, 0, 0);
      wrefresh(fieldMessage);

      wclear(messageDisplay);
      currentMessageReceiver->printSelectedText(messageDisplay, currentMessageReceiver->lastRowIdx);
      continue;
    }

    //this event handles the event of pulling new peers; commented out to ensure the program can run
    else if (c == ctrl('p'))
    {
      peerControl->updateList();
      userList = peerControl->getList();
    }

    // this event handles the event of refreshing message history window; commented out to ensure the program can run
    /*else if (c == ctrl('r'))
    {
      vector<Message> fullMessageHistory = peerControl->getMessages();
      // compare fullMessageHistory with the messagesWithReceiver vector of currentMessageReceiver window
      for (int i = 0; i < fullMessageHistory.size(); i++)
      {
        bool found = 0;
        for (int j = 0; j <)
          if (fullMessageHistory[i] not in currentMessageReceiver->messagesWithReceiver)
          {
            messagesWithReceiver.push_back(fullMessageHistory[i]);
            currentMessageReceiver->addText(fullMessageHistory[i].hashID.c_str(), fullMessageHistory[i].message.c_str());
          }
      }
      currentLastRow = currentMessageReceiver->lastRowIdx;
      currentMessageReceiver->printSelectedText(messageDisplay, currentLastRow);
      wmove(fieldMessage, 1, 2);
    }*/

    else
    {
      inputMessage[charCntMessage] = (char)c;
      charCntMessage++;
    }
    mvwprintw(fieldMessage, 1, 2, inputMessage);
    wrefresh(fieldMessage);
  }

  endwin();
}
