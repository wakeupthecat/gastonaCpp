/*
Gastona C++ 
Copyright (C) 2016-2019  Alejandro Xalabarder Aulet

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/

#ifndef _UNIRECT_HEADER
#define _UNIRECT_HEADER

struct uniRect
{
   uniRect (int px=0, int py=0, int pdx=0, int pdy=0):
      x  (px),
      y  (py),
      dx (pdx),
      dy (pdy)
   {
   }
   
   int x, y, dx, dy;
   
   int left ()   { return x; }
   int right ()  { return x + dx; }
   int top ()    { return y; }
   int bottom () { return y + dy; }
};

#endif
