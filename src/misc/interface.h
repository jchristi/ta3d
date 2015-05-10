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

#ifndef __TA3D_I_INTERFACES_H__
# define __TA3D_I_INTERFACES_H__

# include "../threads/thread.h"


# define I_Msg( x, xx,xxx,xxxx ) InterfaceManager->DispatchMsg( x,xx,xxx,xxxx )
# define I_sMsg( x ) InterfaceManager->DispatchMsg( x )
# define GUI_Msg( x ) InterfaceManager->DispatchMsg( TA3D_IM_GUI_MSG, (x).c_str(), NULL, NULL )


namespace TA3D
{

	class IInterfaceManager;
	class IInterface;



	enum TA3D_INTERFACE_MESSAGES
	{
		TA3D_IM_DEBUG_MSG,
		TA3D_IM_GFX_MSG,
		TA3D_IM_GUI_MSG
	};

	enum INTERFACE_RESULT
	{
		INTERFACE_RESULT_HANDLED  = 0,
		INTERFACE_RESULT_CONTINUE = 1
	//	INTERFACE_RESULT_IGNORE_THIS_MSG =2
	};

	typedef struct IInterfaceMessage
	{
		IInterfaceMessage(const uint32 mid, void *a, void *b, void *c )
            :MsgID(mid), lpParm1(a), lpParm2(b), lpParm3(c)
		{}

		const uint32 MsgID;

		void* lpParm1;
		void* lpParm2;
		void* lpParm3;
	} cIMsg, *lpcImsg;



	class IInterface
	{
		friend class IInterfaceManager;

	public:
		virtual ~IInterface() {}

	protected:
		void InitInterface();
		void DeleteInterface();

	private:
		uint32 m_InterfaceID;

        /*!
        ** \brief Callback to manage  broadcasted messages
        ** \param msg The received message
        ** \return The return status
        */
		virtual uint32 InterfaceMsg(const lpcImsg msg) = 0;

	}; // class IInterface





	class IInterfaceManager : public ObjectSync
	{
	public:
        //! \name Constructor & destructor
        //@{
        //! Default constructor
        IInterfaceManager();
        //! Destructor
		~IInterfaceManager();
        //@}

        /*!
        ** \brief Dispatch a message (from its ID) to all registered interfaces
        **
        ** The broadcast of the message when an interface returns
        ** `INTERFACE_RESULT_HANDLED`.
        **
        ** \param mID ID of the message
        ** \param a Parameter 1
        ** \param b Parameter 2
        ** \param c Parameter 3
        **
        ** \see IInterface::InterfaceMsg()
        */
		void DispatchMsg(const uint32 mID, void* a, void* b, void* c);

	private:
        /*!
        ** \brief Add an interface in the list
        ** \param i Interface to add
        ** \warning The parameter `i` must not be null
        */
		void AddInterface(IInterface* i);

        /*!
        ** \brief Remove an interface from the list
        ** \param i Interface to remove
        ** \warning The parameter `i` must not be null
        */
		void RemoveInterface(IInterface* i);

        /*!
        ** \brief Dispatch message to all registered interfaces
        **
        ** The broadcast of the message when an interface returns
        ** `INTERFACE_RESULT_HANDLED`.
        **
        ** \param msg Message to broadcast
        **
        ** \see IInterface::InterfaceMsg()
        */
		void DispatchMsg(const lpcImsg msg);

    private:
		friend class IInterface;
        typedef std::vector<IInterface*> InterfacesList;

	private:
		InterfacesList pInterfaces;
		uint32 pNextInterfaceID;

	}; // class IInterfaceManager
}


#endif // __TA3D_I_INTERFACES_H__
