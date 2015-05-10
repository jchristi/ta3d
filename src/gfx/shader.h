#ifndef __TA3D_GFX_Shader_H__
# define __TA3D_GFX_Shader_H__

# include "../stdafx.h"


namespace TA3D
{
// namespace GFX
// {


    /*! \class Shader
    **
    ** \brief Shader
    */
    class Shader
    {
    public:
        //! \name Constructor & Destructor
        //@{
        //! Default constructor
        Shader() :pLoaded(false) {}
        //! Destructor
        ~Shader() {destroy();}
        //@}
        
        void destroy();


        /*!
        ** \brief Load a shader from files
        */
        void load(const String& pShaderFragmentFilename, const String& vertexFilename);

        /*!
        ** \brief Load a shader from a memory buffer
        */
        void load_memory(const char* pShaderFragment_data, const int frag_len, const char *vertex_data, const int vert_len);


        /*!
        ** \brief Enable the shader
        */
        void on();

        /*!
        ** \brief Disable the shader
        */
        void off();


        //! \name Variable for the ARB extension
        //@{
        void setvar1f(const char* var, const float v0);
        void setvar2f(const char* var, const float v0, const float v1);
        void setvar3f(const char* var, const float v0, const float v1, const float v2);
        void setvar4f(const char* var, const float v0, const float v1, const float v2, const float v3);
        void setvar1i(const char* var, const int v0);
        void setvar2i(const char* var, const int v0, const int v1);
        void setvar3i(const char* var, const int v0, const int v1, const int v2);
        void setvar4i(const char* var, const int v0, const int v1, const int v2, const int v3);
        //@}

        /*!
        ** \brief Get if the shader has been loaded by the previous call to `load` or `load_memory`
        **
        ** \see load()
        ** \see load_memory()
        */
        bool isLoaded() const {return pLoaded;}


    private:
        //! Is the shader loaded ?
        bool pLoaded;
        //!
        GLhandleARB		pShaderProgram;
        //!
        GLhandleARB		pShaderFragment;
        //!
        GLhandleARB		pShaderVertex;

    }; // class Shader




    // } // namespace GFX
} // namespace TA3D

#endif // __TA3D_GFX_Shader_H__
