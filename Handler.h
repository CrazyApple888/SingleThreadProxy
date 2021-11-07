//
// Created by Андрей Исаченко on 07.11.2021.
//

#ifndef SINGLETHREADPROXY_HANDLER_H
#define SINGLETHREADPROXY_HANDLER_H


class Handler {
public:
    virtual void execute() = 0;
    virtual ~Handler() = default;
};


#endif //SINGLETHREADPROXY_HANDLER_H
