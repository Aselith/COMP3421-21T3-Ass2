#include <chicken3421/chicken3421.hpp>

#include <ass2/memes.hpp>

const char *MinecraftIcon = "res/textures/jack_o_lantern.png";

/**
 * Please don't kill me marc
 * @param win The window to make GLORIOUS
 * @return a better window
 */
GLFWwindow *marcify(GLFWwindow *win) {
    chicken3421::image_t mineCoin = chicken3421::load_image(MinecraftIcon, false);

    GLFWimage favicon = {mineCoin.width, mineCoin.height, (unsigned char *) mineCoin.data};
    glfwSetWindowIcon(win, 1, &favicon);

    chicken3421::delete_image(mineCoin);

    return win;
}
