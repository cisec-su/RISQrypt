def compare_hex_strings(hex1, hex2):

    hex1 = hex1.lower().lstrip("0x")
    hex2 = hex2.lower().lstrip("0x")

    max_len = max(len(hex1), len(hex2))
    hex1 = hex1.zfill(max_len)
    hex2 = hex2.zfill(max_len)

    differences = []
    for i, (c1, c2) in enumerate(zip(hex1, hex2)):
        if c1 != c2:
            differences.append((i, c1, c2))

    print(f"Total diff num: {len(differences)}")
    for pos, val1, val2 in differences:
        print(f"Position {pos}: {val1} ≠ {val2}")

    return differences
#1fpga 2py
hex_str1 = "690783566373553c3941c40c11b0b390d556156b65c70bf1fbfe2ecb37d62a7dc0fb7ee4ccc20412000336d9af069631602cfca58fcf647431ee490eae83d469"
hex_str2 = "690783566373553c3941c40c11b0b390d556156b65c70bf1fbfe2ecb37d62a7dc0fb7ee4ccc20412000336d9af069631602cfca58fcf647431ee490eae83d469"
compare_hex_strings(hex_str1, hex_str2)
