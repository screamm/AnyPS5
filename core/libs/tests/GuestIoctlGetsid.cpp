#include "prx/libc/include/general/VabiMacros.hpp"
#include <cstdlib>
#include <initializer_list>
extern "C" {
int APS5_VABI getsid_nid_postfix(int);
int APS5_VABI getpid_nid_postfix(void);
int* APS5_VABI __error_nid_postfix();
}
static void Require(bool value) { if (!value) std::abort(); }
static void Fails(int pid) {
    *__error_nid_postfix() = 0;
    Require(getsid_nid_postfix(pid) == -1 && *__error_nid_postfix() == 3);
}
int main() {
    const int self = getpid_nid_postfix();
    *__error_nid_postfix() = 0;
    Require(getsid_nid_postfix(0) == self && getsid_nid_postfix(self) == self && *__error_nid_postfix() == 0);
    for (int pid : {self + 1, -1, -self}) Fails(pid);
}
