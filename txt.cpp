#include <exception>

class TxtException : std::exception {    
public:    
    TxtException(char *m) : msg(m) {};

    virtual const char* what () const throw() {
	return msg;
    }

private:
    char *msg;    
};
