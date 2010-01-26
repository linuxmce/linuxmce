/*
    Copyright 2004-2005 Chris Tallon

    This file is part of VOMP.

    VOMP is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    VOMP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with VOMP; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include "ringbuffer.h"

Ringbuffer::Ringbuffer()
{
  capacity = 0;
  content = 0;
  buffer = NULL;
  start = NULL;
  end = NULL;
}

Ringbuffer::~Ringbuffer()
{
  if (buffer) free(buffer);
  buffer = NULL;
  capacity = 0;
  content = 0;
  start = NULL;
  end = NULL;
}

int Ringbuffer::init(size_t size)
{
  capacity = size;
  buffer = (UCHAR*)malloc(capacity);
  if (!buffer) return 0;
  start = buffer;
  end = buffer;
  return 1;
}

int Ringbuffer::put(UCHAR* from, size_t amount)
{
  if (amount > capacity) return 0;

  if ((end + amount) <= (buffer + capacity))
  {
    memcpy(end, from, amount);
    end += amount;
    content += amount;

    if (end == (buffer + capacity)) end = buffer;
    if (content >= capacity)
    {
      start = end;
      content = capacity;
    }
    return 1;
  }
  else
  {
    size_t firstAmount = buffer + capacity - end;
    return (put(from, firstAmount) && put(from + firstAmount, amount - firstAmount));
  }
}

int Ringbuffer::get(UCHAR* to, size_t amount)
{
  if (amount > content) return get(to, content);

  if ((start + amount) <= (buffer + capacity))
  {
    memcpy(to, start, amount);
    start += amount;
    content -= amount;

    if (start == (buffer + capacity)) start = buffer;
    return amount;
  }
  else
  {
    size_t firstAmount = buffer + capacity - start;
    return (get(to, firstAmount) + get(to + firstAmount, amount - firstAmount));
  }
}

int Ringbuffer::getContent()
{
  return content;
}
