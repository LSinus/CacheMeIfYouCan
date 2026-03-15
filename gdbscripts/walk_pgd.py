import gdb

class WalkPageTable(gdb.Command):
    """Walks the page table for a given virtual address.
    Usage: walk_pgd <virtual_address>"""

    def __init__(self):
        super(WalkPageTable, self).__init__("walk_pgd", gdb.COMMAND_USER)
        self.OFFSET = 0xffff888000000000


    def walk(self, pgd_virt, pgd_idx, pud_idx, pmd_idx, pte_idx):
        # 4. Read PGD Entry
        pgd_entry = int(gdb.parse_and_eval(f"*(unsigned long *)({pgd_virt} + {pgd_idx} * 8)"))
        s = f"PGD Entry [{pgd_idx}]: {hex(pgd_entry)}"
        print(f"{s:<40} PT saved at: {hex(pgd_virt + pgd_idx * 8)}")

        if not (pgd_entry & 1):
            print("!! PGD Entry not present.")
            return

        # 5. Read PUD Entry
        pud_phys = pgd_entry & ~0xFFF
        pud_virt = pud_phys + self.OFFSET
        pud_entry = int(gdb.parse_and_eval(f"*(unsigned long *)({pud_virt} + {pud_idx} * 8)"))
        s = f"PUD Entry [{pud_idx}]: {hex(pud_entry)}"
        print(f"{s:<40} PT saved at: {hex(pud_virt + pud_idx * 8)}")

        if not (pud_entry & 1):
            print("!! PUD Entry not present.")
            return

        # Check for 1GB Huge Page (Bit 7)
        if (pud_entry & 0x80):
            print(">> Found 1GB Huge Page Mapping!")
            print(f">> Physical Target: {hex(pud_entry & 0x000FFFFFFFFFF000)}")
            return

        # 6. Read PMD Entry
        pmd_phys = pud_entry & ~0xFFF
        pmd_virt = pmd_phys + self.OFFSET
        pmd_entry = int(gdb.parse_and_eval(f"*(unsigned long *)({pmd_virt} + {pmd_idx} * 8)"))
        s = f"PMD Entry [{pmd_idx}]: {hex(pmd_entry)}"
        print(f"{s:<40} PT saved at: {hex(pmd_virt + pmd_idx * 8)}")

        if not (pmd_entry & 1):
            print("!! PMD Entry not present.")
            return

        # Check for 2MB Huge Page (Bit 7)
        if (pmd_entry & 0x80):
            print(">> Found 2MB Huge Page Mapping!")
            print(f">> Physical Target: {hex(pmd_entry & 0x000FFFFFFFFFF000)}")
            return

        # 7. Read PTE Entry
        pte_phys = pmd_entry & ~0xFFF
        pte_virt = pte_phys + self.OFFSET
        pte_entry = int(gdb.parse_and_eval(f"*(unsigned long *)({pte_virt} + {pte_idx} * 8)"))
        s = f"PTE Entry [{pte_idx}]: {hex(pte_entry)}"
        print(f"{s:<40} PT saved at: {hex(pte_virt + pte_idx * 8)}")
        print(f">> Physical Target: {hex(pte_entry & 0x000FFFFFFFFFF000)}")



    def invoke(self, arg, from_tty):
        args = gdb.string_to_argv(arg)
        if len(args) != 1:
            print("Usage: walk_pgd <virtual_address>")
            return

        vaddr = int(args[0], 16)
        
        pgd_idx = (vaddr >> 39) & 0x1FF
        pud_idx = (vaddr >> 30) & 0x1FF
        pmd_idx = (vaddr >> 21) & 0x1FF
        pte_idx = (vaddr >> 12) & 0x1FF

        print(f"\nWalking tables for Virtual Address: {hex(vaddr)}")
        print(f"Indices: PGD: {pgd_idx}, PUD: {pud_idx}, PMD: {pmd_idx}, PTE: {pte_idx}")
        print("-" * 60)

        try:
            cr3 = int(gdb.parse_and_eval("$cr3"))
            cr3_phys = cr3 & ~0xFFF
            is_user_pgd = bool(cr3 & (1 << 12))

            if is_user_pgd:
                pgd_virt_u = cr3_phys + self.OFFSET
                pgd_virt_k = pgd_virt_u - 4096
            else:
                pgd_virt_k = cr3_phys + self.OFFSET
                pgd_virt_u = pgd_virt_k + 4096
            
            print("KERNL VERSION PAGE TABLE")
            self.walk(pgd_virt_k, pgd_idx, pud_idx, pmd_idx, pte_idx)

            print("")
            print("USER VERSION PAGE TABLE")
            self.walk(pgd_virt_u, pgd_idx, pud_idx, pmd_idx, pte_idx)
        except Exception as e:
            print(f"Error during walk: {e}")

WalkPageTable()
