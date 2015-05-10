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

#include "SocketClass.h"
#include "../logs/logs.h"



namespace TA3D
{


    String Socket::getError()
    {
        NLint error = nlGetError();
        if (error == NL_SYSTEM_ERROR)
            return String(nlGetSystemErrorStr(nlGetSystemError()));
        return String(nlGetErrorStr(nlGetError()));
    }

    Socket::Socket()
    {
        sockreports = 1;
        sockerrors = 1;
        strncpy(number,"new socket",NI_MAXHOST);
        strncpy(service,"???",NI_MAXSERV);
        stype = STYPE_BROKEN;
    }


    Socket::~Socket()
    {
        if (stype != STYPE_BROKEN)
        {
            if( nlClose(fd) == NL_FALSE )
                LOG_ERROR(LOG_PREFIX_NET_SOCKET << nlGetErrorStr(nlGetError()));
        }
    }


    Socket::Socket(const char *hostname, const char *port, int transport)
    {
        strncpy(number,"new socket",NI_MAXHOST);
        strncpy(service,"???",NI_MAXSERV);
        stype = STYPE_BROKEN;
        sockreports = 1;
        sockerrors = 1;
        if (Open(hostname, port, transport) < 0)
            LOG_ERROR(LOG_PREFIX_NET_SOCKET << "Socket initialization for `" << hostname << "`:" << port << " failed !");
    }

    Socket::Socket(const char *hostname, const char *port)
    {
        strncpy(number,"new socket",NI_MAXHOST);
        strncpy(service,"???",NI_MAXSERV);
        stype = STYPE_BROKEN;
        sockreports = 1;
        sockerrors = 1;
        if (Open(hostname, port, PROTOCOL_TCPIP) < 0)
            LOG_ERROR(LOG_PREFIX_NET_SOCKET << "Socket initialization for `" << hostname << "`:" << port << " failed !");
    }

    int Socket::Open(const char *hostname, const char *port)
    {
        return Open(hostname,port,PROTOCOL_TCPIP);
    }



    int Socket::Open(const char *hostname, const char *port, int transport)
    {
        //create a socket
        if (stype != STYPE_BROKEN)
        {
            sockError("Open: this socket is already open");
            return -1;
        }

        //determine stype
        if(hostname == NULL && transport == PROTOCOL_TCPIP)
        {
            sockReport("making tcp server");
            stype = STYPE_TCP_SERVER;
        }
        else
        {
            if(transport == PROTOCOL_TCPIP)
            {
                sockReport("making tcp client");
                stype = STYPE_TCP_CLIENT;
            }
        }
        if(hostname == NULL && transport == PROTOCOL_UDP )
        {
            sockReport("making udp receiver");
            stype = STYPE_UDP;
        }
        else
        {
            if(transport == PROTOCOL_UDP)
            {
                sockReport("making udp sender");
                stype = STYPE_UDP;
            }
        }
        if( transport == PROTOCOL_BROADCAST )
        {
            sockReport("making udp broadcaster");
            stype = STYPE_BROADCAST;
        }


        if( hostname )
        {
            bool valid = nlGetAddrFromName( hostname, &address ) != NL_FALSE;		// Get the address
            if( !valid || address.valid == NL_FALSE )
            {
                bool valid = nlGetAddrFromName( format( "%s:%s", hostname, port ).c_str(), &address ) != NL_FALSE;		// Retry adding port

                if( !valid || address.valid == NL_FALSE )
                {
                    stype = STYPE_BROKEN;
                    sockReport( format( "failed getting address for '%s'", hostname ).c_str() );
                    return -1;
                }
            }
        }

        // Try to connect

        int n_port = atoi( port );

        ta3d_socket s;
        if( stype == STYPE_TCP_CLIENT )
            s = nlOpen( 0, transport );
        else
            s = nlOpen( n_port, transport );

        if( s == NL_INVALID )
        {
            stype = STYPE_BROKEN;
            sockReport("cannot create socket : " + getError());
            return -1;
        }

        switch( stype )
        {
            case STYPE_UDP:
                if( hostname == NULL )	break;
                nlSetAddrPort(&address, n_port);
                nlSetRemoteAddr( s, &address );
                break;
            case STYPE_TCP_CLIENT:
                nlSetAddrPort(&address, n_port);
                if( nlConnect( s, &address ) == NL_FALSE )
                {
                    stype = STYPE_BROKEN;
                    sockReport("connect error : " + getError());
                    return -1;
                }
                break;
            case STYPE_TCP_SERVER:
                if( nlListen( s ) == NL_FALSE )
                {
                    stype = STYPE_BROKEN;
                    sockReport("listen error : " + getError());
                    return -1;
                }
                break;
            case STYPE_BROADCAST:			// We're ready to broadcast/receive
                break;
        }

        //set up socket details
        fd = s;
        strncpy(service,port,NI_MAXSERV);

        nlAddrToString(&address, number);
        for( char *f = number ; *f ; ++f) // Get rid of the port
            if( *f == ':' ) {
                *f = 0;
                break;
            }

        sockReport("socket opened");

        return 0;

    }


    int Socket::Close()
    {
        if (stype == STYPE_BROKEN)
            return 0;

        nlClose(fd);
        sockReport("socket closed");
        strncpy(number,"closed socket",NI_MAXHOST);
        strncpy(service,"???",NI_MAXSERV);
        stype = STYPE_BROKEN;
        return 0;
    }




    int Socket::Accept(Socket& sock,int timeout)
    {
        if(sock.stype != STYPE_BROKEN)
        {
            sockError("Accept: please provide a broken socket to fill-in");
            return -1;
        }
        if(stype != STYPE_TCP_SERVER)
        {
            sockError("Accept: only tcp server socket can accept!");
            return -1;
        }

        NLint group = nlGroupCreate();
        nlGroupAddSocket( group, fd );

        ta3d_socket s;
        NLint v = nlPollGroup( group, NL_READ_STATUS, &s, 1, timeout);

        nlGroupDestroy( group );

        if( v == NL_INVALID || v == 0 )			// Nothing to do
            return -1;

        s = nlAcceptConnection(fd);

        if (s == NL_INVALID )
        {
            sockError("Accept: accept() error" + getError());
            return -1;
        }

        sock.fd = s;
        sock.address = address;
        sock.stype = STYPE_TCP_CLIENT;

        nlGetRemoteAddr( s, &sock.address );

        strncpy(sock.service,service,NI_MAXSERV);

        nlAddrToString(&sock.address, sock.number);
        for( char *f = sock.number ; *f ; f++ )			// Get rid of the port
            if( *f == ':' ) {
                *f = 0;
                break;
            }

        sockReport("new connection accepted");
        sock.sockReport("connection established");
        return 0;
    }



    int Socket::Accept(Socket& sock)
    {
        return Accept( sock, 0 );
    }


    void Socket::sockError(const String& message)
    {
        if(sockreports)
        {
            String type = "broken";
            switch(stype)
            {
                case STYPE_UDP:         type = "UDP"; break;
                case STYPE_TCP_CLIENT:  type = "TCP"; break;
                case STYPE_TCP_SERVER:  type = "TCP"; break;
                case STYPE_BROADCAST:   type = "BROADCAST"; break;
                default: type = "broken";
            }
            LOG_ERROR(LOG_PREFIX_NET_SOCKET << type << ", [" << number << "]:" << service << " ->" << message);
        }
    }


    void Socket::sockReport(const String& message)
    {
        if(sockerrors)
        {
            String type;
            switch(stype)
            {
                case STYPE_UDP:         type = "UDP"; break;
                case STYPE_TCP_CLIENT:  type = "TCP"; break;
                case STYPE_TCP_SERVER:  type = "TCP"; break;
                case STYPE_BROADCAST:   type = "BROADCAST"; break;
                default: type = "broken";
            }
            LOG_ERROR(LOG_PREFIX_NET_SOCKET << type << ", [" << number << "]:" << service << " ->" << message);
        }
    }



    int Socket::Send(const void* data,int num)
    {
        if(stype == STYPE_BROKEN)
        {
            sockError("Send: socket must be open before sending");
            return -1;
        }
        if(stype == STYPE_TCP_SERVER)
        {
            sockError("Send: tcp server socket can't send");
            return -1;
        }

        int count = 0;
        int v = NL_INVALID;

        while( (v = nlWrite( fd, data, num )) == NL_INVALID )
        {
            sockError("Send: " +  getError());
            switch(nlGetError())
            {
            case NL_CON_PENDING: // Don't close connection, it's not even opened yet !!
                count++;
                if( count < 1000 )
                {
                    rest(10);
                    continue;
                }
                break;
            case NL_BUFFER_SIZE:        // The buffer is not large enough to hold the packet. ==> split the packet if possible
                if (num >= 2)
                {
                    int s1 = num / 2;
                    v = Send(data, s1);
                    if (v == -1 || v < s1)      // We didn't send everything, so stop there, higher level process should decide what to do here
                        return v;
                    int v2 = Send((const void*)(((const char*)data) + s1), num - s1); // Send the second part
                    if (v2 == -1)               // Success ?
                        return v;
                    return v + v2;                      // We've sent v + v2
                }
                else
                    LOG_ERROR( LOG_PREFIX_NET_SOCKET << "packet too big to fit in buffer, but too small to be split");
                break;
            case NL_NULL_POINTER:       // A NULL pointer was passed to the function.
            case NL_INVALID_SOCKET:     // The socket is not valid.
            case NL_CON_REFUSED:        // The pending connection on a non-blocking socket has been refused.
            case NL_SYSTEM_ERROR:       // A system Socket error has ocurred. The system error can be retrieved by calling nlGetSystemError.
            case NL_MESSAGE_END:
            default:
                break;
            };
            Close();
            return -1;
        }
        return v;
    }


    int Socket::Recv(void* data, int num)
    {
        if(stype == STYPE_BROKEN)
        {
            sockError("Recv: socket must be open before receiving");
            return -1;
        }
        if(stype == STYPE_TCP_SERVER)
        {
            sockError("Recv: tcp server socket can't recv");
            return -1;
        }
        int v = nlRead( fd, data, num );

#ifdef TA3D_PLATFORM_WINDOWS
        if( v == NL_INVALID && nlGetError() == 	NL_SYSTEM_ERROR && nlGetSystemError() == 10040 )		// Windows raise an error if you read only the beginning of the buffer !!
            v = num;
#endif
        if( v == NL_INVALID && nlGetError() != NL_BUFFER_SIZE )	// If buffer is too small then return what we've read but don't close the connection
        {
            sockError("Recv: " + getError());
            switch( nlGetError() )
            {
                case NL_SYSTEM_ERROR :
                case NL_INVALID_SOCKET :
                case NL_CON_REFUSED :
                case NL_MESSAGE_END :
                    Close();
            }
            return -1;
        }

        if( v == NL_INVALID )	v = num;		// We've filled all the buffer

        if( stype == STYPE_UDP || stype == STYPE_BROADCAST )
        {
            nlGetRemoteAddr( fd, &address );
            nlAddrToString(&address, number);
            for( char *f = number ; *f ; f++ )			// Get rid of the port
                if( *f == ':' ) {
                    *f = 0;
                    break;
                }
        }
        return v;
    }


    int Socket::takeFive(const int time)
    {
        NLint group = nlGroupCreate();
        nlGroupAddSocket( group, fd );
        ta3d_socket s;
        NLint v = nlPollGroup( group, NL_READ_STATUS, &s, 1, time);
        nlGroupDestroy( group );
        return v;
    }


} // namespace TA3D
