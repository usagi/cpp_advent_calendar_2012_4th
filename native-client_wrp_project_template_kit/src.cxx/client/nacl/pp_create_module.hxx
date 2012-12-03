#pragma once

#include "ppapi/cpp/module.h"

namespace pp {
  Module* CreateModule()
  { return new BLAND_NAME::APP_NAME::client::nacl::module(); }
}

