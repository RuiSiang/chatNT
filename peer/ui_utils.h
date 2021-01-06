#include "peer_control.h"

#include <ncursesw/ncurses.h>
#include <cstring>
#include <vector>
int getRowCount(char message[], const int maxMessageRows, const int maxMessageCols);
int getRowIndexOfAChar(char message[], int idxOfTheChar, const int maxMessageRows, const int maxMessageCols);

struct ConnectionInfo
{
  char ipString[100] = {0};
  char portString[100] = {0};
};

class Scrollable
{
public:
  int maxRowsInDisplayWindow, maxColsInDisplayWindow, charCntAllText, startingRowIdx, lastRowIdx;
  char *textBody;
  char *receiver;
  std::vector<Message> messagesWithReceiver;
  Scrollable();
  Scrollable(int, int, const char *);
  void addText(char[], char[]);
  void printAllText(WINDOW *messageDisplay)
  {
    mvwprintw(messageDisplay, 0, 0, textBody);
    wrefresh(messageDisplay);
  }
  void printSelectedText(WINDOW *, int);
};

class ScrollableList
{
public:
  std::vector<Scrollable> scrollableList;
  bool isNewReceiver(const char *receiverIDToCheck)
  {
    for (int i = 0; i < scrollableList.size(); i++)
    {
      if (strcmp(scrollableList[i].receiver, receiverIDToCheck) == 0)
      {
        return false;
      }
    }
    return true;
  }

  void addReceiver(Scrollable newReceiver)
  {
    scrollableList.push_back(newReceiver);
  }

  int findIdxInScrollableArray(const char *receiverIDToCheck)
  {
    int index = 0;
    for (int i = 0; i < scrollableList.size(); i++)
    {
      if (strcmp(scrollableList[i].receiver, receiverIDToCheck) == 0)
      {
        index = i;
        return i;
      }
    }
    return -1;
  }
};
