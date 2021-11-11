#ifndef TEXTURE2D_H
#define TEXTURE2D_H

#include <glad/glad.h>
#include <string>

namespace texture_2d {

    struct params_t {
        GLint wrap_s = GL_REPEAT; // wrapping mode on S axis
        GLint wrap_t = GL_REPEAT; // wrapping mode on T axis
        GLint filter_min = GL_LINEAR_MIPMAP_LINEAR; // filtering mode if texture pixels < screen pixels
        GLint filter_max = GL_LINEAR; // filtering mode if texture pixels > screen pixels
    };

    /**
     * @brief Creates a map with the given filename and returns the handler for the newly created texture
     * 
     * @param fileName 
     * @param params 
     * @return GLuint 
     */
    GLuint init(std::string fileName, params_t const &params = params_t{GL_REPEAT, GL_REPEAT, GL_NEAREST, GL_NEAREST});

    /**
     * @brief Destroys the given texture
     * 
     * @param tex 
     */
    void destroy(GLuint tex);
}

#endif
