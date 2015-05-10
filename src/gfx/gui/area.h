#ifndef __TA3D_GFX_GUI_AREA_H__
# define __TA3D_GFX_GUI_AREA_H__

# include "../../stdafx.h"
# include "../../threads/thread.h"
# include <vector>
# include "../../misc/interface.h"
# include "wnd.h"


namespace TA3D
{

    /*! \class AREA
    **
    ** \brief This class is a window handler, so it will manage windows
    ** and signals given to them
    */
    class AREA:	public ObjectSync, protected IInterface
    {
    public:
        //! \name Constructor && Destructor
        //@{
        /*!
        ** \brief Constructor
        ** \param area_name Name of the area
        */
        AREA(const String& area_name = "unnamed_area");
        //! Destructor
        ~AREA();
        //@}

        /*!
        ** \brief Reset the object
        */
        void destroy();

        /*!
        ** \brief Returns a pointer to current AREA object if one exists, otherwise returns NULL
        */
        static AREA *current();

        /*!
        ** \brief Load a window from a TDF file
        **
        ** \param filename TDF File to load
        ** \return
        */
        uint16 load_window(const String& filename);

        /*!
        ** \brief Check the user interface, manage events
        */
        uint16 check();

        /*!
        ** \brief Draw all windows
        */
        void draw();

        /*!
        ** \brief Load a TDF file, telling which windows to load and which skin to use
        **
        ** \param filename TDF File to load
        */
        void load_tdf(const String& filename);

        /*!
        ** \brief Return the specified window
        **
        ** This method is thread-safe
        **
        ** \param message
        */
        WND* get_wnd(const String& message);

        /*!
        ** \brief Return the state of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        bool get_state(const String& message);

        /*!
        ** \brief Return true if the given object has its activated flag set (mouse is on and button pressed)
        **
        ** \param message
        ** \return
        */
        bool is_activated(const String& message);

        /*!
        ** \brief Return true if the cursor is over the given object
        **
        ** \param message
        ** \return
        */
        bool is_mouse_over(const String& message);

        /*!
        ** \brief Return the value of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        sint32 get_value(const String& message);

        /*!
        ** \brief Return the caption of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        String get_caption(const String& message);

        /*!
        ** \brief Return a pointer to the specified object
        **
        ** \param message
        ** \return
        */
        GUIOBJ* get_object(const String& message, bool skip_hidden = false);

        /*!
        ** \brief Set the state of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        void set_state(const String& message, const bool state);

        /*!
        ** \brief Set the value of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        void set_value(const String& message, const sint32 value);

        /*!
        ** \brief Set the data of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        void set_data(const String& message, const sint32 data);

        /*!
        ** \brief Set the enabled/disabled state of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        void set_enable_flag(const String& message, const bool enable);

        /*!
        ** \brief Set the caption of specified object in the specified window
        **
        ** \param message
        ** \return
        */
        void set_caption(const String& message, const String& caption);

        /*!
        ** \brief Set the title of specified window to title
        **
        ** \param message
        ** \return
        */
        void set_title(const String& message, const String& title);

        /*!
        ** \brief Set the entry of specified object in the specified window to entry
        **
        ** \param message
        ** \return
        */
        void set_entry(const String& message, const std::list<String>& entry);
        void set_entry(const String& message, const std::vector<String>& entry);

        /*!
        ** \brief Set the function pointer of an object, it will be called when the widget is clicked
        **
        ** \param message
        ** \return
        */
        void set_action(const String& message, void (*Func)(int));

        /*!
        ** \brief Send that message to the area
        **
        ** \param message
        ** \return
        */
        int	msg(String message);


    public:
        //!
        bool scrolling;
        //! Of course we need a background, not a single color :-)
        GLuint background;

    private:
        /*!
        ** \brief Manage signals sent through the interface to GUI
        **
        ** \param msg
        ** \return
        */
        virtual uint32 InterfaceMsg(const lpcImsg msg);

        /*!
        ** \brief Same as get_object, but not thread-safe
        ** \see get_object()
        */
        GUIOBJ* doGetObject(const String& message);

        /*!
        ** \brief Same as get_wnd, but not thread-safe
        ** \see get_wnd()
        */
        WND* doGetWnd(const String& message);

        /*!
        ** \brief Same as load_tdf(), but not thread-safe
        */
        void doLoadTDF(const String& filename);

    private:
        //! This list stores the stack of all AREA objects so you can grab the current one at any time
        static std::list<AREA*> area_stack;

        //! This vector stores all the windows the area object deals with
        std::vector<WND*>  vec_wnd;
        //! This vector stores data about the z order of windows
        std::vector<uint16> vec_z_order;

        //! Name of the ares
        String name;

        //! The last cursor x-position
        int amx;
        //! The last cursor y-position
        int amy;
        //! The last cursor z-position
        int amz;
        //! The last cursor button status
        int amb;

        //! The skin used by the ares
        SKIN* skin;

        //! hashtable used to speed up loading of *.gui files and save memory
        TA3D::UTILS::cHashTable< std::vector< TA3D::Interfaces::GfxTexture >* > gui_hashtable;
        //! hashtable used to speed up operations on WND objects
        TA3D::UTILS::cHashTable<int>  wnd_hashtable;

        //!
        String  cached_key;
        //!
        WND* cached_wnd;

        //!
        uint32  scroll_timer;

    }; // class AREA


} // namespace TA3D

#endif // __TA3D_GFX_GUI_AREA_H__
