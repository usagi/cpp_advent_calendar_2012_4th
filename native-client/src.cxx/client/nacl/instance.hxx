#pragma once

#include <array>
#include <string>
#include <memory>
//#include <functional>
#include <chrono>
#include <cmath>

#include <boost/foreach.hpp>
//#include <boost/range/algorithm.hpp>
//#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/math/constants/constants.hpp>
//#include <boost/operators.hpp>
//#include <boost/gil/color_convert.hpp>

//#include <eigen3/Eigen/Core>

#include "ppapi/cpp/instance.h"
//#include "ppapi/gles2/gl2ext_ppapi.h"
//#include "ppapi/cpp/rect.h"
//#include "ppapi/cpp/size.h"
#include "ppapi/cpp/var.h"
//#include "ppapi/cpp/var_array_buffer.h"
#include "ppapi/cpp/input_event.h"
#include "ppapi/cpp/audio.h"

#define TMP2(a) TMP1(a)
#define TMP1(a) #a
#define LIBWRP_PRODUCT_NAME TMP2(APP_NAME)
#define WRP_NO_LOG
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
    
    class particle_system {
    public:
      
      struct color_type{
        double r,g,b;
      private:
        static const double p_p_2;
        static const double p_d_3;
        static const double p_p_2_d_3;
        static const double p_p_4_d_3;
      public:
        color_type& operator+=(const color_type& t)
        { r += t.r; g += t.g; b += t.b; return *this; }
        
        color_type operator*(const double v)
        { return { r * v, g * v, b * v }; }
        
        static color_type from_HSL(double h, double s, double l){

          while(h < 0)
            h += p_p_2;
          while(p_p_2 < h)
            h -= p_p_2;

          if(s == 0)
            return {l,l,l};

          auto m2 = (l < 0.5) ? l * (1 + s) : l + s -l * s;
          auto m1 = l * 2 - m2;
          
          auto hr = h + p_p_2_d_3;
          if(p_p_2 < hr)
            hr -= p_p_2;
          auto hb = h - p_p_2_d_3;
          if(hb < 0)
            hb += p_p_2;

          return { from_HSL_sub(hr,m2,m1), from_HSL_sub(h,m2,m1), from_HSL_sub(hb, m2, m1) };
        }
      private:
        static double from_HSL_sub(double tmp, double m2, double m1){
          if(tmp < p_d_3)
            return (m1 + (m2 - m1) * tmp / p_d_3);
          else if(tmp < boost::math::constants::pi<double>())
            return m2;
          else if(tmp < p_p_4_d_3)
            return m1 + (m2 - m1) * (p_p_4_d_3 - tmp) / p_d_3;
          return m1;
        }
      };
      
      struct vector2{
        double x,y;
        vector2& operator+=(const vector2& t)
        { x += t.x; y += t.y; return *this;}
        vector2& operator-=(const vector2& t)
        { x -= t.x; y -= t.y; return *this;}
        vector2 operator-(const vector2& t)
        { return {x - t.x, y - t.y}; }
        vector2 operator*(const double v)
        { return {x * v, y * v}; }
        double norm()
        { return std::sqrt(x * x + y * y); }
        vector2 to_screen_coordinates(){
          return {
            particle_system::screen_translation_x + x,
            particle_system::screen_translation_y - y
          };
        }
      };
      
      class particle{
        double mass;
        double radius;
        color_type color;
        vector2 position;
        vector2 velocity;
      
      public:
        particle(){}
        
        particle(
          const double mass_,
          const double radius_,
          const color_type& color_,
          const vector2& position_,
          const vector2& velocity_
        ) :
          mass(mass_),
          radius(radius_),
          color(color_),
          position(position_),
          velocity(velocity_)
        {}
        void update(const double dt){
          velocity.y -= dt * particle_system::c_gravity;
          position += velocity * dt;
          
          auto abs_px = std::abs(position.x);
          if(abs_px > particle_system::screen_translation_x){
            position.x -= (abs_px - particle_system::screen_translation_x) * position.x / abs_px;
            velocity.x *= -particle_system::c_wall_friction;
          }
          auto abs_py = std::abs(position.y);
          if(abs_py > particle_system::screen_translation_y){
            position.y -= (abs_py - particle_system::screen_translation_y) * position.y / abs_py;
            velocity.y *= -particle_system::c_wall_friction;
          }
        }
        const vector2& get_position() const
        { return position; }
        const color_type& get_color() const
        { return color; }
        double get_radius() const
        { return radius; }
      };
      
      class pixel{
        vector2 position;
        color_type color;
      public:
        pixel(){}
        pixel(const vector2&& position_, const color_type&& color_)
          : position(std::move(position_)), color(std::move(color_))
        {}
        void apply_from(const particle& p){
          auto pps = vector2(p.get_position()).to_screen_coordinates();
          //auto prn = p.get_radius() * particle_system::c_radius_enhancement;
          auto w = particle_system::screen_width  / 160; 
          auto h = particle_system::screen_height /  90; 
          auto wd2 = w * 0.5;
          auto hd2 = h * 0.5;
          auto d = vector2({
            (position.x * w + wd2) - pps.x,
            (position.y * h + hd2) - pps.y
          });
          auto l = std::sqrt(d.x * d.x + d.y * d.y);
          auto k = particle_system::c_luminance / (l + 1);
          color += color_type(p.get_color()) * k;
        }
        const color_type& get_color() const
        { return color;}
        void clear()
        { color = {0,0,0}; }
      };
      
      typedef std::vector<std::vector<pixel>> pixels_type;
      std::vector<particle> particles;
      static const double c_gravity                = 9.80655;
      static const double c_wall_friction          = 0.80;
      static const double c_luminance              = 3.0;
      static const double c_radius_enhancement     = 32.0;
      static const double num_of_initial_particles = 256;
      static const double generator_timer_time     = 0.025;
      static const double particle_mass            = 10;
      static const double particle_radius          = 40;
      static const size_t screen_width  = 1920;
      static const size_t screen_height = 1080;
      static const size_t screen_translation_x = screen_width  >> 1;
      static const size_t screen_translation_y = screen_height >> 1;
    private:
      pixels_type pixels;
      double total_time_elapsed;
      vector2 pixel_size;
      decltype(std::chrono::system_clock::now()) last_update_time;
    public:
      particle_system(const size_t x_resolution, const size_t y_resolution)
        : total_time_elapsed(0)
        , last_update_time(std::chrono::system_clock::now())
      {
        particles.reserve(num_of_initial_particles);
        change_screen_resolution(x_resolution, y_resolution);
      }
      void change_screen_resolution(const size_t x_resolution, const size_t y_resolution) {
        pixel_size = {
          static_cast<double>(screen_width)  / x_resolution,
          static_cast<double>(screen_height) / y_resolution
        };
        pixels.resize(x_resolution);
        for(size_t x = 0; x < x_resolution; ++x){
          pixels[x].resize(y_resolution);
          for(size_t y = 0; y < y_resolution; ++y)
            pixels[x][y] = pixel(
              {static_cast<double>(x), static_cast<double>(y)},
              color_type()
            );
        }
      }
      void initial_particle_generator(){
        log l("initial_particle_generator", this);
        const auto pss = particles.size();
        if( (pss < num_of_initial_particles)
          && (pss < total_time_elapsed / generator_timer_time + 1)
        ){
          l << "(!) generate " << (pss + 1) << " / " << num_of_initial_particles << "\n"
            "ratio : " << (total_time_elapsed / generator_timer_time + 1);
          auto r = static_cast<double>(pss) / num_of_initial_particles;
          auto t = boost::math::constants::pi<double>() * 2 * r;
          auto tr = t * 7;
          static const auto v0 = 128;
          particles.push_back(
            particle(
              particle_mass,
              particle_radius,
              color_type::from_HSL(t, 1.0, 0.5),
              { 0, 0 },
              { std::cos(tr) * v0, std::sin(tr) * v0 }
            )
          );
        }
      }
      void update(){
        auto now = std::chrono::system_clock::now();
        typedef std::chrono::duration<double> unit;
        auto dt = std::chrono::duration_cast<unit>(now - last_update_time).count();
        last_update_time = now;
        total_time_elapsed += dt;
        initial_particle_generator();
        
        BOOST_FOREACH(auto& col, pixels)
          BOOST_FOREACH(auto& pixel, col)
            pixel.clear();
        
        BOOST_FOREACH(auto& particle, particles){
          particle.update(dt);
          BOOST_FOREACH(auto& col, pixels)
            BOOST_FOREACH(auto& pixel, col)
              pixel.apply_from(particle);
        }
      }
      
      const pixels_type& get_pixels() const{
        return pixels;
      }
      const double get_total_time_elapsed() const{
        return total_time_elapsed;
      }
    };
    
    void keyboard_event(const pp::KeyboardInputEvent& e){
      log l("keyboard_event", this);
      l << "type   : " << e.GetType() << "\n"
        << "keycode: " << e.GetKeyCode()
        ;
      switch(e.GetType()){
      case PP_INPUTEVENT_TYPE_KEYDOWN:
        break;
      case PP_INPUTEVENT_TYPE_KEYUP:
        test();
        break;
      default:
        ;
      }
    }
    
    void test(){
      log l("test", this);
      //ps.update(0.005);
      //l << "time: " << ps.get_total_time_elapsed() << "\n";
      auto pixels = ps.get_pixels();
      BOOST_FOREACH(const auto& col, pixels){
        BOOST_FOREACH(const auto& p, col){
          l << "(" << p.get_color().r << ","
                   << p.get_color().g << ","
                   << p.get_color().b << ") ";
      }
      }
    }
    
    
    particle_system ps;
    
  public:
    explicit instance(PP_Instance a)
      : pp::Instance(a), ps(particle_system(160, 90))
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
      if(v == "update"){
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
  
const double instance::particle_system::color_type::p_p_2 = boost::math::constants::pi<double>() * 2;
const double instance::particle_system::color_type::p_d_3 = boost::math::constants::pi<double>() / 3;
const double instance::particle_system::color_type::p_p_2_d_3 = color_type::p_p_2 / 3;
const double instance::particle_system::color_type::p_p_4_d_3 = color_type::p_p_2_d_3 * 2;

}}}}

