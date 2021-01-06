#include "ui_utils.h"

#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
using namespace std;

Scrollable::Scrollable()
{
  maxRowsInDisplayWindow = 0;
  maxColsInDisplayWindow = 0;
  charCntAllText = 0;
  startingRowIdx = 0;
  lastRowIdx = 0;
  textBody = nullptr;
  receiver = nullptr;
}

Scrollable::Scrollable(int rows, int cols, const char *receiverID)
{
  maxRowsInDisplayWindow = rows;
  maxColsInDisplayWindow = cols;
  textBody = new char[(rows - 2) * (cols - 3) * 50];

  for (int i = 0; i < (rows - 2) * (cols - 3) * 50; i++)
  {
    textBody[i] = '\0';
  }

  receiver = new char[strlen(receiverID) + 1];
  for (int i = 0; i < strlen(receiverID); i++)
  {
    receiver[i] = receiverID[i];
  }
  receiver[strlen(receiverID)] = '\0';

  charCntAllText = 0;
  startingRowIdx = 1; // 1-based
  lastRowIdx = 1;
}

void Scrollable::addText(char inputHash[], char inputMessage[])
{
  for (int i = 0; i < strlen(inputHash); i++)
  {
    textBody[charCntAllText] = inputHash[i];
    charCntAllText++;
  }

  for (int i = 0; i < 1; i++)
  {
    textBody[charCntAllText] = ':';
    charCntAllText++;
  }

  for (int i = 0; i < strlen(inputMessage); i++)
  {
    textBody[charCntAllText] = inputMessage[i];
    charCntAllText++;
  }

  for (int i = 0; i < 2; i++)
  {
    textBody[charCntAllText] = '\n';
    charCntAllText++;
  }

  lastRowIdx = getRowCount(textBody, maxRowsInDisplayWindow - 2, maxColsInDisplayWindow - 3);

  /* Keeps every message from me in the vector messagesWithReceiver, refresh 的時候再把不存在在這個 vector 的 message
        用 addText 的方式加到 textBody，並且更新 vector */
  // messagesWithReceiver.push_back(Message(inputHash, inputMessage));
}

void Scrollable::printSelectedText(WINDOW *messageDisplay, int rowLast)
{
  int rowToStart = rowLast - maxRowsInDisplayWindow + 3;
  char textDisplay[(maxRowsInDisplayWindow - 2) * (maxColsInDisplayWindow - 3)] = {0};
  int charCntTextDisplay = 0;

  int startingCharIndex = 0;
  int endingCharIndex = 0;
  for (int i = 0; i < strlen(textBody); i++)
  {
    if (getRowIndexOfAChar(textBody, i, maxRowsInDisplayWindow - 2, maxColsInDisplayWindow - 3) == rowToStart)
    {
      startingCharIndex = i;
      break;
    }
  }

  for (int i = 0; i < strlen(textBody); i++)
  {
    if (getRowIndexOfAChar(textBody, i, maxRowsInDisplayWindow - 2, maxColsInDisplayWindow - 3) == rowLast)
    {
      int startCharOfEndingLine = i;
      if ((startCharOfEndingLine + maxColsInDisplayWindow - 3) >= strlen(textBody))
        endingCharIndex = strlen(textBody) - 1;
      else
      {
        endingCharIndex = startCharOfEndingLine + maxColsInDisplayWindow - 3 - 1;
      }
      break;
    }
  }

  int totalStringLength = endingCharIndex - startingCharIndex + 1;

  for (int i = 0; i < totalStringLength; i++)
  {
    textDisplay[charCntTextDisplay] = textBody[startingCharIndex + i];
    charCntTextDisplay++;
  }

  mvwprintw(messageDisplay, 0, 0, textDisplay);
  wrefresh(messageDisplay);
}

int getRowCount(char message[], const int maxMessageRows, const int maxMessageCols)
{
  int stringLength = strlen(message);
  int basicRows = stringLength / maxMessageCols;
  if (stringLength % maxMessageCols != 0)
  {
    basicRows++;
  }

  for (int i = 0; i < stringLength; i++)
  {
    if (message[i] == '\n')
      basicRows++;
  }
  return basicRows - 2;
}

int getRowIndexOfAChar(char message[], int idxOfTheChar, const int maxMessageRows, const int maxMessageCols)
{
  // row index is 1-based, idxOfTheChar is 0-based
  int rowIndex = 0;
  rowIndex += (idxOfTheChar + 1) / maxMessageCols;
  if ((idxOfTheChar + 1) % maxMessageCols != 0)
  {
    rowIndex++;
  }
  for (int i = 0; i <= idxOfTheChar; i++)
  {
    if (message[i] == '\n')
    {
      rowIndex++;
    }
  }
  return rowIndex;
}
