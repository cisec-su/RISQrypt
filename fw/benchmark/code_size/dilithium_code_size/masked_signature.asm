
dilithium_code_size.elf:     file format elf32-littleriscv


Disassembly of section .init:

Disassembly of section .text:

00002738 <pqcrystals_dilithium_masked_signature.isra.0>:
    2738:	ffff02b7          	lui	t0,0xffff0
    273c:	81010113          	addi	sp,sp,-2032
    2740:	d1028293          	addi	t0,t0,-752 # fffefd10 <__stack_top+0xfffafd14>
    2744:	7e112623          	sw	ra,2028(sp)
    2748:	7e812423          	sw	s0,2024(sp)
    274c:	7e912223          	sw	s1,2020(sp)
    2750:	7f212023          	sw	s2,2016(sp)
    2754:	7d312e23          	sw	s3,2012(sp)
    2758:	7d412c23          	sw	s4,2008(sp)
    275c:	7d512a23          	sw	s5,2004(sp)
    2760:	7d612823          	sw	s6,2000(sp)
    2764:	7d712623          	sw	s7,1996(sp)
    2768:	7d812423          	sw	s8,1992(sp)
    276c:	7d912223          	sw	s9,1988(sp)
    2770:	7da12023          	sw	s10,1984(sp)
    2774:	7bb12e23          	sw	s11,1980(sp)
    2778:	00510133          	add	sp,sp,t0
    277c:	00068493          	mv	s1,a3
    2780:	00e12c23          	sw	a4,24(sp)
    2784:	00a12823          	sw	a0,16(sp)
    2788:	02b12423          	sw	a1,40(sp)
    278c:	00060413          	mv	s0,a2
    2790:	8f5fe0ef          	jal	1084 <pqcrystals_dilithium_poly_init_q>
    2794:	93dfe0ef          	jal	10d0 <pqcrystals_dilithium_masked_gadgets_init_q>
    2798:	01812583          	lw	a1,24(sp)
    279c:	02000613          	li	a2,32
    27a0:	06010513          	addi	a0,sp,96
    27a4:	5f5000ef          	jal	3598 <memcpy>
    27a8:	0000e7b7          	lui	a5,0xe
    27ac:	a7078793          	addi	a5,a5,-1424 # da70 <memcpy+0xa4d8>
    27b0:	00278533          	add	a0,a5,sp
    27b4:	b01ff0ef          	jal	22b4 <x2x_prng_read.constprop.0.isra.0>
    27b8:	01812783          	lw	a5,24(sp)
    27bc:	00000613          	li	a2,0
    27c0:	00200893          	li	a7,2
    27c4:	04078813          	addi	a6,a5,64
    27c8:	01812783          	lw	a5,24(sp)
    27cc:	02078593          	addi	a1,a5,32
    27d0:	0000e7b7          	lui	a5,0xe
    27d4:	a7078793          	addi	a5,a5,-1424 # da70 <memcpy+0xa4d8>
    27d8:	00278733          	add	a4,a5,sp
    27dc:	0c010793          	addi	a5,sp,192
    27e0:	24061863          	bnez	a2,2a30 <pqcrystals_dilithium_masked_signature.isra.0+0x2f8>
    27e4:	0005c503          	lbu	a0,0(a1)
    27e8:	00074303          	lbu	t1,0(a4)
    27ec:	00654533          	xor	a0,a0,t1
    27f0:	00a78023          	sb	a0,0(a5)
    27f4:	00158593          	addi	a1,a1,1
    27f8:	00170713          	addi	a4,a4,1
    27fc:	00178793          	addi	a5,a5,1
    2800:	ff0590e3          	bne	a1,a6,27e0 <pqcrystals_dilithium_masked_signature.isra.0+0xa8>
    2804:	00160793          	addi	a5,a2,1
    2808:	00100613          	li	a2,1
    280c:	fb179ee3          	bne	a5,a7,27c8 <pqcrystals_dilithium_masked_signature.isra.0+0x90>
    2810:	02000613          	li	a2,32
    2814:	08010513          	addi	a0,sp,128
    2818:	581000ef          	jal	3598 <memcpy>
    281c:	ffff27b7          	lui	a5,0xffff2
    2820:	72878793          	addi	a5,a5,1832 # ffff2728 <__stack_top+0xfffb272c>
    2824:	00f12423          	sw	a5,8(sp)
    2828:	01812783          	lw	a5,24(sp)
    282c:	0000e737          	lui	a4,0xe
    2830:	aa070713          	addi	a4,a4,-1376 # daa0 <memcpy+0xa508>
    2834:	06078a13          	addi	s4,a5,96
    2838:	2e078993          	addi	s3,a5,736
    283c:	000037b7          	lui	a5,0x3
    2840:	1c878793          	addi	a5,a5,456 # 31c8 <pqcrystals_dilithium_masked_signature.isra.0+0xa90>
    2844:	00278ab3          	add	s5,a5,sp
    2848:	0000e7b7          	lui	a5,0xe
    284c:	00001bb7          	lui	s7,0x1
    2850:	a7078793          	addi	a5,a5,-1424 # da70 <memcpy+0xa4d8>
    2854:	00270b33          	add	s6,a4,sp
    2858:	414b8b93          	addi	s7,s7,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    285c:	00278933          	add	s2,a5,sp
    2860:	017a87b3          	add	a5,s5,s7
    2864:	fd5b2823          	sw	s5,-48(s6)
    2868:	000a0593          	mv	a1,s4
    286c:	fcfb2a23          	sw	a5,-44(s6)
    2870:	00090513          	mv	a0,s2
    2874:	080a0a13          	addi	s4,s4,128
    2878:	fb0ff0ef          	jal	2028 <pqcrystals_dilithium_masked_poly_ptr_unpack.constprop.0>
    287c:	404a8a93          	addi	s5,s5,1028
    2880:	ff3a10e3          	bne	s4,s3,2860 <pqcrystals_dilithium_masked_signature.isra.0+0x128>
    2884:	01812783          	lw	a5,24(sp)
    2888:	0000ba37          	lui	s4,0xb
    288c:	a10a0a13          	addi	s4,s4,-1520 # aa10 <memcpy+0x7478>
    2890:	5e078793          	addi	a5,a5,1504
    2894:	02f12223          	sw	a5,36(sp)
    2898:	03010793          	addi	a5,sp,48
    289c:	00fa0a33          	add	s4,s4,a5
    28a0:	0000e7b7          	lui	a5,0xe
    28a4:	aa078793          	addi	a5,a5,-1376 # daa0 <memcpy+0xa508>
    28a8:	00002b37          	lui	s6,0x2
    28ac:	00278ab3          	add	s5,a5,sp
    28b0:	818b0b13          	addi	s6,s6,-2024 # 1818 <ntt_lite_pointwise_op+0xac>
    28b4:	016a07b3          	add	a5,s4,s6
    28b8:	fd4aa823          	sw	s4,-48(s5)
    28bc:	fcfaaa23          	sw	a5,-44(s5)
    28c0:	00098593          	mv	a1,s3
    28c4:	00090513          	mv	a0,s2
    28c8:	f60ff0ef          	jal	2028 <pqcrystals_dilithium_masked_poly_ptr_unpack.constprop.0>
    28cc:	02412783          	lw	a5,36(sp)
    28d0:	08098993          	addi	s3,s3,128
    28d4:	404a0a13          	addi	s4,s4,1028
    28d8:	fcf99ee3          	bne	s3,a5,28b4 <pqcrystals_dilithium_masked_signature.isra.0+0x17c>
    28dc:	000067b7          	lui	a5,0x6
    28e0:	9f078793          	addi	a5,a5,-1552 # 59f0 <memcpy+0x2458>
    28e4:	00001737          	lui	a4,0x1
    28e8:	41470713          	addi	a4,a4,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    28ec:	002787b3          	add	a5,a5,sp
    28f0:	04f12423          	sw	a5,72(sp)
    28f4:	00000593          	li	a1,0
    28f8:	00e787b3          	add	a5,a5,a4
    28fc:	01100513          	li	a0,17
    2900:	04f12623          	sw	a5,76(sp)
    2904:	ff4fe0ef          	jal	10f8 <keccak_init.isra.0>
    2908:	00800613          	li	a2,8
    290c:	00000593          	li	a1,0
    2910:	08010513          	addi	a0,sp,128
    2914:	821fe0ef          	jal	1134 <keccak_absorb.isra.0>
    2918:	0024d613          	srli	a2,s1,0x2
    291c:	00000593          	li	a1,0
    2920:	00040513          	mv	a0,s0
    2924:	811fe0ef          	jal	1134 <keccak_absorb.isra.0>
    2928:	01f00793          	li	a5,31
    292c:	03c10513          	addi	a0,sp,60
    2930:	02f12e23          	sw	a5,60(sp)
    2934:	10010413          	addi	s0,sp,256
    2938:	835fe0ef          	jal	116c <keccak_finish.isra.0>
    293c:	01000613          	li	a2,16
    2940:	00000593          	li	a1,0
    2944:	00040513          	mv	a0,s0
    2948:	99dfe0ef          	jal	12e4 <keccak_squeeze.isra.0>
    294c:	00100593          	li	a1,1
    2950:	01100513          	li	a0,17
    2954:	fa4fe0ef          	jal	10f8 <keccak_init.isra.0>
    2958:	0c010513          	addi	a0,sp,192
    295c:	00800613          	li	a2,8
    2960:	02050593          	addi	a1,a0,32
    2964:	fd0fe0ef          	jal	1134 <keccak_absorb.isra.0>
    2968:	100407b7          	lui	a5,0x10040
    296c:	0407a703          	lw	a4,64(a5) # 10040040 <__stack_top+0x10000044>
    2970:	04078793          	addi	a5,a5,64
    2974:	00171693          	slli	a3,a4,0x1
    2978:	0206c863          	bltz	a3,29a8 <pqcrystals_dilithium_masked_signature.isra.0+0x270>
    297c:	01000713          	li	a4,16
    2980:	00e7a223          	sw	a4,4(a5)
    2984:	0087a623          	sw	s0,12(a5)
    2988:	0007a703          	lw	a4,0(a5)
    298c:	40176713          	ori	a4,a4,1025
    2990:	00e7a023          	sw	a4,0(a5)
    2994:	0007a703          	lw	a4,0(a5)
    2998:	fe075ee3          	bgez	a4,2994 <pqcrystals_dilithium_masked_signature.isra.0+0x25c>
    299c:	0007a703          	lw	a4,0(a5)
    29a0:	9ff77713          	andi	a4,a4,-1537
    29a4:	00e7a023          	sw	a4,0(a5)
    29a8:	01f00793          	li	a5,31
    29ac:	03810513          	addi	a0,sp,56
    29b0:	02f12c23          	sw	a5,56(sp)
    29b4:	fb8fe0ef          	jal	116c <keccak_finish.isra.0>
    29b8:	14010513          	addi	a0,sp,320
    29bc:	04050593          	addi	a1,a0,64
    29c0:	01000613          	li	a2,16
    29c4:	921fe0ef          	jal	12e4 <keccak_squeeze.isra.0>
    29c8:	c85fe0ef          	jal	164c <pqcrystals_dilithium_poly_init_ntt>
    29cc:	000037b7          	lui	a5,0x3
    29d0:	1c878793          	addi	a5,a5,456 # 31c8 <pqcrystals_dilithium_masked_signature.isra.0+0xa90>
    29d4:	00278533          	add	a0,a5,sp
    29d8:	000029b7          	lui	s3,0x2
    29dc:	cf5fe0ef          	jal	16d0 <pqcrystals_dilithium_masked_polyvecl_t_ntt>
    29e0:	00000413          	li	s0,0
    29e4:	00600a13          	li	s4,6
    29e8:	81898993          	addi	s3,s3,-2024 # 1818 <ntt_lite_pointwise_op+0xac>
    29ec:	0000b7b7          	lui	a5,0xb
    29f0:	a1078793          	addi	a5,a5,-1520 # aa10 <memcpy+0x7478>
    29f4:	03010713          	addi	a4,sp,48
    29f8:	00e787b3          	add	a5,a5,a4
    29fc:	00f404b3          	add	s1,s0,a5
    2a00:	00000913          	li	s2,0
    2a04:	00048593          	mv	a1,s1
    2a08:	00048513          	mv	a0,s1
    2a0c:	00000613          	li	a2,0
    2a10:	00190913          	addi	s2,s2,1
    2a14:	c79fe0ef          	jal	168c <ntt_lite_ntt_core.isra.0>
    2a18:	40448493          	addi	s1,s1,1028
    2a1c:	ff4914e3          	bne	s2,s4,2a04 <pqcrystals_dilithium_masked_signature.isra.0+0x2cc>
    2a20:	17340ae3          	beq	s0,s3,3394 <pqcrystals_dilithium_masked_signature.isra.0+0xc5c>
    2a24:	00002437          	lui	s0,0x2
    2a28:	81840413          	addi	s0,s0,-2024 # 1818 <ntt_lite_pointwise_op+0xac>
    2a2c:	fc1ff06f          	j	29ec <pqcrystals_dilithium_masked_signature.isra.0+0x2b4>
    2a30:	00074503          	lbu	a0,0(a4)
    2a34:	02a78023          	sb	a0,32(a5)
    2a38:	dbdff06f          	j	27f4 <pqcrystals_dilithium_masked_signature.isra.0+0xbc>
    2a3c:	02012783          	lw	a5,32(sp)
    2a40:	00178793          	addi	a5,a5,1
    2a44:	01079793          	slli	a5,a5,0x10
    2a48:	0107d793          	srli	a5,a5,0x10
    2a4c:	02f12023          	sw	a5,32(sp)
    2a50:	00078463          	beqz	a5,2a58 <pqcrystals_dilithium_masked_signature.isra.0+0x320>
    2a54:	e7cfe0ef          	jal	10d0 <pqcrystals_dilithium_masked_gadgets_init_q>
    2a58:	02012783          	lw	a5,32(sp)
    2a5c:	00500913          	li	s2,5
    2a60:	14010513          	addi	a0,sp,320
    2a64:	03278933          	mul	s2,a5,s2
    2a68:	00000a13          	li	s4,0
    2a6c:	01091913          	slli	s2,s2,0x10
    2a70:	01095913          	srli	s2,s2,0x10
    2a74:	00090593          	mv	a1,s2
    2a78:	811fe0ef          	jal	1288 <dilithium_masked_shake256_stream_init>
    2a7c:	000067b7          	lui	a5,0x6
    2a80:	9f078793          	addi	a5,a5,-1552 # 59f0 <memcpy+0x2458>
    2a84:	002787b3          	add	a5,a5,sp
    2a88:	00f12423          	sw	a5,8(sp)
    2a8c:	00078a93          	mv	s5,a5
    2a90:	000027b7          	lui	a5,0x2
    2a94:	dc878793          	addi	a5,a5,-568 # 1dc8 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x288>
    2a98:	002784b3          	add	s1,a5,sp
    2a9c:	000017b7          	lui	a5,0x1
    2aa0:	5c078793          	addi	a5,a5,1472 # 15c0 <pqcrystals_dilithium_poly_challenge+0x144>
    2aa4:	28048b13          	addi	s6,s1,640
    2aa8:	00278433          	add	s0,a5,sp
    2aac:	000019b7          	lui	s3,0x1
    2ab0:	41498993          	addi	s3,s3,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    2ab4:	013a89b3          	add	s3,s5,s3
    2ab8:	b35d2023          	sw	s5,-1248(s10)
    2abc:	b33d2223          	sw	s3,-1244(s10)
    2ac0:	0a000613          	li	a2,160
    2ac4:	000b0593          	mv	a1,s6
    2ac8:	00048513          	mv	a0,s1
    2acc:	819fe0ef          	jal	12e4 <keccak_squeeze.isra.0>
    2ad0:	00400793          	li	a5,4
    2ad4:	00fa0e63          	beq	s4,a5,2af0 <pqcrystals_dilithium_masked_signature.isra.0+0x3b8>
    2ad8:	00190593          	addi	a1,s2,1
    2adc:	014585b3          	add	a1,a1,s4
    2ae0:	01059593          	slli	a1,a1,0x10
    2ae4:	0105d593          	srli	a1,a1,0x10
    2ae8:	14010513          	addi	a0,sp,320
    2aec:	f9cfe0ef          	jal	1288 <dilithium_masked_shake256_stream_init>
    2af0:	b19fe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    2af4:	01400613          	li	a2,20
    2af8:	00048593          	mv	a1,s1
    2afc:	000a8513          	mv	a0,s5
    2b00:	fb9fe0ef          	jal	1ab8 <ntt_lite_decode.isra.0>
    2b04:	b05fe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    2b08:	00098513          	mv	a0,s3
    2b0c:	01400613          	li	a2,20
    2b10:	000b0593          	mv	a1,s6
    2b14:	fa5fe0ef          	jal	1ab8 <ntt_lite_decode.isra.0>
    2b18:	00040593          	mv	a1,s0
    2b1c:	00040513          	mv	a0,s0
    2b20:	da4ff0ef          	jal	20c4 <pqcrystals_dilithium_masked_gadgets_B2A_2k_ptr>
    2b24:	10040bb7          	lui	s7,0x10040
    2b28:	000807b7          	lui	a5,0x80
    2b2c:	00fba823          	sw	a5,16(s7) # 10040010 <__stack_top+0x10000014>
    2b30:	00040993          	mv	s3,s0
    2b34:	00840c13          	addi	s8,s0,8
    2b38:	ab5fe0ef          	jal	15ec <ntt_lite_set_clr.isra.0>
    2b3c:	0009a583          	lw	a1,0(s3)
    2b40:	00498993          	addi	s3,s3,4
    2b44:	00058513          	mv	a0,a1
    2b48:	f0dfe0ef          	jal	1a54 <ntt_lite_sub_rev_const.isra.0>
    2b4c:	000ba823          	sw	zero,16(s7)
    2b50:	ff3c14e3          	bne	s8,s3,2b38 <pqcrystals_dilithium_masked_signature.isra.0+0x400>
    2b54:	001a0a13          	addi	s4,s4,1
    2b58:	00500793          	li	a5,5
    2b5c:	404a8a93          	addi	s5,s5,1028
    2b60:	f4fa16e3          	bne	s4,a5,2aac <pqcrystals_dilithium_masked_signature.isra.0+0x374>
    2b64:	ae9fe0ef          	jal	164c <pqcrystals_dilithium_poly_init_ntt>
    2b68:	000067b7          	lui	a5,0x6
    2b6c:	9f078793          	addi	a5,a5,-1552 # 59f0 <memcpy+0x2458>
    2b70:	00278533          	add	a0,a5,sp
    2b74:	b5dfe0ef          	jal	16d0 <pqcrystals_dilithium_masked_polyvecl_t_ntt>
    2b78:	000017b7          	lui	a5,0x1
    2b7c:	5c078793          	addi	a5,a5,1472 # 15c0 <pqcrystals_dilithium_poly_challenge+0x144>
    2b80:	002787b3          	add	a5,a5,sp
    2b84:	40478713          	addi	a4,a5,1028
    2b88:	00000593          	li	a1,0
    2b8c:	06010513          	addi	a0,sp,96
    2b90:	04f12823          	sw	a5,80(sp)
    2b94:	04f12c23          	sw	a5,88(sp)
    2b98:	04e12a23          	sw	a4,84(sp)
    2b9c:	04e12e23          	sw	a4,92(sp)
    2ba0:	e30fe0ef          	jal	11d0 <pqcrystals_dilithium_dilithium_shake128_stream_init>
    2ba4:	0000e7b7          	lui	a5,0xe
    2ba8:	a7078793          	addi	a5,a5,-1424 # da70 <memcpy+0xa4d8>
    2bac:	002787b3          	add	a5,a5,sp
    2bb0:	00f12623          	sw	a5,12(sp)
    2bb4:	00078a13          	mv	s4,a5
    2bb8:	000027b7          	lui	a5,0x2
    2bbc:	dc878793          	addi	a5,a5,-568 # 1dc8 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x288>
    2bc0:	00100993          	li	s3,1
    2bc4:	00000c13          	li	s8,0
    2bc8:	00000913          	li	s2,0
    2bcc:	00278433          	add	s0,a5,sp
    2bd0:	10040737          	lui	a4,0x10040
    2bd4:	02a00793          	li	a5,42
    2bd8:	02f72023          	sw	a5,32(a4) # 10040020 <__stack_top+0x10000024>
    2bdc:	007fe7b7          	lui	a5,0x7fe
    2be0:	00178793          	addi	a5,a5,1 # 7fe001 <__stack_top+0x7be005>
    2be4:	ffb90b93          	addi	s7,s2,-5
    2be8:	00f72823          	sw	a5,16(a4)
    2bec:	01703bb3          	snez	s7,s7
    2bf0:	00040b13          	mv	s6,s0
    2bf4:	00000a93          	li	s5,0
    2bf8:	00500c93          	li	s9,5
    2bfc:	008c1613          	slli	a2,s8,0x8
    2c00:	ffca8693          	addi	a3,s5,-4
    2c04:	01360633          	add	a2,a2,s3
    2c08:	00d036b3          	snez	a3,a3
    2c0c:	01061613          	slli	a2,a2,0x10
    2c10:	00dbe6b3          	or	a3,s7,a3
    2c14:	01065613          	srli	a2,a2,0x10
    2c18:	06010593          	addi	a1,sp,96
    2c1c:	000b0513          	mv	a0,s6
    2c20:	efcfe0ef          	jal	131c <pqcrystals_dilithium_poly_uniform_fromhw>
    2c24:	00400793          	li	a5,4
    2c28:	78f99e63          	bne	s3,a5,33c4 <pqcrystals_dilithium_masked_signature.isra.0+0xc8c>
    2c2c:	001c0c13          	addi	s8,s8,1
    2c30:	00000993          	li	s3,0
    2c34:	001a8a93          	addi	s5,s5,1
    2c38:	400b0b13          	addi	s6,s6,1024
    2c3c:	fd9a90e3          	bne	s5,s9,2bfc <pqcrystals_dilithium_masked_signature.isra.0+0x4c4>
    2c40:	003ff7b7          	lui	a5,0x3ff
    2c44:	00812b83          	lw	s7,8(sp)
    2c48:	10040737          	lui	a4,0x10040
    2c4c:	00178793          	addi	a5,a5,1 # 3ff001 <__stack_top+0x3bf005>
    2c50:	00001ab7          	lui	s5,0x1
    2c54:	02f72023          	sw	a5,32(a4) # 10040020 <__stack_top+0x10000024>
    2c58:	05010493          	addi	s1,sp,80
    2c5c:	00000b13          	li	s6,0
    2c60:	414a8a93          	addi	s5,s5,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    2c64:	00040613          	mv	a2,s0
    2c68:	000b8593          	mv	a1,s7
    2c6c:	00000513          	li	a0,0
    2c70:	c19fe0ef          	jal	1888 <ntt_lite_pwm.isra.0>
    2c74:	00100d93          	li	s11,1
    2c78:	00000c93          	li	s9,0
    2c7c:	00ad9513          	slli	a0,s11,0xa
    2c80:	00850533          	add	a0,a0,s0
    2c84:	9a9fe0ef          	jal	162c <ntt_lite_load_twiddle_core.isra.0>
    2c88:	40400793          	li	a5,1028
    2c8c:	02fd85b3          	mul	a1,s11,a5
    2c90:	78000613          	li	a2,1920
    2c94:	000c8513          	mv	a0,s9
    2c98:	001d8d93          	addi	s11,s11,1
    2c9c:	017585b3          	add	a1,a1,s7
    2ca0:	9edfe0ef          	jal	168c <ntt_lite_ntt_core.isra.0>
    2ca4:	00500793          	li	a5,5
    2ca8:	72fd9863          	bne	s11,a5,33d8 <pqcrystals_dilithium_masked_signature.isra.0+0xca0>
    2cac:	015b8bb3          	add	s7,s7,s5
    2cb0:	00448493          	addi	s1,s1,4
    2cb4:	715b1c63          	bne	s6,s5,33cc <pqcrystals_dilithium_masked_signature.isra.0+0xc94>
    2cb8:	9b5fe0ef          	jal	166c <pqcrystals_dilithium_poly_init_invntt>
    2cbc:	94dfe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    2cc0:	000017b7          	lui	a5,0x1
    2cc4:	5c078793          	addi	a5,a5,1472 # 15c0 <pqcrystals_dilithium_poly_challenge+0x144>
    2cc8:	002784b3          	add	s1,a5,sp
    2ccc:	00048593          	mv	a1,s1
    2cd0:	00048513          	mv	a0,s1
    2cd4:	a91fe0ef          	jal	1764 <ntt_lite_backward_ntt.isra.0>
    2cd8:	995fe0ef          	jal	166c <pqcrystals_dilithium_poly_init_invntt>
    2cdc:	40448593          	addi	a1,s1,1028
    2ce0:	00058513          	mv	a0,a1
    2ce4:	a81fe0ef          	jal	1764 <ntt_lite_backward_ntt.isra.0>
    2ce8:	00002b37          	lui	s6,0x2
    2cec:	007fe7b7          	lui	a5,0x7fe
    2cf0:	818b0b13          	addi	s6,s6,-2024 # 1818 <ntt_lite_pointwise_op+0xac>
    2cf4:	10040737          	lui	a4,0x10040
    2cf8:	ff178793          	addi	a5,a5,-15 # 7fdff1 <__stack_top+0x7bdff5>
    2cfc:	016a0b33          	add	s6,s4,s6
    2d00:	00f72823          	sw	a5,16(a4) # 10040010 <__stack_top+0x10000014>
    2d04:	05810493          	addi	s1,sp,88
    2d08:	00000d93          	li	s11,0
    2d0c:	00000513          	li	a0,0
    2d10:	00100a93          	li	s5,1
    2d14:	00a12a23          	sw	a0,20(sp)
    2d18:	8d5fe0ef          	jal	15ec <ntt_lite_set_clr.isra.0>
    2d1c:	0004a583          	lw	a1,0(s1)
    2d20:	01412503          	lw	a0,20(sp)
    2d24:	b75fe0ef          	jal	1898 <ntt_lite_mul_const.isra.0>
    2d28:	020d9663          	bnez	s11,2d54 <pqcrystals_dilithium_masked_signature.isra.0+0x61c>
    2d2c:	003ff7b7          	lui	a5,0x3ff
    2d30:	10040bb7          	lui	s7,0x10040
    2d34:	7ff10513          	addi	a0,sp,2047
    2d38:	00fba823          	sw	a5,16(s7) # 10040010 <__stack_top+0x10000014>
    2d3c:	00000593          	li	a1,0
    2d40:	5c150513          	addi	a0,a0,1473
    2d44:	ce5fe0ef          	jal	1a28 <ntt_lite_add_const.isra.0>
    2d48:	007fe7b7          	lui	a5,0x7fe
    2d4c:	ff178793          	addi	a5,a5,-15 # 7fdff1 <__stack_top+0x7bdff5>
    2d50:	00fba823          	sw	a5,16(s7)
    2d54:	01c12503          	lw	a0,28(sp)
    2d58:	00448493          	addi	s1,s1,4
    2d5c:	695d9a63          	bne	s11,s5,33f0 <pqcrystals_dilithium_masked_signature.isra.0+0xcb8>
    2d60:	7ff10693          	addi	a3,sp,2047
    2d64:	5c168693          	addi	a3,a3,1473
    2d68:	00050613          	mv	a2,a0
    2d6c:	00068593          	mv	a1,a3
    2d70:	b78ff0ef          	jal	20e8 <x2x_ref_a2b.constprop.0.isra.0>
    2d74:	100407b7          	lui	a5,0x10040
    2d78:	01000713          	li	a4,16
    2d7c:	00e7aa23          	sw	a4,20(a5) # 10040014 <__stack_top+0x10000018>
    2d80:	0007ac23          	sw	zero,24(a5)
    2d84:	10000737          	lui	a4,0x10000
    2d88:	00e7ae23          	sw	a4,28(a5)
    2d8c:	01b7a823          	sw	s11,16(a5)
    2d90:	85dfe0ef          	jal	15ec <ntt_lite_set_clr.isra.0>
    2d94:	7ff10593          	addi	a1,sp,2047
    2d98:	5c158593          	addi	a1,a1,1473
    2d9c:	00058513          	mv	a0,a1
    2da0:	af9fe0ef          	jal	1898 <ntt_lite_mul_const.isra.0>
    2da4:	849fe0ef          	jal	15ec <ntt_lite_set_clr.isra.0>
    2da8:	01c12583          	lw	a1,28(sp)
    2dac:	7ff10493          	addi	s1,sp,2047
    2db0:	1c148493          	addi	s1,s1,449
    2db4:	00058513          	mv	a0,a1
    2db8:	ae1fe0ef          	jal	1898 <ntt_lite_mul_const.isra.0>
    2dbc:	7ff10713          	addi	a4,sp,2047
    2dc0:	5c170713          	addi	a4,a4,1473 # 100005c1 <__stack_top+0xffc05c5>
    2dc4:	00000793          	li	a5,0
    2dc8:	40000613          	li	a2,1024
    2dcc:	00072683          	lw	a3,0(a4)
    2dd0:	40072503          	lw	a0,1024(a4)
    2dd4:	00f485b3          	add	a1,s1,a5
    2dd8:	00478793          	addi	a5,a5,4
    2ddc:	00a6c6b3          	xor	a3,a3,a0
    2de0:	00d5a023          	sw	a3,0(a1)
    2de4:	00470713          	addi	a4,a4,4
    2de8:	fec792e3          	bne	a5,a2,2dcc <pqcrystals_dilithium_masked_signature.isra.0+0x694>
    2dec:	a98fe0ef          	jal	1084 <pqcrystals_dilithium_poly_init_q>
    2df0:	000807b7          	lui	a5,0x80
    2df4:	e0078793          	addi	a5,a5,-512 # 7fe00 <__stack_top+0x3fe04>
    2df8:	10040ab7          	lui	s5,0x10040
    2dfc:	00faa823          	sw	a5,16(s5) # 10040010 <__stack_top+0x10000014>
    2e00:	00048593          	mv	a1,s1
    2e04:	00000513          	li	a0,0
    2e08:	a91fe0ef          	jal	1898 <ntt_lite_mul_const.isra.0>
    2e0c:	ffcfe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    2e10:	000015b7          	lui	a1,0x1
    2e14:	03010793          	addi	a5,sp,48
    2e18:	59058593          	addi	a1,a1,1424 # 1590 <pqcrystals_dilithium_poly_challenge+0x114>
    2e1c:	00f585b3          	add	a1,a1,a5
    2e20:	000a0513          	mv	a0,s4
    2e24:	c19fe0ef          	jal	1a3c <ntt_lite_sub_rev.constprop.0.isra.0>
    2e28:	000aa823          	sw	zero,16(s5)
    2e2c:	fc0fe0ef          	jal	15ec <ntt_lite_set_clr.isra.0>
    2e30:	000017b7          	lui	a5,0x1
    2e34:	03010713          	addi	a4,sp,48
    2e38:	59078793          	addi	a5,a5,1424 # 1590 <pqcrystals_dilithium_poly_challenge+0x114>
    2e3c:	00e787b3          	add	a5,a5,a4
    2e40:	40478593          	addi	a1,a5,1028
    2e44:	000b0513          	mv	a0,s6
    2e48:	be1fe0ef          	jal	1a28 <ntt_lite_add_const.isra.0>
    2e4c:	01012783          	lw	a5,16(sp)
    2e50:	00791513          	slli	a0,s2,0x7
    2e54:	00400613          	li	a2,4
    2e58:	00a78533          	add	a0,a5,a0
    2e5c:	00048593          	mv	a1,s1
    2e60:	c09fe0ef          	jal	1a68 <ntt_lite_encode.isra.0>
    2e64:	00190913          	addi	s2,s2,1
    2e68:	00600793          	li	a5,6
    2e6c:	404a0a13          	addi	s4,s4,1028
    2e70:	d6f910e3          	bne	s2,a5,2bd0 <pqcrystals_dilithium_masked_signature.isra.0+0x498>
    2e74:	00000593          	li	a1,0
    2e78:	01100513          	li	a0,17
    2e7c:	a7cfe0ef          	jal	10f8 <keccak_init.isra.0>
    2e80:	01000613          	li	a2,16
    2e84:	00000593          	li	a1,0
    2e88:	10010513          	addi	a0,sp,256
    2e8c:	aa8fe0ef          	jal	1134 <keccak_absorb.isra.0>
    2e90:	01012503          	lw	a0,16(sp)
    2e94:	0c000613          	li	a2,192
    2e98:	00000593          	li	a1,0
    2e9c:	a98fe0ef          	jal	1134 <keccak_absorb.isra.0>
    2ea0:	01f00793          	li	a5,31
    2ea4:	04010513          	addi	a0,sp,64
    2ea8:	04f12023          	sw	a5,64(sp)
    2eac:	0a010413          	addi	s0,sp,160
    2eb0:	abcfe0ef          	jal	116c <keccak_finish.isra.0>
    2eb4:	00800613          	li	a2,8
    2eb8:	00040513          	mv	a0,s0
    2ebc:	00000593          	li	a1,0
    2ec0:	c24fe0ef          	jal	12e4 <keccak_squeeze.isra.0>
    2ec4:	00040593          	mv	a1,s0
    2ec8:	1c010513          	addi	a0,sp,448
    2ecc:	db0fe0ef          	jal	147c <pqcrystals_dilithium_poly_challenge>
    2ed0:	f7cfe0ef          	jal	164c <pqcrystals_dilithium_poly_init_ntt>
    2ed4:	1c010513          	addi	a0,sp,448
    2ed8:	881fe0ef          	jal	1758 <pqcrystals_dilithium_poly_ntt>
    2edc:	00080c37          	lui	s8,0x80
    2ee0:	000087b7          	lui	a5,0x8
    2ee4:	f3bc0c13          	addi	s8,s8,-197 # 7ff3b <__stack_top+0x3ff3f>
    2ee8:	21878793          	addi	a5,a5,536 # 8218 <memcpy+0x4c80>
    2eec:	018aa823          	sw	s8,16(s5)
    2ef0:	002787b3          	add	a5,a5,sp
    2ef4:	00f12a23          	sw	a5,20(sp)
    2ef8:	01412703          	lw	a4,20(sp)
    2efc:	000017b7          	lui	a5,0x1
    2f00:	41478793          	addi	a5,a5,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    2f04:	00f70cb3          	add	s9,a4,a5
    2f08:	000037b7          	lui	a5,0x3
    2f0c:	1c878793          	addi	a5,a5,456 # 31c8 <pqcrystals_dilithium_masked_signature.isra.0+0xa90>
    2f10:	00278933          	add	s2,a5,sp
    2f14:	00812983          	lw	s3,8(sp)
    2f18:	ffff17b7          	lui	a5,0xffff1
    2f1c:	32878793          	addi	a5,a5,808 # ffff1328 <__stack_top+0xfffb132c>
    2f20:	00070413          	mv	s0,a4
    2f24:	02f12623          	sw	a5,44(sp)
    2f28:	000017b7          	lui	a5,0x1
    2f2c:	41478793          	addi	a5,a5,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    2f30:	00f40733          	add	a4,s0,a5
    2f34:	32ed2623          	sw	a4,812(s10)
    2f38:	00f98733          	add	a4,s3,a5
    2f3c:	b2ed2223          	sw	a4,-1244(s10)
    2f40:	7ff10713          	addi	a4,sp,2047
    2f44:	5c170713          	addi	a4,a4,1473
    2f48:	01272023          	sw	s2,0(a4)
    2f4c:	7ff10713          	addi	a4,sp,2047
    2f50:	00f907b3          	add	a5,s2,a5
    2f54:	5cf722a3          	sw	a5,1477(a4)
    2f58:	000017b7          	lui	a5,0x1
    2f5c:	5c078793          	addi	a5,a5,1472 # 15c0 <pqcrystals_dilithium_poly_challenge+0x144>
    2f60:	00278b33          	add	s6,a5,sp
    2f64:	000027b7          	lui	a5,0x2
    2f68:	328d2423          	sw	s0,808(s10)
    2f6c:	b33d2023          	sw	s3,-1248(s10)
    2f70:	7ff10b93          	addi	s7,sp,2047
    2f74:	dc878793          	addi	a5,a5,-568 # 1dc8 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x288>
    2f78:	5c570713          	addi	a4,a4,1477
    2f7c:	018aa823          	sw	s8,16(s5)
    2f80:	5c1b8b93          	addi	s7,s7,1473
    2f84:	00278a33          	add	s4,a5,sp
    2f88:	00000493          	li	s1,0
    2f8c:	000ba583          	lw	a1,0(s7)
    2f90:	1c010613          	addi	a2,sp,448
    2f94:	00000513          	li	a0,0
    2f98:	8f1fe0ef          	jal	1888 <ntt_lite_pwm.isra.0>
    2f9c:	000b2603          	lw	a2,0(s6)
    2fa0:	00000593          	li	a1,0
    2fa4:	00000513          	li	a0,0
    2fa8:	905fe0ef          	jal	18ac <ntt_lite_add.isra.0>
    2fac:	ec0fe0ef          	jal	166c <pqcrystals_dilithium_poly_init_invntt>
    2fb0:	00100793          	li	a5,1
    2fb4:	00f48463          	beq	s1,a5,2fbc <pqcrystals_dilithium_masked_signature.isra.0+0x884>
    2fb8:	e50fe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    2fbc:	000a2503          	lw	a0,0(s4)
    2fc0:	00000593          	li	a1,0
    2fc4:	00148493          	addi	s1,s1,1
    2fc8:	f9cfe0ef          	jal	1764 <ntt_lite_backward_ntt.isra.0>
    2fcc:	00200793          	li	a5,2
    2fd0:	004b8b93          	addi	s7,s7,4
    2fd4:	004b0b13          	addi	s6,s6,4
    2fd8:	004a0a13          	addi	s4,s4,4
    2fdc:	40f49e63          	bne	s1,a5,33f8 <pqcrystals_dilithium_masked_signature.isra.0+0xcc0>
    2fe0:	000015b7          	lui	a1,0x1
    2fe4:	03010793          	addi	a5,sp,48
    2fe8:	59058593          	addi	a1,a1,1424 # 1590 <pqcrystals_dilithium_poly_challenge+0x114>
    2fec:	00002537          	lui	a0,0x2
    2ff0:	00080637          	lui	a2,0x80
    2ff4:	00f585b3          	add	a1,a1,a5
    2ff8:	d9850513          	addi	a0,a0,-616 # 1d98 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x258>
    2ffc:	03010793          	addi	a5,sp,48
    3000:	f3c60613          	addi	a2,a2,-196 # 7ff3c <__stack_top+0x3ff40>
    3004:	00f50533          	add	a0,a0,a5
    3008:	8f4ff0ef          	jal	20fc <pqcrystals_dilithium_masked_poly_ptr_chknorm>
    300c:	a20518e3          	bnez	a0,2a3c <pqcrystals_dilithium_masked_signature.isra.0+0x304>
    3010:	40440413          	addi	s0,s0,1028
    3014:	40498993          	addi	s3,s3,1028
    3018:	40490913          	addi	s2,s2,1028
    301c:	f08c96e3          	bne	s9,s0,2f28 <pqcrystals_dilithium_masked_signature.isra.0+0x7f0>
    3020:	000407b7          	lui	a5,0x40
    3024:	e3c78793          	addi	a5,a5,-452 # 3fe3c <__BSS_END__+0x2f614>
    3028:	10040737          	lui	a4,0x10040
    302c:	00f72823          	sw	a5,16(a4) # 10040010 <__stack_top+0x10000014>
    3030:	00c12783          	lw	a5,12(sp)
    3034:	00002937          	lui	s2,0x2
    3038:	81890913          	addi	s2,s2,-2024 # 1818 <ntt_lite_pointwise_op+0xac>
    303c:	0000bab7          	lui	s5,0xb
    3040:	01278c33          	add	s8,a5,s2
    3044:	a10a8a93          	addi	s5,s5,-1520 # aa10 <memcpy+0x7478>
    3048:	03010793          	addi	a5,sp,48
    304c:	00fa8ab3          	add	s5,s5,a5
    3050:	00c12483          	lw	s1,12(sp)
    3054:	ffff17b7          	lui	a5,0xffff1
    3058:	7ff10a13          	addi	s4,sp,2047
    305c:	32878793          	addi	a5,a5,808 # ffff1328 <__stack_top+0xfffb132c>
    3060:	2a1a0a13          	addi	s4,s4,673
    3064:	02f12623          	sw	a5,44(sp)
    3068:	012487b3          	add	a5,s1,s2
    306c:	32fd2623          	sw	a5,812(s10)
    3070:	32fa2223          	sw	a5,804(s4)
    3074:	012a87b3          	add	a5,s5,s2
    3078:	b2fd2223          	sw	a5,-1244(s10)
    307c:	000017b7          	lui	a5,0x1
    3080:	5c078793          	addi	a5,a5,1472 # 15c0 <pqcrystals_dilithium_poly_challenge+0x144>
    3084:	00278433          	add	s0,a5,sp
    3088:	000027b7          	lui	a5,0x2
    308c:	dc878793          	addi	a5,a5,-568 # 1dc8 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x288>
    3090:	7ff10b13          	addi	s6,sp,2047
    3094:	329d2423          	sw	s1,808(s10)
    3098:	329a2023          	sw	s1,800(s4)
    309c:	b35d2023          	sw	s5,-1248(s10)
    30a0:	00278bb3          	add	s7,a5,sp
    30a4:	5c1b0b13          	addi	s6,s6,1473
    30a8:	00000993          	li	s3,0
    30ac:	00200c93          	li	s9,2
    30b0:	00042583          	lw	a1,0(s0)
    30b4:	1c010613          	addi	a2,sp,448
    30b8:	00000513          	li	a0,0
    30bc:	fccfe0ef          	jal	1888 <ntt_lite_pwm.isra.0>
    30c0:	dacfe0ef          	jal	166c <pqcrystals_dilithium_poly_init_invntt>
    30c4:	00000593          	li	a1,0
    30c8:	00000513          	li	a0,0
    30cc:	e98fe0ef          	jal	1764 <ntt_lite_backward_ntt.isra.0>
    30d0:	00100793          	li	a5,1
    30d4:	00f98463          	beq	s3,a5,30dc <pqcrystals_dilithium_masked_signature.isra.0+0x9a4>
    30d8:	d30fe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    30dc:	000b2583          	lw	a1,0(s6)
    30e0:	000ba503          	lw	a0,0(s7)
    30e4:	00198993          	addi	s3,s3,1
    30e8:	00440413          	addi	s0,s0,4
    30ec:	951fe0ef          	jal	1a3c <ntt_lite_sub_rev.constprop.0.isra.0>
    30f0:	004b8b93          	addi	s7,s7,4
    30f4:	004b0b13          	addi	s6,s6,4
    30f8:	31999463          	bne	s3,s9,3400 <pqcrystals_dilithium_masked_signature.isra.0+0xcc8>
    30fc:	00002537          	lui	a0,0x2
    3100:	00040637          	lui	a2,0x40
    3104:	d9850513          	addi	a0,a0,-616 # 1d98 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x258>
    3108:	03010793          	addi	a5,sp,48
    310c:	e3c60613          	addi	a2,a2,-452 # 3fe3c <__BSS_END__+0x2f614>
    3110:	04810593          	addi	a1,sp,72
    3114:	00f50533          	add	a0,a0,a5
    3118:	fe5fe0ef          	jal	20fc <pqcrystals_dilithium_masked_poly_ptr_chknorm>
    311c:	920510e3          	bnez	a0,2a3c <pqcrystals_dilithium_masked_signature.isra.0+0x304>
    3120:	40448493          	addi	s1,s1,1028
    3124:	404a8a93          	addi	s5,s5,1028
    3128:	f49c10e3          	bne	s8,s1,3068 <pqcrystals_dilithium_masked_signature.isra.0+0x930>
    312c:	01812783          	lw	a5,24(sp)
    3130:	00812903          	lw	s2,8(sp)
    3134:	02412a83          	lw	s5,36(sp)
    3138:	7ff78b13          	addi	s6,a5,2047
    313c:	000409b7          	lui	s3,0x40
    3140:	7a1b0b13          	addi	s6,s6,1953
    3144:	10040a37          	lui	s4,0x10040
    3148:	00001cb7          	lui	s9,0x1
    314c:	5c010413          	addi	s0,sp,1472
    3150:	f0098993          	addi	s3,s3,-256 # 3ff00 <__BSS_END__+0x2f6d8>
    3154:	00100b93          	li	s7,1
    3158:	00d00613          	li	a2,13
    315c:	000a8593          	mv	a1,s5
    3160:	00000513          	li	a0,0
    3164:	019a2823          	sw	s9,16(s4) # 10040010 <__stack_top+0x10000014>
    3168:	951fe0ef          	jal	1ab8 <ntt_lite_decode.isra.0>
    316c:	00000593          	li	a1,0
    3170:	00040513          	mv	a0,s0
    3174:	8e1fe0ef          	jal	1a54 <ntt_lite_sub_rev_const.isra.0>
    3178:	cd4fe0ef          	jal	164c <pqcrystals_dilithium_poly_init_ntt>
    317c:	00040513          	mv	a0,s0
    3180:	dd8fe0ef          	jal	1758 <pqcrystals_dilithium_poly_ntt>
    3184:	00040613          	mv	a2,s0
    3188:	1c010593          	addi	a1,sp,448
    318c:	00090513          	mv	a0,s2
    3190:	ef8fe0ef          	jal	1888 <ntt_lite_pwm.isra.0>
    3194:	cd8fe0ef          	jal	166c <pqcrystals_dilithium_poly_init_invntt>
    3198:	00000513          	li	a0,0
    319c:	013a2823          	sw	s3,16(s4)
    31a0:	00090593          	mv	a1,s2
    31a4:	dc0fe0ef          	jal	1764 <ntt_lite_backward_ntt.isra.0>
    31a8:	00000513          	li	a0,0
    31ac:	945fe0ef          	jal	1af0 <ntt_lite_chknorm>
    31b0:	897506e3          	beq	a0,s7,2a3c <pqcrystals_dilithium_masked_signature.isra.0+0x304>
    31b4:	004a2783          	lw	a5,4(s4)
    31b8:	00179713          	slli	a4,a5,0x1
    31bc:	24075663          	bgez	a4,3408 <pqcrystals_dilithium_masked_signature.isra.0+0xcd0>
    31c0:	1a0a8a93          	addi	s5,s5,416
    31c4:	40090913          	addi	s2,s2,1024
    31c8:	f96a98e3          	bne	s5,s6,3158 <pqcrystals_dilithium_masked_signature.isra.0+0xa20>
    31cc:	00c12403          	lw	s0,12(sp)
    31d0:	00002937          	lui	s2,0x2
    31d4:	81890913          	addi	s2,s2,-2024 # 1818 <ntt_lite_pointwise_op+0xac>
    31d8:	00040493          	mv	s1,s0
    31dc:	01240633          	add	a2,s0,s2
    31e0:	00040593          	mv	a1,s0
    31e4:	00048513          	mv	a0,s1
    31e8:	40440413          	addi	s0,s0,1028
    31ec:	ec0fe0ef          	jal	18ac <ntt_lite_add.isra.0>
    31f0:	40048493          	addi	s1,s1,1024
    31f4:	ff8414e3          	bne	s0,s8,31dc <pqcrystals_dilithium_masked_signature.isra.0+0xaa4>
    31f8:	01012783          	lw	a5,16(sp)
    31fc:	03d00613          	li	a2,61
    3200:	00000593          	li	a1,0
    3204:	7ff78493          	addi	s1,a5,2047
    3208:	4a148493          	addi	s1,s1,1185
    320c:	00048513          	mv	a0,s1
    3210:	2ac000ef          	jal	34bc <memset>
    3214:	000407b7          	lui	a5,0x40
    3218:	10040737          	lui	a4,0x10040
    321c:	f0078793          	addi	a5,a5,-256 # 3ff00 <__BSS_END__+0x2f6d8>
    3220:	00f72823          	sw	a5,16(a4) # 10040010 <__stack_top+0x10000014>
    3224:	007be7b7          	lui	a5,0x7be
    3228:	10178793          	addi	a5,a5,257 # 7be101 <__stack_top+0x77e105>
    322c:	02f72023          	sw	a5,32(a4)
    3230:	01012783          	lw	a5,16(sp)
    3234:	00000a93          	li	s5,0
    3238:	aa010b13          	addi	s6,sp,-1376
    323c:	7ff78993          	addi	s3,a5,2047
    3240:	30078b93          	addi	s7,a5,768
    3244:	00078a13          	mv	s4,a5
    3248:	000027b7          	lui	a5,0x2
    324c:	dc878793          	addi	a5,a5,-568 # 1dc8 <pqcrystals_dilithium_verify.constprop.0.isra.0+0x288>
    3250:	4d898993          	addi	s3,s3,1240
    3254:	00278433          	add	s0,a5,sp
    3258:	03700c13          	li	s8,55
    325c:	00400613          	li	a2,4
    3260:	000a0593          	mv	a1,s4
    3264:	00040513          	mv	a0,s0
    3268:	851fe0ef          	jal	1ab8 <ntt_lite_decode.isra.0>
    326c:	00812603          	lw	a2,8(sp)
    3270:	00c12583          	lw	a1,12(sp)
    3274:	00000513          	li	a0,0
    3278:	e34fe0ef          	jal	18ac <ntt_lite_add.isra.0>
    327c:	00812503          	lw	a0,8(sp)
    3280:	00000793          	li	a5,0
    3284:	00000713          	li	a4,0
    3288:	60000693          	li	a3,1536
    328c:	00040613          	mv	a2,s0
    3290:	00000593          	li	a1,0
    3294:	cd8fe0ef          	jal	176c <ntt_lite_pointwise_op>
    3298:	00000793          	li	a5,0
    329c:	00000713          	li	a4,0
    32a0:	48000693          	li	a3,1152
    32a4:	00000613          	li	a2,0
    32a8:	00000593          	li	a1,0
    32ac:	04410513          	addi	a0,sp,68
    32b0:	cbcfe0ef          	jal	176c <ntt_lite_pointwise_op>
    32b4:	5a4b2783          	lw	a5,1444(s6)
    32b8:	16fc7663          	bgeu	s8,a5,3424 <pqcrystals_dilithium_masked_signature.isra.0+0xcec>
    32bc:	5a4b2783          	lw	a5,1444(s6)
    32c0:	03700713          	li	a4,55
    32c4:	f6f76c63          	bltu	a4,a5,2a3c <pqcrystals_dilithium_masked_signature.isra.0+0x304>
    32c8:	01012503          	lw	a0,16(sp)
    32cc:	02000613          	li	a2,32
    32d0:	0a010593          	addi	a1,sp,160
    32d4:	2c4000ef          	jal	3598 <memcpy>
    32d8:	100407b7          	lui	a5,0x10040
    32dc:	00080737          	lui	a4,0x80
    32e0:	00e7a823          	sw	a4,16(a5) # 10040010 <__stack_top+0x10000014>
    32e4:	01012783          	lw	a5,16(sp)
    32e8:	01412903          	lw	s2,20(sp)
    32ec:	000019b7          	lui	s3,0x1
    32f0:	02078413          	addi	s0,a5,32
    32f4:	41498993          	addi	s3,s3,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    32f8:	01412503          	lw	a0,20(sp)
    32fc:	01390633          	add	a2,s2,s3
    3300:	00090593          	mv	a1,s2
    3304:	da8fe0ef          	jal	18ac <ntt_lite_add.isra.0>
    3308:	01412583          	lw	a1,20(sp)
    330c:	00000513          	li	a0,0
    3310:	40490913          	addi	s2,s2,1028
    3314:	f40fe0ef          	jal	1a54 <ntt_lite_sub_rev_const.isra.0>
    3318:	00040513          	mv	a0,s0
    331c:	01400613          	li	a2,20
    3320:	00000593          	li	a1,0
    3324:	f44fe0ef          	jal	1a68 <ntt_lite_encode.isra.0>
    3328:	01412783          	lw	a5,20(sp)
    332c:	28040413          	addi	s0,s0,640
    3330:	40078793          	addi	a5,a5,1024
    3334:	00f12a23          	sw	a5,20(sp)
    3338:	fc8490e3          	bne	s1,s0,32f8 <pqcrystals_dilithium_masked_signature.isra.0+0xbc0>
    333c:	02812703          	lw	a4,40(sp)
    3340:	000017b7          	lui	a5,0x1
    3344:	cdd78793          	addi	a5,a5,-803 # cdd <_start-0x323>
    3348:	000102b7          	lui	t0,0x10
    334c:	00f72023          	sw	a5,0(a4) # 80000 <__stack_top+0x40004>
    3350:	2f028293          	addi	t0,t0,752 # 102f0 <psi_inv+0x2f0>
    3354:	00510133          	add	sp,sp,t0
    3358:	7ec12083          	lw	ra,2028(sp)
    335c:	7e812403          	lw	s0,2024(sp)
    3360:	7e412483          	lw	s1,2020(sp)
    3364:	7e012903          	lw	s2,2016(sp)
    3368:	7dc12983          	lw	s3,2012(sp)
    336c:	7d812a03          	lw	s4,2008(sp)
    3370:	7d412a83          	lw	s5,2004(sp)
    3374:	7d012b03          	lw	s6,2000(sp)
    3378:	7cc12b83          	lw	s7,1996(sp)
    337c:	7c812c03          	lw	s8,1992(sp)
    3380:	7c412c83          	lw	s9,1988(sp)
    3384:	7c012d03          	lw	s10,1984(sp)
    3388:	7bc12d83          	lw	s11,1980(sp)
    338c:	7f010113          	addi	sp,sp,2032
    3390:	00008067          	ret
    3394:	000027b7          	lui	a5,0x2
    3398:	aa078793          	addi	a5,a5,-1376 # 1aa0 <ntt_lite_encode.isra.0+0x38>
    339c:	00278d33          	add	s10,a5,sp
    33a0:	ffff07b7          	lui	a5,0xffff0
    33a4:	32078793          	addi	a5,a5,800 # ffff0320 <__stack_top+0xfffb0324>
    33a8:	00f12423          	sw	a5,8(sp)
    33ac:	7ff10793          	addi	a5,sp,2047
    33b0:	5c178793          	addi	a5,a5,1473
    33b4:	40078793          	addi	a5,a5,1024
    33b8:	02012023          	sw	zero,32(sp)
    33bc:	00f12e23          	sw	a5,28(sp)
    33c0:	e98ff06f          	j	2a58 <pqcrystals_dilithium_masked_signature.isra.0+0x320>
    33c4:	00198993          	addi	s3,s3,1
    33c8:	86dff06f          	j	2c34 <pqcrystals_dilithium_masked_signature.isra.0+0x4fc>
    33cc:	00001b37          	lui	s6,0x1
    33d0:	414b0b13          	addi	s6,s6,1044 # 1414 <pqcrystals_dilithium_poly_uniform_eta_fromhw+0x48>
    33d4:	891ff06f          	j	2c64 <pqcrystals_dilithium_masked_signature.isra.0+0x52c>
    33d8:	00400793          	li	a5,4
    33dc:	00000c93          	li	s9,0
    33e0:	88fd9ee3          	bne	s11,a5,2c7c <pqcrystals_dilithium_masked_signature.isra.0+0x544>
    33e4:	0004ac83          	lw	s9,0(s1)
    33e8:	a20fe0ef          	jal	1608 <ntt_lite_set_clr_with_twiddle.isra.0>
    33ec:	891ff06f          	j	2c7c <pqcrystals_dilithium_masked_signature.isra.0+0x544>
    33f0:	00100d93          	li	s11,1
    33f4:	921ff06f          	j	2d14 <pqcrystals_dilithium_masked_signature.isra.0+0x5dc>
    33f8:	000d8493          	mv	s1,s11
    33fc:	b91ff06f          	j	2f8c <pqcrystals_dilithium_masked_signature.isra.0+0x854>
    3400:	000d8993          	mv	s3,s11
    3404:	cadff06f          	j	30b0 <pqcrystals_dilithium_masked_signature.isra.0+0x978>
    3408:	012a2623          	sw	s2,12(s4)
    340c:	000a2783          	lw	a5,0(s4)
    3410:	0077e793          	ori	a5,a5,7
    3414:	00fa2023          	sw	a5,0(s4)
    3418:	004a2783          	lw	a5,4(s4)
    341c:	fe07dee3          	bgez	a5,3418 <pqcrystals_dilithium_masked_signature.isra.0+0xce0>
    3420:	da1ff06f          	j	31c0 <pqcrystals_dilithium_masked_signature.isra.0+0xa88>
    3424:	00812703          	lw	a4,8(sp)
    3428:	00000793          	li	a5,0
    342c:	10000693          	li	a3,256
    3430:	00072603          	lw	a2,0(a4)
    3434:	00060863          	beqz	a2,3444 <pqcrystals_dilithium_masked_signature.isra.0+0xd0c>
    3438:	01548633          	add	a2,s1,s5
    343c:	00f60023          	sb	a5,0(a2)
    3440:	001a8a93          	addi	s5,s5,1
    3444:	00178793          	addi	a5,a5,1
    3448:	00470713          	addi	a4,a4,4
    344c:	fed792e3          	bne	a5,a3,3430 <pqcrystals_dilithium_masked_signature.isra.0+0xcf8>
    3450:	00c12783          	lw	a5,12(sp)
    3454:	01598023          	sb	s5,0(s3)
    3458:	080a0a13          	addi	s4,s4,128
    345c:	40078793          	addi	a5,a5,1024
    3460:	00f12623          	sw	a5,12(sp)
    3464:	00812783          	lw	a5,8(sp)
    3468:	00198993          	addi	s3,s3,1
    346c:	40078793          	addi	a5,a5,1024
    3470:	00f12423          	sw	a5,8(sp)
    3474:	df4b94e3          	bne	s7,s4,325c <pqcrystals_dilithium_masked_signature.isra.0+0xb24>
    3478:	04412783          	lw	a5,68(sp)
    347c:	e45ff06f          	j	32c0 <pqcrystals_dilithium_masked_signature.isra.0+0xb88>
