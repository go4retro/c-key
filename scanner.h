/*
    Copyright Jim Brain and Brain Innovations, 2004
  
    This file is part of C=Key.

    C=Key is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    C=Key is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with C=Key; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef SCANNER_H
#define SCANNER_H

#define SCAN_FLAG_NONE  0
#define SCAN_FLAG_SHIFT 1
#define SCAN_FLAG_CTRL  2
#define SCAN_FLAG_CMDR  4

#define SCAN_MAP_NONE   0
#define SCAN_MAP_SHIFT  1
#define SCAN_MAP_EXT    2

void scan(void);

#endif