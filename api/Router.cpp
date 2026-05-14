#include "Router.hpp"
#include "../endpoints/IController.hpp"
#include <cstring>

Router& Router::instance() {
    static Router inst;
    return inst;
}
