#include <msg.h>
int main(void) {
    Msg_t source = {0}, copy = {0};
    source.head.function = 7;
    source.head.length = sizeof(MsgHead_t);
    MsgCopy(&copy, &source);
    return copy.head.function != 7;
}
