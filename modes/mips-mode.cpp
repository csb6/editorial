#include "pattern-match.h"
#include <string>
#include <array>

int main()
{
    Tree t;
    t.add_match("addiu ", "InstructColor");
    t.add_match("addi ", "InstructColor");
    t.add_match("addu ", "InstructColor");
    t.add_match("add ", "InstructColor");
    t.add_match("andi ", "InstructColor");
    t.add_match("and ", "InstructColor");
    t.add_match("beq ", "InstructColor");
    t.add_match("bne ", "InstructColor");
    t.add_match("div ", "InstructColor");
    t.add_match("j ", "InstructColor");
    t.add_match("jal ", "InstructColor");
    t.add_match("jr ", "InstructColor");
    t.add_match("lb ", "InstructColor");
    t.add_match("lw ", "InstructColor");
    t.add_match("mult ", "InstructColor");
    t.add_match("ori ", "InstructColor");
    t.add_match("or ", "InstructColor");
    t.add_match("sb ", "InstructColor");
    t.add_match("sw ", "InstructColor");
    t.add_match("syscall ", "InstructColor");
    t.add_match("subu ", "InstructColor");
    t.add_match("sub ", "InstructColor");
    t.add_match("sll ", "InstructColor");
    t.add_match("slt ", "InstructColor");
    t.add_match("srl ", "InstructColor");
    t.add_match("sra ", "InstructColor");
    t.add_match("$zero", "RegColor");
    t.add_match("$sp", "RegColor");
    t.add_match("$fp", "RegColor");
    t.add_match("$ra", "RegColor");

    // There are $tX registers, $sX registers, $aX registers, and $vX registers
    constexpr int reg_count = 4;
    const std::array<std::string,reg_count> reg_kind{"$t", "$s", "$a", "$v"};
    // How many registers there are for each register kind
    const std::array<int,reg_count>         kind_amt{ 10,   8,    4,    2};
    // Add $t0-$t9, $s0-$s7, $a0-$a3, and $v0-$v1 to list of things to match
    for(int kind = 0; kind < reg_count; ++kind) {
        for(int i = 0; i < kind_amt[kind]; ++i)
            t.add_match(reg_kind[kind] + std::to_string(i), "RegColor");
    }

    const char *func_name = "match_mips";
    const std::string path("build/mips_matcher");

    write_header(path, func_name);
    write_source(path, func_name, t);

    return 0;
}
