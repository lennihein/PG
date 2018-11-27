## API für MiniBug

## Funktionen 

- create_breakpoint
- remove_breakpoint
- singlestep
- next_syscall
- change_syscall
- do_instructions
- poke_reg
- peek_reg
- peek_address
- poke_address
- view_stack
- continue
- raise_signal

step in step over?

### void create_breakpoint(?);

Sets a breakpoint at the given position. 
// todo: specify position

### void singlestep();

Sets a breakpoint after the next instruction (or syscall) and continues the tracee.

### void next_syscall(uint32_t syscall);

Sets breakpoint to the next syscall, gets ignored if this syscall is not the one specified by param. `int syscall`. If `-1` accepts all syscalls.

### void do_instructions(char** instructions);

Injects a payload of one or multiple instructions.
// todo: specify format of instructions

### void poke_reg(ENUM reg, uint64_t value);

Stores `value` into the specified register.
// todo: specify enumeration

### uint64_t peek_reg(ENUM reg);

Returns value of `reg`.

### void poke_reg(ENUM reg, uint64_t val);

Overwrites `reg` with `val`.

### uint64_t peek_address(uint64_t addr);

Returns value stored in `addr`.

### void poke_address(uint64_t addr, uint64_t val);

Overwrites address `addr` with `val`.

### char** view_stack();

Gives a hexadecimal representation of all addresses between `RSP` and `RBP`.

### void continue();

Continues the tracee.

### void raise_signal(ENUM sig);

Raises signal `sig` at tracee.