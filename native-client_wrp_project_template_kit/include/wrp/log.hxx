#include <iostream>
#include <string>
#include <sstream>
#include <chrono>

#include <boost/range/algorithm.hpp>

namespace WonderRabbitProject { 

#if !defined(LIBWRP_PRODUCT_NAME)
  #define LIBWRP_PRODUCT_NAME "UNKNOWN PRODUCT"
#endif

  class log{
  private:
#ifndef WRP_NO_LOG
    const decltype(std::chrono::system_clock::now()) start;
    const std::string object_name;
    const void* object_address;
    std::string indent;
    static int nesting_counter;
#endif
  public:
#ifndef WRP_NO_LOG
    typedef std::chrono::duration<double> unit;
#endif
    std::stringstream buffer;
    explicit log(const std::string object_name_ = "", const void* object_address_ = 0)
#ifndef WRP_NO_LOG
      : start(std::chrono::system_clock::now())
      , object_name(object_name_)
      , object_address(object_address_)
#endif
    {
#ifndef WRP_NO_LOG
      ++nesting_counter;
      indent.resize(2 * nesting_counter);
      boost::fill(indent, ' ');
      std::cout
        << indent << "[" LIBWRP_PRODUCT_NAME "] "
                  << object_name << " " << object_address << "\n"
        << indent << "start: "
                  << std::chrono::duration_cast<unit>(start.time_since_epoch()).count()
        << std::endl
        ;
#endif
    }
#ifndef WRP_NO_LOG
    ~log(){
      auto end = std::chrono::system_clock::now();
      std::string b;
      while(std::getline(buffer, b))
        std::cout << indent << b << "\n";
      std::cout
        << indent << "end  : "
                  << std::chrono::duration_cast<unit>(end.time_since_epoch()).count() << "\n"
        << indent << "dt   : "
                  << std::chrono::duration_cast<unit>((end - start)).count()
        << std::endl
        ;
      --nesting_counter;
    }
    auto operator<<(const bool v) -> decltype(buffer<<v) { return buffer; }
    auto operator<<(const char* v) -> decltype(buffer<<v) { return buffer; }
    auto operator<<(std::string& v) -> decltype(buffer<<v) { return buffer; }
    auto operator<<(int& v) -> decltype(buffer<<v){ return buffer; }
#else
    auto operator<<(const bool v) -> decltype(buffer<<v) { return buffer; }
    auto operator<<(const char* v) -> decltype(buffer<<v) { return buffer; }
    auto operator<<(std::string& v) -> decltype(buffer<<v) { return buffer; }
    auto operator<<(int& v) -> decltype(buffer<<v){ return buffer; }
#endif
  };
#ifndef WRP_NO_LOG  
  int log::nesting_counter = -1;
#endif
}

