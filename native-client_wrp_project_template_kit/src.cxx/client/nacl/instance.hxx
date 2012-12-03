#pragma once

#include <array>
#include <string>
#include <memory>
//#include <functional>
#include <cmath>

#include <boost/foreach.hpp>
//#include <boost/range/algorithm.hpp>
//#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

#include "ppapi/cpp/instance.h"
//#include "ppapi/gles2/gl2ext_ppapi.h"
//#include "ppapi/cpp/rect.h"
//#include "ppapi/cpp/size.h"
#include "ppapi/cpp/var.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/audio.h"

#define TMP2(a) TMP1(a)
#define TMP1(a) #a
#define LIBWRP_PRODUCT_NAME TMP2(APP_NAME)
#include <wrp/log.hxx>
#undef TMP1
#undef TMP2

namespace BLAND_NAME {
namespace APP_NAME {
  using WonderRabbitProject::log;
namespace client {
namespace nacl {

  class instance
    : public pp::Instance
  {
    class audio {
      pp::Audio a;
      static const PP_AudioSampleRate sample_rate = PP_AUDIOSAMPLERATE_44100;
      static const uint32_t sample_frame_count_request = 4096;
      static const uint32_t channels = 2;
      double clock;
      void initialize(const pp::Instance& i){
        log l("audio::initialize", this);
        auto pi = const_cast<pp::Instance*>(&i);
        auto sample_frame_count = pp::AudioConfig::RecommendSampleFrameCount(
          pi,
          sample_rate,
          sample_frame_count_request
        );
        l << "sample_frame_count: " << sample_frame_count << "\n";
        
        const auto config = pp::AudioConfig(
          pi,
          sample_rate,
          sample_frame_count
        );
        
        a = pp::Audio(pi, config, callback, this);
      }
      static void callback(void* samples, uint32_t buffer_size, void* data){
        auto& this_ = *reinterpret_cast<audio*>(data);
        auto ps = reinterpret_cast<int16_t*>(samples);
        auto ss = std::vector<int16_t>(
          ps,
          ps + this_.channels * this_.a.config().sample_frame_count()
        );
        reinterpret_cast<audio*>(data)->callback(ss);
      }
      inline void callback(std::vector<int16_t>& samples){
        //const auto base_clock = 440.0;
        //const auto clock_scaling_unit = std::pow(2.0, 1.0 / 12.0);
        
        BOOST_FOREACH(auto& s, samples){
          s = 0;
        }
      }
    public:
      explicit audio(const pp::Instance& i)
        : clock(0.0)
      { initialize(i); }
      void play() { a.StartPlayback(); }
      void stop() { a.StopPlayback(); }
    };
    
    std::unique_ptr<audio> audio_;
    
    void initialize_audio(){
      log l("initialize_audio", this);
      audio_.reset( new audio(*this) );
      l << "audio_ reset to " << audio_;
    }
    void keyboard_event(const pp::KeyboardInputEvent& e){
      log l("keyboard_event", this);
      l << "type   : " << e.GetType() << "\n"
        << "keycode: " << e.GetKeyCode()
        ;
      switch(e.GetType()){
      case PP_INPUTEVENT_TYPE_KEYDOWN:
        ;
        break;
      case PP_INPUTEVENT_TYPE_KEYUP:
        ;
        break;
      default:
        ;
      }
    }
  public:
    explicit instance(PP_Instance a): pp::Instance(a){
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
      //initialize_audio();
      return true;
    }
    virtual void HandleMessage(const pp::Var& a){
      log l("HandleMessage", this);
      if(!a.is_string())
        return;
      auto v = a.AsString();
      l << "v: " << v;
    }
    virtual bool HandleInputEvent(const pp::InputEvent& e){
      //log l("HandleInputEvent", this);
      //l << "type: " << boost::lexical_cast<std::string>(e.GetType()) + "\n"
      //  << "time: " << boost::lexical_cast<std::string>(e.GetTimeStamp())
      //  ;
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
  };

}}}}

