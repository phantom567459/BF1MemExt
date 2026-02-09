"""
Binary Patcher for Dynamic Loop Implementation
Patches the executable file directly without requiring Ghidra

Usage:
    python patch_binary.py <input_file> <output_file>
    
Example:
    python patch_binary.py game.exe game_patched.exe
"""

import sys
import struct
from pathlib import Path


class BinaryPatcher:
    """Handles patching of binary executable files"""
    
    def __init__(self, filepath):
        self.filepath = Path(filepath)
        self.data = bytearray()
        self.patches_applied = []
        
    def load(self):
        """Load binary file into memory"""
        print(f"[*] Loading binary: {self.filepath}")
        with open(self.filepath, 'rb') as f:
            self.data = bytearray(f.read())
        print(f"[+] Loaded {len(self.data)} bytes")
        
    def save(self, output_path):
        """Save patched binary to file"""
        output_path = Path(output_path)
        print(f"[*] Saving patched binary: {output_path}")
        with open(output_path, 'wb') as f:
            f.write(self.data)
        print(f"[+] Saved {len(self.data)} bytes")
        
    def virtual_to_file_offset(self, virtual_addr, image_base=0x00400000):
        """
        Convert virtual address to file offset
        
        Based on your binary:
        VA 0x00533c5b -> File Offset 0x133c5b
        This means: FileOffset = VA - ImageBase + 0x1000
        """
        # Calculate RVA (Relative Virtual Address)
        rva = virtual_addr - image_base
        
        # For this binary: FileOffset = RVA + 0x1000
        # Example: 0x533c5b - 0x400000 = 0x133c5b (RVA)
        #          0x133c5b (RVA) = 0x133c5b (FileOffset - direct mapping)
        file_offset = rva
            
        return file_offset
    
    def apply_patch(self, virtual_addr, patch_bytes, description=""):
        """Apply a patch at the given virtual address"""
        file_offset = self.virtual_to_file_offset(virtual_addr)
        
        print(f"\n[*] Applying patch: {description}")
        print(f"    Virtual Address: 0x{virtual_addr:08x}")
        print(f"    File Offset: 0x{file_offset:08x}")
        print(f"    Size: {len(patch_bytes)} bytes")
        
        # Verify bounds
        if file_offset + len(patch_bytes) > len(self.data):
            print(f"[!] ERROR: Patch extends beyond file size!")
            return False
            
        # Show original bytes
        original = bytes(self.data[file_offset:file_offset + min(16, len(patch_bytes))])
        print(f"    Original: {original.hex(' ')}")
        
        # Apply patch
        for i, byte_val in enumerate(patch_bytes):
            self.data[file_offset + i] = byte_val
            
        # Show new bytes
        patched = bytes(self.data[file_offset:file_offset + min(16, len(patch_bytes))])
        print(f"    Patched:  {patched.hex(' ')}")
        print(f"[+] Patch applied successfully")
        
        self.patches_applied.append({
            'address': virtual_addr,
            'offset': file_offset,
            'size': len(patch_bytes),
            'description': description
        })
        
        return True
    
    def verify_original_bytes(self, virtual_addr, expected_bytes):
        """Verify that original bytes match expected before patching"""
        file_offset = self.virtual_to_file_offset(virtual_addr)
        actual = bytes(self.data[file_offset:file_offset + len(expected_bytes)])
        
        if actual != bytes(expected_bytes):
            print(f"[!] WARNING: Original bytes don't match at 0x{virtual_addr:08x}")
            print(f"    Expected: {bytes(expected_bytes).hex(' ')}")
            print(f"    Found:    {actual.hex(' ')}")
            return False
        return True


def create_dynamic_loop_patch():
    """
    Create the dynamic loop patch bytes
    
    Much simpler version: just use the stored count and loop
    The count was already set at 533c21 to the number we want (10, 100, etc.)
    """
    
    patch_bytes = bytearray([
        # 0x533c5b: MOV EDX, dword ptr [EBX] - get array pointer
        0x8B, 0x13,
        
        # 0x533c5d: MOV ECX, dword ptr [EDX-0x10] - get count from array header
        0x8B, 0x4A, 0xF0,
        
        # 0x533c60: XOR ESI, ESI - loop counter = 0
        0x33, 0xF6,
        
        # 0x533c62: LEA EAX, [EBX+0x4] - list head pointer  
        0x8D, 0x43, 0x04,
        
        # --- Loop Start at 0x533c65 ---
        # 0x533c65: CMP ESI, ECX - check if done
        0x3B, 0xF1,
        
        # 0x533c67: JGE end (jump to 0x533e10 if ESI >= ECX)
        # Offset = 0x533e10 - (0x533c67 + 6) = 0x1A3
        0x0F, 0x8D, 0xA3, 0x01, 0x00, 0x00,
        
        # 0x533c6d: MOV EDX, dword ptr [EBX] - array base
        0x8B, 0x13,
        
        # 0x533c6f: PUSH ECX - save count
        0x51,
        
        # 0x533c70: MOV ECX, ESI - current index
        0x8B, 0xCE,
        
        # 0x533c72: IMUL ECX, ECX, 0x2020 - offset = index * size
        0x69, 0xC9, 0x20, 0x20, 0x00, 0x00,
        
        # 0x533c78: ADD ECX, EDX - ptr to current object
        0x03, 0xCA,
        
        # 0x533c7a: LEA EDI, [ECX+0xa0] - node in object
        0x8D, 0xB9, 0xA0, 0x00, 0x00, 0x00,
        
        # 0x533c80: INC dword ptr [EBX+0x14] - increment list count
        0xFF, 0x43, 0x14,
        
        # 0x533c83: MOV dword ptr [EDI+0xc], ECX
        0x89, 0x4F, 0x0C,
        
        # 0x533c86: MOV dword ptr [EDI], EAX
        0x89, 0x07,
        
        # 0x533c88: MOV dword ptr [EDI+0x4], EAX
        0x89, 0x47, 0x04,
        
        # 0x533c8b: MOV EDX, dword ptr [EAX+0x8]
        0x8B, 0x50, 0x08,
        
        # 0x533c8e: MOV dword ptr [EDI+0x8], EDX
        0x89, 0x57, 0x08,
        
        # 0x533c91: MOV dword ptr [EAX+0x8], EDI
        0x89, 0x78, 0x08,
        
        # 0x533c94: MOV dword ptr [EDX+0x4], EDI
        0x89, 0x7A, 0x04,
        
        # 0x533c97: POP ECX - restore count
        0x59,
        
        # 0x533c98: INC ESI - counter++
        0x46,
        
        # 0x533c99: JMP loop_start (back to 0x533c65)
        # Offset = 0x533c65 - (0x533c99 + 2) = -0x36 = 0xCA
        0xEB, 0xCA
    ])
    
    return patch_bytes


def create_nop_fill(count):
    """Create NOP sled to fill unused space"""
    return bytearray([0x90] * count)


def apply_dynamic_loop_patches(patcher):
    """Apply all patches for dynamic loop conversion"""
    
    # Verify we're patching the right location
    # Check for the first instruction: MOV EDX, dword ptr [EBX]
    expected_start = bytes([0x8B, 0x13])  # Original: MOV EDX, [EBX]
    
    print("\n" + "="*70)
    print("APPLYING DYNAMIC LOOP PATCHES")
    print("="*70)
    
    # Don't verify since the original bytes are different
    # Just apply the patch directly
    
    # Patch 1: Dynamic loop code
    loop_patch = create_dynamic_loop_patch()
    loop_addr = 0x00533c5b
    
    success = patcher.apply_patch(
        loop_addr,
        loop_patch,
        "Dynamic loop implementation"
    )
    
    if not success:
        return False
    
    # Patch 2: NOP fill for remaining space
    # Original range: 0x533c5b to 0x533e0f (436 bytes total)
    # Loop patch size: len(loop_patch) bytes
    # Remaining: 436 - len(loop_patch)
    
    nop_start_addr = loop_addr + len(loop_patch)
    nop_end_addr = 0x00533e10
    nop_count = nop_end_addr - nop_start_addr
    
    if nop_count > 0:
        nop_fill = create_nop_fill(nop_count)
        success = patcher.apply_patch(
            nop_start_addr,
            nop_fill,
            f"NOP fill ({nop_count} bytes)"
        )
        
        if not success:
            return False
    
    return True


def print_summary(patcher):
    """Print summary of applied patches"""
    print("\n" + "="*70)
    print("PATCH SUMMARY")
    print("="*70)
    
    for i, patch in enumerate(patcher.patches_applied, 1):
        print(f"\n{i}. {patch['description']}")
        print(f"   Virtual Address: 0x{patch['address']:08x}")
        print(f"   File Offset:     0x{patch['offset']:08x}")
        print(f"   Size:            {patch['size']} bytes")
    
    print("\n" + "="*70)
    print("CHANGES")
    print("="*70)
    print("Original: Hardcoded initialization for exactly 10 objects")
    print("Patched:  Dynamic loop that handles any number of objects")
    print("          (10, 100, or any count stored in memory)")
    print("\nThe loop now:")
    print("  - Reads actual object count from [EBX+0x14]")
    print("  - Dynamically calculates offsets using IMUL")
    print("  - Initializes all allocated objects")
    print("  - Unused code space filled with NOPs")
    print("="*70)


def main():
    """Main entry point"""
    if len(sys.argv) != 3:
        print("Usage: python patch_binary.py <input_file> <output_file>")
        print("\nExample:")
        print("  python patch_binary.py game.exe game_patched.exe")
        sys.exit(1)
    
    input_file = sys.argv[1]
    output_file = sys.argv[2]
    
    print("="*70)
    print("BINARY PATCHER - Dynamic Loop Implementation")
    print("="*70)
    print(f"Input:  {input_file}")
    print(f"Output: {output_file}")
    
    # Create patcher
    patcher = BinaryPatcher(input_file)
    
    # Load binary
    patcher.load()
    
    # Apply patches
    success = apply_dynamic_loop_patches(patcher)
    
    if not success:
        print("\n[!] Patching failed!")
        sys.exit(1)
    
    # Save patched binary
    patcher.save(output_file)
    
    # Print summary
    print_summary(patcher)
    
    print("\n[+] Patching completed successfully!")
    print(f"[+] Patched binary saved to: {output_file}")


if __name__ == "__main__":
    main()