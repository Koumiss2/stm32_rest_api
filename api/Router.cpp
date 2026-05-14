#include "Router.hpp"
#include "IController.hpp"
#include <cstring>

Router& Router::instance() {
    static Router inst;
    return inst;
}
