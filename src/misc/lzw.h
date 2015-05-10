/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2005  Roland BROCHARD

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
    Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA*/

#ifndef __LZW__

#define __LZW__

byte *LZW_compress( byte *data, int size );
byte *LZW_decompress( byte *data, int *p_size = NULL );

class BYTE_FLOW
{
public:
	int		pos;
	int		s;
	byte	*flow;

	BYTE_FLOW( byte *data )
	{
		flow = data;
		pos = 0;
		s = 9;
	}

	inline void set_size( int size ) {	s = size;	}
	void put_code( int code );
	int get_code();
};

#endif
