/*
    C=Key - Commodore <-> PS/2 Interface
    Copyright Jim Brain and RETRO Innovations, 2004-2011

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    scanner.h: Definitions for generic keyboard scanning routine

*/
#ifndef SCANNER_H
#define SCANNER_H

#define SCAN_MAP_NONE   0
#define SCAN_MAP_SHIFT  1
#define SCAN_MAP_EXT    2

void scan_init(void);
void scan_irq(void);
void scan(void);


#endif

