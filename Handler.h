#ifndef SINGLETHREADPROXY_HANDLER_H
#define SINGLETHREADPROXY_HANDLER_H


class Handler {
public:
    virtual void execute(int event) = 0;
    virtual ~Handler() = default;
};


#endif //SINGLETHREADPROXY_HANDLER_H