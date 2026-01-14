import math

# === FALCON-512 CONFIGURATION ===
Q = 12289
LOGN = 8          # MATCHING YOUR HARDWARE CONFIG (256 STEPS)
TABLE_SIZE = 256  
ROOT = 7 
MONTGOMERY_R = 4091 

def bit_reverse(k, bits):
    bin_k = bin(k)[2:].zfill(bits)
    return int(bin_k[::-1], 2)

def generate():
    print(f"/* === FALCON-512 TABLES (LogN={LOGN}, R={MONTGOMERY_R}) === */")

    # 1. PSI TABLE (Forward)
    print(f"const uint32_t psi[{TABLE_SIZE}] = {{")
    for i in range(TABLE_SIZE):
        # Critical: Bit-reverse using 8 bits to match HW counter
        br_idx = bit_reverse(i, LOGN)
        val = pow(ROOT, br_idx, Q)
        val = (val * MONTGOMERY_R) % Q  # Montgomery Encode
        
        packed_val = (val << 16) | val  # Broadcast to both lanes
        
        end = "," if i < TABLE_SIZE - 1 else ""
        if i % 4 == 0: print("    ", end="")
        print(f"0x{packed_val:08X}{end}", end=" ")
        if i % 4 == 3: print("")
    print("};\n")

    # 2. PSI_INV TABLE (Inverse)
    root_inv = pow(ROOT, -1, Q)
    print(f"const uint32_t psi_inv[{TABLE_SIZE}] = {{")
    for i in range(TABLE_SIZE):
        br_idx = bit_reverse(i, LOGN)
        val = pow(root_inv, br_idx, Q)
        val = (val * MONTGOMERY_R) % Q  # Montgomery Encode ONLY
        
        packed_val = (val << 16) | val
        
        end = "," if i < TABLE_SIZE - 1 else ""
        if i % 4 == 0: print("    ", end="")
        print(f"0x{packed_val:08X}{end}", end=" ")
        if i % 4 == 3: print("")
    print("};")

if __name__ == "__main__":
    generate()