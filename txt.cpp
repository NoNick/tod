#include <exception>
#include <string>

class TxtException : std::exception {    
public:    
    TxtException(std::string m) : msg(m) {};

    virtual const char* what () const throw() {
	return msg.c_str();
    }

private:
    std::string msg;
};
