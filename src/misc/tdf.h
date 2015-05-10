#ifndef __TA3D_XX__TDF_H__
# define __TA3D_XX__TDF_H__

# include "../stdafx.h"
# include "string.h"
# include "hash_table.h"
# include <stack>


namespace TA3D
{


    /*! \class TDFParser
    **
    ** \brief
    **
    ** This class is not thread-safe.
    */
    class TDFParser
    {
    public:
        //! \name Constructors & Destructor
        //@{

        //! Constructor by default
        TDFParser();

        /*!
        ** \brief Contructor
        **
        ** \param filename The absolute filename to load
        ** \param caSensitive Get if all keys are case sensitive or not
        ** \param toUTF8 Convert this buffer from ASCII to the UTF8 charset
        ** \param gadgetMode Mystic mode
        **
        ** \see loadFromFile()
        */
        explicit TDFParser(const String& filename, const bool caSensitive = false, const bool toUTF8 = false,
                  const bool gadgetMode = false, const bool realFS = false);

        //! Destructor
        ~TDFParser();

        //@} Constructors & Destructor

        /*!
        ** \brief Clear all stored keys
        */
        void clear();


        //! \name Import Data
        //@{

        /*!
        ** \brief Load data from a file
        **
        ** \param filename The absolute filename to load
        ** \param clear Clear the all previoulsy stored keys
        ** \param toUTF8 Convert this buffer from ASCII to the UTF8 charset
        ** \param gadgetMode Mystic mode
        ** \return True if the operation succeeded, False otherwise
        */
        bool loadFromFile(const String& filename, const bool clear = false, const bool toUTF8 = false, const bool gadgetMode = false, const bool realFS = false);

        /*!
        ** \brief Load data from a buffer
        ** 
        ** \param caption The caption of this buffer to display when a parse error is encountered
        ** \param data The buffer
        ** \param size Size of the buffer
        ** \param clear Clear the all previoulsy stored keys
        ** \param toUTF8 Convert this buffer from ASCII to the UTF8 charset
        ** \param gadgetMode Mystic mode
        ** \return True if the operation succeeded, False otherwise
        */
        bool loadFromMemory(const String& caption, const char* data, uint64 size, const bool clear = false,
                            const bool toUTF8 = false, const bool gadgetMode = false);

        /*!
        ** \brief Load data from a buffer
        ** 
        ** This method will find the end of the buffer with the first '\0' in the buffer
        **
        ** \param caption The caption of this buffer to display when a parse error is encountered
        ** \param data The buffer
        ** \param clear Clear the all previoulsy stored keys
        ** \param toUTF8 Convert this buffer from ASCII to the UTF8 charset
        ** \param gadgetMode Mystic mode
        ** \return True if the operation succeeded, False otherwise
        */
 
        bool loadFromMemory(const String& caption, const char* data, const bool clearTable = false,
                            const bool toUTF8 = false, const bool gadgetMode = false);

        //@}


        //! \name Keys & Values
        //@{

        /*!
        ** \brief Get the value for a given key
        ** \param key The key
        ** \param def The default value if the key could not be found
        ** \return The value of the key that has been found, def otherwise
        */
        sint32  pullAsInt(const String& key, const sint32 def = 0);

        /*!
        ** \brief Get the value for a given key
        ** \param key The key
        ** \param def The default value if the key could not be found
        ** \return The value of the key that has been found, def otherwise
        */
        real32  pullAsFloat(const String& key, const real32 def = 0.0f);

        /*!
        ** \brief Get the value for a given key
        ** \param key The key
        ** \param def The default value if the key could not be found
        ** \return The value of the key that has been found, def otherwise
        */
        String pullAsString(const String& key, const String& def = "");

        /*!
        ** \brief Get the value for a given key
        ** \param key The key
        ** \param def The default value if the key could not be found
        ** \return The value of the key that has been found, def otherwise
        */
        bool pullAsBool(const String& key, const bool def = false);

        /*!
        ** \brief Insert or update the value of a key
        ** \param key The key
        ** \param value The new value of the key
        */
        void insertOrUpdate(const String& key, const String& value) {pTable.insertOrUpdate(key, value);}
 
        /*!
        ** \brief Remove a key if exists
        ** \param key The key to remove
        */
        void remove(const String& key) {pTable.remove(key);}
        
        bool exists(const String& key) {return pTable.exists(key);}

        /*!
        ** \brief Call a callback for each key
        ** \param callback The callback
        **
        ** \code
        ** class Predicate
        ** {
        ** public:
        **     bool operator () (const String& key, const String& value)
        **     {
        **         std::cout << "Key: " << key << ", value: " << value << std::endl;
        **         return true; // False to stop the process
        **     }
        ** };
        **
        ** int main(void)
        ** {
        **     TA3D::TDFParser p;
        **     p.loadFromFile("gui/mdrn_save_menu.tdf");
        **     p.forEach(Predicate());
        **     return 0;
        ** }
        ** \endcode
        */
        template<typename C>
        void forEach(C callback) {pTable.forEach(callback);}

        //@}


    private:
        //! The hash table
        TA3D::UTILS::cHashTable<String> pTable;
        //! The size of the table
        uint16 pTableSize;
        //! Get if the table is empty
        bool pTableIsEmpty;

        //! CharCase
        bool pIgnoreCase;

    }; // class TDFParser


} // namespace TA3D



#endif // __TA3D_XX__TDF_H__
