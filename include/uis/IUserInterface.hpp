#ifndef _IUserInterface_HPP_
#define _IUserInterface_HPP_

class IUserInterface {
    public:
        virtual ~IUserInterface() = default;
        virtual void run() = 0;
};

#endif
