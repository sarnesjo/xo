TESTS = $(check_PROGRAMS)
check_PROGRAMS = \
                 asm_input \
                 asm_input_insns \
                 asm_input_count \
                 program_analysis \
                 program_generator \
                 insn_impl_c \
                 program_equivalence_c \
                 insn_impl_bdd \
                 program_equivalence_bdd

SOURCES = \
          check.h \
          ../src/equivalence_checker_bdd.cpp \
          ../src/equivalence_checker_bdd.h \
          ../src/equivalence_checker_c.c \
          ../src/equivalence_checker_c.h \
          ../src/flag_set.c \
          ../src/flag_set.h \
          ../src/generator.c \
          ../src/generator.h \
          ../src/insns.c \
          ../src/insns.h \
          ../src/instruction.c \
          ../src/instruction.h \
          ../src/invocation.c \
          ../src/invocation.h \
          ../src/machine_state.c \
          ../src/machine_state.h \
          ../src/parser.c \
          ../src/parser.h \
          ../src/program.c \
          ../src/program.h \
          ../src/register_set.c \
          ../src/register_set.h \
          ../src/std.h \
          ../src/test_states.c \
          ../src/test_states.h \
          ../src/types.h

asm_input_SOURCES = asm_input.c $(SOURCES)
asm_input_insns_SOURCES = asm_input_insns.c $(SOURCES)
asm_input_count_SOURCES = asm_input_count.c $(SOURCES)
program_analysis_SOURCES = program_analysis.c $(SOURCES)
program_generator_SOURCES = program_generator.c $(SOURCES)

insn_impl_c_SOURCES = insn_impl.c $(SOURCES)
insn_impl_c_CFLAGS = -DXO_TEST_C

program_equivalence_c_SOURCES = program_equivalence.c $(SOURCES)
program_equivalence_c_CFLAGS = -DXO_TEST_C

insn_impl_bdd_SOURCES = insn_impl.c $(SOURCES)
insn_impl_bdd_CFLAGS = -DXO_TEST_BDD

program_equivalence_bdd_SOURCES = program_equivalence.c $(SOURCES)
program_equivalence_bdd_CFLAGS = -DXO_TEST_BDD
