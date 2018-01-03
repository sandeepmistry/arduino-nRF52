#include "scroll_support.h"


ScrollSupport *ScrollSupport::makeFor(readingDirection_t dir, String message)
{
  switch (dir) {
  case TopToBottom: return new ScrollUp(message);
  case LeftToRight: return new ScrollLeft(message);
  case RightToLeft: return new ScrollRight(message);
  }
}


ScrollSupport::ScrollSupport(String aMessage)
{
  message = aMessage;
}


ScrollLeft::ScrollLeft(String aMessage) : ScrollSupport(aMessage)
{
}


ScrollRight::ScrollRight(String aMessage) : ScrollSupport(aMessage)
{
}


ScrollUp::ScrollUp(String aMessage) : ScrollSupport(aMessage)
{
}

