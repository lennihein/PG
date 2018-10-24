<table>
  <tr>
    <th><h2> <a href="https://pg.bwuah.me/prep-task"> TASK </a> </h2></th>
    <th> <h2> <a href="https://pg.bwuah.me/prep"> SOLUTION </a> </h2></th>
  </tr>
</table>

---

## 1

### a)

```C
// todo
```

### b)

- `int (* call_shell)()` ist eine Deklaration einer Funktion ohne Eingabe mit Ausgabe `int`.
- `int(*)()` ist der Datentyp einer Funktion ohne Eingabe mit Ausgabe `int`.
- `shellcode` ist ein Bitmuster.

Die gesamte Main führt also die Funktion aus, die durch das Bitmuster, welches als String übergeben wird, definiert wird.

### c)

```C
// todo
```

### d)

```asm
section .data

	str: db "Hello World", 10, 0
    str_len equ $ - str -1
    
section .text
global _start

_start:

	mov RAX, 1
	mov RDI, 1
	mov RSI, str
	mov RDX, str_len
	syscall

	mov RAX, 60
	mov RDI, 0
	syscall
```

---

## 2

- Was ist ein Prozess? <br>
-> 
- Was ist ein Thread? <br>
->
- Wie werden Prozesse in Linux identifiziert?  <br>
->
- Was ist der Zweck von clone, fork, execve?  <br>
->
- Was ist ptrace? <br>
->
