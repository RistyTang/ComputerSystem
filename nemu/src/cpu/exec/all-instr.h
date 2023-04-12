#include "cpu/exec.h"

make_EHelper(mov);

make_EHelper(operand_size);

make_EHelper(inv);
make_EHelper(nemu_trap);
make_EHelper(call);
make_EHelper(push);
make_EHelper(pop);
make_EHelper(sub);
make_EHelper(xor);
make_EHelper(ret);
//pa2.2
make_EHelper(leave);
make_EHelper(lea);
make_EHelper(cltd);
make_EHelper(movsx);
make_EHelper(movzx);

make_EHelper(add);
make_EHelper(inc);