import gdb

class SearchTLB(gdb.Command):
    """Search for an entry in TLB for the given virtual address
    Usage: search_tlb <virtual_address>"""
    

    def __init__(self):
        super(SearchTLB, self).__init__("search_tlb", gdb.COMMAND_USER)

    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)

        if len(args) != 1:
            print("Usage: search_tlb <virtual_address>")
            return

        vaddr = int(args[0], 16)

        vaddr_page = vaddr & ~0xFFF
        print(f"Searching TLB for page: {hex(vaddr_page)}...")

        tlb_output = gdb.execute("monitor info tlb", False, True)

        found = False
        needle = hex(vaddr_page)[2:]
        for line in tlb_output.splitlines():
            if needle in line.lower():
                print(f"[TLB HIT] {line.strip()}")
                found = True

        if not found:
            print("[TLB MISS] No cached entry found for this address.")

SearchTLB()
