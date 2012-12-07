#pragma once

#include <array>
#include <string>
#include <memory>
//#include <functional>
#include <chrono>

#include <boost/foreach.hpp>
//#include <boost/range/algorithm.hpp>
//#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

#include "ppapi/cpp/instance.h"
//#include "ppapi/gles2/gl2ext_ppapi.h"
//#include "ppapi/cpp/rect.h"
//#include "ppapi/cpp/size.h"
#include "ppapi/cpp/var.h"
//#include "ppapi/cpp/var_array_buffer.h"
#include "ppapi/cpp/input_event.h"
//#include "ppapi/cpp/audio.h"

#define TMP2(a) TMP1(a)
#define TMP1(a) #a
#define LIBWRP_PRODUCT_NAME TMP2(APP_NAME)
#define WRP_NO_LOG
#include <wrp/log.hxx>
#undef TMP1
#undef TMP2

#include <picojson/picojson.h>

#include "particle_system.hxx"

namespace BLAND_NAME {
namespace APP_NAME {
  using WonderRabbitProject::log;
namespace client {
namespace nacl {

  class instance
    : public pp::Instance
  {
    particle_system ps;
    
    void keyboard_event(const pp::KeyboardInputEvent& e){
      log l("keyboard_event", this);
      l << "type   : " << e.GetType() << "\n"
        << "keycode: " << e.GetKeyCode()
        ;
      switch(e.GetType()){
      case PP_INPUTEVENT_TYPE_KEYDOWN:
        break;
      case PP_INPUTEVENT_TYPE_KEYUP:
        break;
      default:
        ;
      }
    }
    
  public:
    explicit instance(
      PP_Instance a
    ):
      pp::Instance(a),
      ps(particle_system({1920,1080}, {160,90}))
    {
      log l("instance", this);
      RequestInputEvents(
        PP_INPUTEVENT_CLASS_MOUSE |
        PP_INPUTEVENT_CLASS_WHEEL
      );
      RequestFilteringInputEvents(
        PP_INPUTEVENT_CLASS_KEYBOARD
      );
    }
    virtual ~instance(){}
    virtual bool Init(uint32_t argc, const char* argn[], const char* argv[]){
      log l("Init", this);
      for(decltype(argc) n = 0; n < argc; ++n){
        l << "attribute#" << boost::lexical_cast<std::string>(n) << "\n"
          << "name: " << argn[n] <<"\n"
          << "value: " << argv[n] 
          ;
      }
      return true;
    }
    
    virtual void HandleMessage(const pp::Var& a){
      log l("HandleMessage Var", this);
      
      if(!a.is_string())
        return;
      
      auto v = a.AsString();
      l << "v: " << v;
      picojson::value j;
      std::stringstream ss;
      ss << v;
      ss >> j;
      //auto j = picojson::value(v);
      if(!picojson::get_last_error().empty() || !j.is<picojson::object>()){
        l << "json parse error";
        return;
      }
      
      auto& o = j.get<picojson::object>();
      const auto& command = o["command"].get<std::string>();
      l << "command: " << command;
      if(command == "update")
        update();
      else if(command == "change_resolution"){
        auto oi = o["params"].get<picojson::array>().cbegin();
        std::array<double, 4> p;
        for(auto pi = p.begin(); pi != p.end(); ++pi, ++oi)
          *pi = oi->get<double>();
        ps.change_resolution({p[0], p[1]}, {p[2], p[3]});
      }
    }
    
    virtual bool HandleInputEvent(const pp::InputEvent& e){
      log l("HandleInputEvent", this);
      l << "type: " << boost::lexical_cast<std::string>(e.GetType()) + "\n"
        << "time: " << boost::lexical_cast<std::string>(e.GetTimeStamp())
        ;
      switch(e.GetType()){
      case PP_INPUTEVENT_TYPE_KEYDOWN:
      case PP_INPUTEVENT_TYPE_KEYUP:
        keyboard_event(pp::KeyboardInputEvent(e));
        break;
      default:
        ;
      }
      return pp::Instance::HandleInputEvent(e);
    }
    virtual void DidChangeView(const pp::View& view){
      log l("DidChangeView(const pp::View&)", this);
      // ToDo: resize view
      pp::Instance::DidChangeView(view);
    }
    virtual void DidChangeView(const pp::Rect& position, const pp::Rect& clip){
      log l("DidChangeView(const pp::Rect&, const pp::Rect&)", this);
      // ToDo: resize view
    }
    virtual void DidChangeFocus(bool focus){
      log l("DidChangeFocus", this);
      l << "focus: " << focus;
      // ToDo: 
    }
    void update(){
      log l("update", this);
      
      ps.update();
      
      std::stringstream ss;
      ss << "{\"type\":\"updated\",\"data\":[";
      const auto& pixels = ps.get_pixels();
      BOOST_FOREACH(const auto& col, pixels){
        BOOST_FOREACH(const auto& p, col){
          ss << "[" << std::min(255u, std::max(0u, static_cast<unsigned>(p.get_color().r * 255))) << ","
                    << std::min(255u, std::max(0u, static_cast<unsigned>(p.get_color().g * 255))) << ","
                    << std::min(255u, std::max(0u, static_cast<unsigned>(p.get_color().b * 255))) << "],";
        }
      }
      const auto& str = ss.str();
      auto reply = std::string(str.begin(), str.end() - 1) + "]}";
      l << reply;
      PostMessage(pp::Var(reply));
    }
  };
  
}}}}

