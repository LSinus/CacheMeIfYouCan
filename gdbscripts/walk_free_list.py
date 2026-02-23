import gdb

class WalkFreelist(gdb.Command):
    """Walks a SLUB freelist starting from a given head address.
    Usage: walk_freelist <head_addr> <offset>
    Example: walk_freelist 0xffff888004b25840 0x20
    """

    def __init__(self):
        super(WalkFreelist, self).__init__("walk_freelist", gdb.COMMAND_DATA)

    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)
        if len(args) < 2:
            print("Usage: walk_freelist <head_addr> <offset>")
            return

        # Parse hex inputs
        current_obj = int(args[0], 16)
        offset = int(args[1], 16)
        
        print(f"\n[+] Walking Freelist starting at {hex(current_obj)} (Offset: {hex(offset)})")
        print("-" * 60)

        count = 0
        visited = set()

        while current_obj != 0:
            if current_obj in visited:
                print(f"!!! LOOP DETECTED at {hex(current_obj)} !!!")
                break
            
            visited.add(current_obj)
            
            # Address where the 'next' pointer is stored
            next_ptr_addr = current_obj + offset
            
            try:
                # Read the 8-byte pointer from kernel memory
                inferior = gdb.selected_inferior()
                # 'Q' is for unsigned 64-bit integer
                mem = inferior.read_memory(next_ptr_addr, 8)
                import struct
                next_obj = struct.unpack("<Q", mem)[0]
                
                print(f"[{count:02d}] Object: {hex(current_obj)}  -->  Next: {hex(next_obj)}")
                
                current_obj = next_obj
                count += 1
            except Exception as e:
                print(f"Error reading memory at {hex(next_ptr_addr)}: {e}")
                break

        print("-" * 60)
        print(f"[+] Found {count} free objects.\n")

WalkFreelist()
