import gdb


class ShiftLeftBytes(gdb.Command):
    """Shift left bytes starting from a given address for a given length
    Usage: shift_left_bytes <address> <length>"""
    

    def __init__(self):
        super(ShiftLeftBytes, self).__init__("shift_left_bytes", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)

        if len(args) != 2:
            print("Usage: shift <address> <length>")
            return

        addr = int(args[0], 16)
        length = int(args[1], 10)
        
        print(f"[*] Shifting {length} bytes left starting at {hex(addr)}...")

        for i in range(length):
            gdb.execute(f"set *(char *){addr+i} = *(char *){addr+i+1}")


ShiftLeftBytes()

