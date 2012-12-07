#pragma once

#include <cmath>
#include <boost/foreach.hpp>
#include <boost/math/constants/constants.hpp>
#include <boost/operators.hpp>
//#include <boost/gil/color_convert.hpp>

//#include <eigen3/Eigen/Core>

namespace BLAND_NAME {
namespace APP_NAME {
  class particle_system;
  class particle;
  class vector2;
  class color_type;
  class pixel;
}}

namespace {
  const double pi = boost::math::constants::pi<double>(); 
  const double pi_pro_2 = pi * 2.;
  const double pi_div_3 = pi / 3.;
  const double pi_pro_2_div_3 = pi * (2. / 3.);
  const double pi_pro_4_div_3 = pi * (4. / 3.);

  const double c_gravity                 =   9.80655;
  const double c_wall_friction           =    .80;
  const double c_luminance               =   3.;
  const double c_radius_enhancement      =  32.;
  
  const double num_of_initial_particles  = 256.;
  const double generator_timer_time      =    .025;
  
  //const double particle_mass             =  10.;
  //const double particle_radius           =  40.;
  const double particle_initial_velocity = 128.;
};

namespace BLAND_NAME {
namespace APP_NAME {

class particle_system {
public:

  struct color_type{
    double r, g, b;
  public:
    color_type& operator+=(const color_type& t)
    { r += t.r; g += t.g; b += t.b; return *this; }
    
    color_type operator*(const double v)
    { return { r * v, g * v, b * v }; }
    
    static color_type black() { return {.0, .0, .0}; }
    
    static color_type from_HSL(double h = .0, double s = 1., double l = .5){
      
      while(h < .0)
        h += pi_pro_2;
      while(pi_pro_2 < h)
        h -= pi_pro_2;
      
      if(s == .0)
        return {l, l, l};
      
      auto m2 = (l < .5) ? l * (1. + s) : l + s -l * s;
      auto m1 = l * 2. - m2;
      
      auto hr = h + pi_pro_2_div_3;
      if(pi_pro_2 < hr)
        hr -= pi_pro_2;
      
      auto hb = h - pi_pro_2_div_3;
      if(hb < .0)
        hb += pi_pro_2;
      
      return {
        from_HSL_sub(hr, m2, m1),
        from_HSL_sub(h , m2, m1),
        from_HSL_sub(hb, m2, m1)
      };
    }
  private:
    static double from_HSL_sub(double tmp, double m2, double m1){
      if(tmp < pi_div_3)
        return (m1 + (m2 - m1) * tmp / pi_div_3);
      else if(tmp < pi)
        return m2;
      else if(tmp < pi_pro_4_div_3)
        return m1 + (m2 - m1) * (pi_pro_4_div_3 - tmp) / pi_div_3;
      return m1;
    }
  };
  
  struct vector2: private boost::operators<vector2> {
    double x, y;
    
    vector2(): x(.0), y(.0) {}
    
    vector2(double&& x_, double&& y_)
      : x(std::move(x_)), y(std::move(y_)) { }
    
    vector2(vector2&& v)
      : x(std::move(v.x)), y(std::move(v.y)) { }
    
    vector2& operator+=(const vector2& t)
    { x += t.x; y += t.y; return *this; }
    
    vector2& operator-=(const vector2& t)
    { x -= t.x; y -= t.y; return *this; }
    
    vector2& operator*=(const vector2& t)
    { x *= t.x; y *= t.y; return *this; }
    
    vector2& operator/=(const vector2& t)
    { x /= t.x; y /= t.y; return *this; }
    
    //vector2 operator-(const vector2& t)
    //{ return {x - t.x, y - t.y}; }
    
    vector2 operator*(const double v)
    { return {x * v, y * v}; }
    
    double norm() const
    { return std::sqrt(x * x + y * y); }
  };
  
  class particle{
    friend class particle_system;
    
    //double mass;
    //double radius;
    color_type color;
    vector2 position;
    vector2 velocity;
  
  public:
    particle(){}
    
    particle(
      //const double mass_,
      //const double radius_,
      const color_type& color_,
      const vector2& position_,
      const vector2& velocity_
    ) :
      //mass(mass_),
      //radius(radius_),
      color(color_),
      position(position_),
      velocity(velocity_)
    {}
    
    void update(const double dt){
      velocity.y -= dt * c_gravity;
      position += velocity * dt;
    }
    
    vector2 get_position() const
    { return position; }
    
    color_type get_color() const
    { return color; }
    
    //double get_radius() const
    //{ return radius; }
  };
  
  class pixel{
    friend class particle_system;
    
    vector2    position;
    color_type color;
    
  public:
    pixel(
      vector2&&    position_,
      color_type&& color_ = color_type::black()
    ) :
      position(std::move(position_)),
      color(std::move(color_))
    {}
    
    const color_type& get_color() const
    { return color;}
    
    void clear()
    { color = color_type::black(); }
  };
  
  typedef std::vector<std::vector<pixel>> pixels_type;
  typedef std::vector<particle>           particles_type;
  
private: 
  vector2 screen_resolution;
  vector2 screen_translation;
  vector2 world_resolution;
  vector2 pixel_size;
  vector2 pixel_center_shift;
  
  particles_type particles;
  pixels_type pixels;
  
  double total_time_elapsed;
  
  decltype(std::chrono::system_clock::now()) last_update_time;
  
  vector2 to_screen_coordinates(vector2&& v){
    return {
      screen_translation.x + v.x,
      screen_translation.y - v.y
    };
  }
public:
  particle_system(
    const vector2& screen_resolution_,
    const vector2& world_resolution_
  ) :
    total_time_elapsed(0),
    last_update_time(std::chrono::system_clock::now())
  {
    particles.reserve(num_of_initial_particles);
    change_resolution(screen_resolution_, world_resolution_);
  }
  
  void change_resolution(const vector2& screen_resolution_, const vector2& world_resolution_) {
    screen_resolution = screen_resolution_;
    world_resolution  = world_resolution_;
    screen_translation = vector2(screen_resolution_) * .5;
    pixel_size = screen_resolution / world_resolution;
    pixel_center_shift = pixel_size * 0.5;
    
    pixels.clear();
    pixels.resize(world_resolution.x);
    for(size_t x = 0; x < world_resolution.x; ++x){
      pixels[x].reserve(world_resolution.y);
      for(size_t y = 0; y < world_resolution.y; ++y)
        pixels[x].push_back(vector2({
          static_cast<double>(x),
          static_cast<double>(y)
        }));
    }
  }
  void initial_particle_generator(){
    const auto pss = particles.size();
    if( (pss < num_of_initial_particles)
      && (pss < total_time_elapsed / generator_timer_time + 1)
    ){
      auto r = static_cast<double>(pss) / num_of_initial_particles;
      auto t = boost::math::constants::pi<double>() * 2 * r;
      auto tr = t * 7;
      particles.push_back(
        particle(
          //particle_mass,
          //particle_radius,
          color_type::from_HSL(t),
          { 0, 0 },
          vector2({std::cos(tr), std::sin(tr)}) * particle_initial_velocity
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
    
    update_particles(dt);
    update_pixels();
  }

  inline void update_particles(const double dt){
    BOOST_FOREACH(auto& particle, particles){
      particle.update(dt);
      auto abs_px = std::abs(particle.position.x);
      if(abs_px > screen_translation.x){
        particle.position.x -= (abs_px - screen_translation.x) * particle.position.x / abs_px;
        particle.velocity.x *= -c_wall_friction;
      }
      auto abs_py = std::abs(particle.position.y);
      if(abs_py > screen_translation.y){
        particle.position.y -= (abs_py - screen_translation.y) * particle.position.y / abs_py;
        particle.velocity.y *= -c_wall_friction;
      }
    }
  }
  
  inline void update_pixels(){
    BOOST_FOREACH(auto& col, pixels)
      BOOST_FOREACH(auto& pixel, col) {
        pixel.clear();
        BOOST_FOREACH(auto& particle, particles){
          auto pps = to_screen_coordinates(particle.position);
          auto d = pixel.position * pixel_size + pixel_center_shift - pps;
          auto k = c_luminance / (d.norm() + 1);
          pixel.color += particle.color * k;
        }
      }
    
  }
  
  const pixels_type& get_pixels() const{
    return pixels;
  }
  const double get_total_time_elapsed() const{
    return total_time_elapsed;
  }
};

}}

