TESTS = $(check_PROGRAMS)
check_PROGRAMS = \
                 asm_input \
                 asm_input_insns \
                 asm_input_count \
                 program_analysis \
                 program_generator

SOURCES = \
          check.h \
          ../src/generator.c \
          ../src/generator.h \
          ../src/insns.c \
          ../src/insns.def \
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
          ../src/types.h

asm_input_SOURCES = asm_input.c $(SOURCES)
asm_input_insns_SOURCES = asm_input_insns.c $(SOURCES)
asm_input_count_SOURCES = asm_input_count.c $(SOURCES)
program_analysis_SOURCES = program_analysis.c $(SOURCES)
program_generator_SOURCES = program_generator.c $(SOURCES)
