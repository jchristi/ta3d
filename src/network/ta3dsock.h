/*  TA3D, a remake of Total Annihilation
    Copyright (C) 2006  Roland BROCHARD

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


#ifndef TA3D_NET_SOCK_H__
# define TA3D_NET_SOCK_H__

#include "../stdafx.h"
#include "SocketClass.h"
#include "../threads/thread.h"
#include "../threads/mutex.h"
#include <string>


namespace TA3D
{


    /*
    ** notes on packets
    **
    ** chat packet
    ** 1 byte  = N+1
    ** 1 byte  = 'C' (or 'X' if it's a special packet, for example for LAN servers discovery)
    ** N bytes = null terminated message string
    ** total = N+2
    **
    ** order packet
    **    1 byte  = 23  (not buffered)
    ** 0  1 byte  = 'O'
    ** 1  4 bytes = time of execution
    ** 5  4 bytes = unit number
    ** 9  1 byte  = order type
    ** 10 4 bytes = floating data 1
    ** 14 4 bytes = floating data 2
    ** 18 4 bytes = target unit
    ** 22 1 byte  = this is an additional order
    ** total = 24
    **
    ** sync packet
    **    1 byte  = 27  (not buffered)
    ** 0  1 byte  = 'S'
    ** 1  4 bytes = time of sync
    ** 5  4 bytes = unit number
    ** 9  4 bytes = floating x
    ** 13 4 bytes = floating y
    ** 17 4 bytes = floating vx
    ** 21 4 bytes = floating vy
    ** 25 2 bytes = orientation
    ** total = 28
        **
        ** event packet
        ** 1 byte  = 4
        ** 1 byte  = 'E'
        ** 1 byte  = event type
        ** 1 byte  = player 1
        ** 1 byte  = player 2
        ** total = 5
        */

#define TA3DSOCK_BUFFER_SIZE 2560

        //chat order sync and event
        //these are sent and received over the network
        struct chat
        {
            uint16 from; 		//uint16 who said
            char message[253];	//said what
        };//max size = 254

    struct order
    {
        uint32	timestamp;	//uint32 what time the order happened
        uint16	unit;		//uin16 order what unit
        byte	command;		//uint8 move,fire,stop,guard,reclaim,etc
        uint32	x;
        uint32	y;
        uint16	target;		//uint16 enough to tell it a target unit
        byte	additional;	//uint8 0=cancel current activity 1=add to order queue
    };//max size = 22

#define SYNC_FLAG_FLYING	0x01
#define SYNC_FLAG_CLOAKING	0x02

    struct sync{
        uint32	timestamp;	//uint32 what tick is this snapshot
        uint16	unit;		//uint16 sync what unit
        real32	x,y,z;
        real32	vx,vy,vz;
        uint16	orientation;//uint16 where 0=0 and 65535~=2pi? ie rad=(rot1/65536.0)*2pi?
        uint16	hp;
        uint8	build_percent_left;
        uint8	flags;		// Some flags (flying, etc..)
    };//max size = 28

#define	EVENT_UNIT_CREATION			0x00
#define EVENT_UNIT_DEATH			0x01
#define EVENT_WEAPON_CREATION		0x02
#define EVENT_UNIT_DAMAGE			0x03
#define EVENT_UNIT_SCRIPT			0x04
#define EVENT_UNIT_SYNCED			0x05		// UDP : Used to tell a unit was synced
#define EVENT_CAMERA_POS			0x06		// Used by scripts
#define EVENT_CLF					0x07		// script ta3d_clf()
#define EVENT_INIT_RES				0x08		// script ta3d_init_res()
#define EVENT_PLAY					0x09		// play a sound file
#define EVENT_PRINT					0x0A		// display a message
#define EVENT_CLS					0x0B		// clear script screen
#define EVENT_DRAW					0x0C		// draw a picture
#define EVENT_UNIT_EXPLODE			0x0D		// a unit explodes
#define EVENT_FEATURE_CREATION		0x0E
#define EVENT_FEATURE_DEATH			0x0F
#define EVENT_FEATURE_FIRE			0x10
#define EVENT_SCRIPT_SIGNAL			0x11		// Send a signal to player
#define EVENT_UNIT_NANOLATHE		0x12		// Tell when a unit is nanolathing something
#define EVENT_UNIT_PARALYZE			0x13        // A unit gets paralyzed

    struct event{
        byte	type;		//uint8 what type of event
        uint16	opt1;		//uint16 optional parameters
        uint16	opt2;		//uint16
        uint32	opt3;		//uint32
        uint32	opt4;		//uint32
        byte	opt5;		//byte
        real32	x;			//real32
        real32	y;			//real32
        real32	z;			//real32
        real32	vx;			//real32
        real32	vy;			//real32
        real32	vz;			//real32
        sint16	dx;			//sint16
        sint16	dy;			//sint16
        sint16	dz;			//sint16
        byte	str[128];	//unit name ? weapon name ?
    };//max size = 172

    typedef chat special;

    chat* strtochat(struct chat *chat_msg, String msg);

    String chattostr( struct chat *chat_msg );

    //TA3DSock- specialized low-level networking
    //used internally by Network to send the data
    //to the players. It is basically a TCP socket
    class TA3DSock{

        char name[64];//player name

        Socket tcpsock;
        Mutex tcpmutex;

        //only touched by main thread
        char outbuf[TA3DSOCK_BUFFER_SIZE];
        int obp;

        //only touched by socket thread
        char tcpinbuf[TA3DSOCK_BUFFER_SIZE];
        int tibp;
        int tiremain;//how much is left to recv
        int tibrp;

        //byte shuffling
        void putLong(uint32_t x);//uint32
        void putShort(uint16_t x);//uint16
        void putByte(uint8_t x);//uint8
        void putString(const char* x);//null terminated
        void putFloat(float x);

        uint32	getLong();//uint32
        uint16	getShort();//uint16
        byte	getByte();//uint8
        void	getString(char* x);//null terminated
        void	getBuffer(char* x, int size);
        float	getFloat();

        void sendTCP();
        void recvTCP();

        int max(int a, int b) {return (a>b ? a : b);}

    public:
        TA3DSock() {obp=0;tibp=0;tiremain=-1;}
        ~TA3DSock() {}

        int Open(const char* hostname,const char* port);
        int Accept(TA3DSock** sock);
        int Accept(TA3DSock** sock,int timeout);
        void Close();

        void sendTCP(byte *data, int size);

        const char* getAddress() const {return tcpsock.getNumber();}
        const char* getPort() const {return tcpsock.getService();}
        Socket& getSock() {return tcpsock;}
        int isOpen();

        //these are for outgoing packets
        int sendSpecial(struct chat* chat, bool all=false);
        int sendPing();
        int sendChat(struct chat* chat);
        int sendOrder(struct order* order);
        int sendSync(struct sync* sync);
        int sendEvent(struct event* event);
        void cleanPacket();

        //these are for incoming packets
        int makeSpecial(struct chat* chat);
        int makeChat(struct chat* chat);
        int makeOrder(struct order* order);
        int makeSync(struct sync* sync);
        int makeEvent(struct event* event);
        int makePing();

        int getFilePort();				// For file transfer, first call this one to get the port which allows us to grab the right thread and buffer
        int getFileData(byte *buffer);	// Fill the buffer with the data and returns the size of the paquet

        char getPacket();//if packet is ready return the type, else return -1
        void pumpIn();//get input from sockets non-blocking
        int takeFive(int time);//sleep until sockets will not block
    };


} // namespace TA3D

#endif // TA3D_NET_SOCK_H__
